/***************************************************//**
 * @file    QE65000SpectrometerFeature.cpp
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *******************************************************/

#include "common/globals.h"
#include "vendors/OceanOptics/features/eeprom_slots/WavelengthEEPROMSlotFeature_QE65000.h"
#include "vendors/OceanOptics/features/spectrometer/QE65000SpectrometerFeature.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/IntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/QESpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/RequestSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/TriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOISpectrometerProtocol.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

const long QE65000SpectrometerFeature::INTEGRATION_TIME_MINIMUM = 8000;
const long QE65000SpectrometerFeature::INTEGRATION_TIME_MAXIMUM = 1600000000;
const long QE65000SpectrometerFeature::INTEGRATION_TIME_INCREMENT = 1000;
const long QE65000SpectrometerFeature::INTEGRATION_TIME_BASE = 1000;

QE65000SpectrometerFeature::QE65000SpectrometerFeature() {

    this->numberOfPixels = 1044;
    this->maxIntensity = 65535;
	this->numberOfBytesPerPixel = sizeof(unsigned short);

    int readoutLength = (1024 + 256)*2 + 1;

    this->integrationTimeMinimum = QE65000SpectrometerFeature::INTEGRATION_TIME_MINIMUM;
    this->integrationTimeMaximum = QE65000SpectrometerFeature::INTEGRATION_TIME_MAXIMUM;
    this->integrationTimeBase = QE65000SpectrometerFeature::INTEGRATION_TIME_BASE;
    this->integrationTimeIncrement = QE65000SpectrometerFeature::INTEGRATION_TIME_INCREMENT;

    for(int i = 0; i < 4; i++) {
        this->electricDarkPixelIndices.push_back(i);
    }
    for(int i = 1040; i < 1044; i++) {
        this->electricDarkPixelIndices.push_back(i);
    }

    IntegrationTimeExchange *intTime = new IntegrationTimeExchange(QE65000SpectrometerFeature::INTEGRATION_TIME_BASE);

    Transfer *requestFormattedSpectrum = new RequestSpectrumExchange();
	Transfer *readFormattedSpectrum = new QESpectrumExchange(readoutLength, this->numberOfPixels);
	Transfer *requestUnformattedSpectrum = new RequestSpectrumExchange();
	Transfer *readUnformattedSpectrum = new ReadSpectrumExchange(readoutLength, this->numberOfPixels);
	Transfer *requestFastBufferSpectrum = new RequestSpectrumExchange();
	Transfer *readFastBufferSpectrum = new ReadSpectrumExchange(readoutLength, this->numberOfPixels);

    TriggerModeExchange *triggerMode = new TriggerModeExchange();

    OOISpectrometerProtocol *ooiProtocol = new OOISpectrometerProtocol(intTime, requestFormattedSpectrum, readFormattedSpectrum,
		requestUnformattedSpectrum, readUnformattedSpectrum, requestFastBufferSpectrum, readFastBufferSpectrum, triggerMode);
    this->protocols.push_back(ooiProtocol);

    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_NORMAL));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SOFTWARE));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_HARDWARE));

}

QE65000SpectrometerFeature::~QE65000SpectrometerFeature() {

}

vector<double> *QE65000SpectrometerFeature::getWavelengths(
            const Protocol &protocol, const Bus &bus) {

    WavelengthEEPROMSlotFeature_QE65000 wlFeature(this->numberOfPixels);

    return wlFeature.readWavelengths(protocol, bus);
}
