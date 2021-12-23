/***************************************************//**
 * @file    Maya2000ProSpectrometerFeature.cpp
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
#include "vendors/OceanOptics/features/spectrometer/Maya2000ProSpectrometerFeature.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/IntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/MayaProSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/RequestSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/TriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOISpectrometerProtocol.h"
#include "vendors/OceanOptics/features/eeprom_slots/EEPROMSlotFeature.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

const long Maya2000ProSpectrometerFeature::INTEGRATION_TIME_MINIMUM = 7200; // 7.2 millisec
const long Maya2000ProSpectrometerFeature::INTEGRATION_TIME_MAXIMUM = 65000000; // 65 sec
const long Maya2000ProSpectrometerFeature::INTEGRATION_TIME_INCREMENT = 1000;
const long Maya2000ProSpectrometerFeature::INTEGRATION_TIME_BASE = 1;

Maya2000ProSpectrometerFeature::Maya2000ProSpectrometerFeature(
        ProgrammableSaturationFeature *saturationFeature)
            : GainAdjustedSpectrometerFeature(saturationFeature) {

    this->numberOfPixels = 2068;
	this->numberOfBytesPerPixel = sizeof(unsigned short);
    this->maxIntensity = 64000;   // MZ: resolves unit-to-unit issues with S10420 detector
    int readoutLength = 2304 * 2 + 1;

    this->integrationTimeMinimum = Maya2000ProSpectrometerFeature::INTEGRATION_TIME_MINIMUM;
    this->integrationTimeMaximum = Maya2000ProSpectrometerFeature::INTEGRATION_TIME_MAXIMUM;
    this->integrationTimeBase = Maya2000ProSpectrometerFeature::INTEGRATION_TIME_BASE;
    this->integrationTimeIncrement = Maya2000ProSpectrometerFeature::INTEGRATION_TIME_INCREMENT;

    for(int i = 0; i < 4; i++) {
        this->electricDarkPixelIndices.push_back(i);
    }
    for(int i = 2064; i < 2068; i++) {
        this->electricDarkPixelIndices.push_back(i);
    }

    IntegrationTimeExchange *intTime = new IntegrationTimeExchange(Maya2000ProSpectrometerFeature::INTEGRATION_TIME_BASE);

	Transfer *requestFormattedSpectrum = new RequestSpectrumExchange();
    Transfer *readFormattedSpectrum = new MayaProSpectrumExchange(readoutLength, this->numberOfPixels, this);
	Transfer *requestUnformattedSpectrum = new RequestSpectrumExchange();
	Transfer *readUnformattedSpectrum = new ReadSpectrumExchange(readoutLength, this->numberOfPixels);
	Transfer *requestFastBufferSpectrum = new RequestSpectrumExchange();
	Transfer *readFastBufferSpectrum = new ReadSpectrumExchange(readoutLength, this->numberOfPixels);

    TriggerModeExchange *triggerMode = new TriggerModeExchange();

    OOISpectrometerProtocol *ooiProtocol = new OOISpectrometerProtocol(intTime, requestFormattedSpectrum, readFormattedSpectrum,
		requestUnformattedSpectrum, readUnformattedSpectrum, requestFastBufferSpectrum, readFastBufferSpectrum, triggerMode);
    this->protocols.push_back(ooiProtocol);

    this->triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_NORMAL));
    this->triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SOFTWARE));
    this->triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SYNCHRONIZATION));
    this->triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_HARDWARE));
}

Maya2000ProSpectrometerFeature::~Maya2000ProSpectrometerFeature() {

}
