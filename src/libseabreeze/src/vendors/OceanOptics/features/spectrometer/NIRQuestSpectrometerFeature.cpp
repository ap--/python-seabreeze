/***************************************************//**
 * @file    NIRQuestSpectrometerFeature.cpp
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
#include "vendors/OceanOptics/features/spectrometer/NIRQuestSpectrometerFeature.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/IntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/NIRQuestSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/RequestSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOISpectrometerProtocol.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/TriggerModeExchange.h"
#include "vendors/OceanOptics/features/eeprom_slots/EEPROMSlotFeature.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

const long NIRQuestSpectrometerFeature::INTEGRATION_TIME_MINIMUM = 1000;
const long NIRQuestSpectrometerFeature::INTEGRATION_TIME_MAXIMUM = 1600000000;
const long NIRQuestSpectrometerFeature::INTEGRATION_TIME_INCREMENT = 1000;
const long NIRQuestSpectrometerFeature::INTEGRATION_TIME_BASE = 1000;

NIRQuestSpectrometerFeature::NIRQuestSpectrometerFeature(
        ProgrammableSaturationFeature *saturationFeature)
            : GainAdjustedSpectrometerFeature(saturationFeature) {
    this->maxIntensity = 65535;

    this->integrationTimeMinimum = NIRQuestSpectrometerFeature::INTEGRATION_TIME_MINIMUM;
    this->integrationTimeMaximum = NIRQuestSpectrometerFeature::INTEGRATION_TIME_MAXIMUM;
    this->integrationTimeBase = NIRQuestSpectrometerFeature::INTEGRATION_TIME_BASE;
    this->integrationTimeIncrement = NIRQuestSpectrometerFeature::INTEGRATION_TIME_INCREMENT;

    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_NORMAL));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SOFTWARE));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SYNCHRONIZATION));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_HARDWARE));
}

NIRQuestSpectrometerFeature::~NIRQuestSpectrometerFeature() {

}

/*
 * This is called by the derived NIRQuest 256 and 512 classes as part of
 * their constructor.  This is deferred until after the usual superclass
 * constructor because some details, like the readout length, will not yet
 * have been computed.  Those could probably be passed on an initializer
 * list to a base class constructor, but this works too.
 */
void NIRQuestSpectrometerFeature::setupExchanges(int readoutLength) {

    IntegrationTimeExchange *intTime = new IntegrationTimeExchange(NIRQuestSpectrometerFeature::INTEGRATION_TIME_BASE);
    Transfer *requestFormattedSpectrum = new RequestSpectrumExchange();
	Transfer *readFormattedSpectrum = new NIRQuestSpectrumExchange(readoutLength, this->numberOfPixels, this);
	Transfer *requestUnformattedSpectrum = new RequestSpectrumExchange();
	Transfer *readUnformattedSpectrum = new ReadSpectrumExchange(readoutLength, this->numberOfPixels);
	Transfer *requestFastBufferSpectrum = new RequestSpectrumExchange();
	Transfer *readFastBufferSpectrum = new ReadSpectrumExchange(readoutLength, this->numberOfPixels);

    TriggerModeExchange *triggerMode = new TriggerModeExchange();

    OOISpectrometerProtocol *ooiProtocol = new OOISpectrometerProtocol(intTime, requestFormattedSpectrum, readFormattedSpectrum,
		requestUnformattedSpectrum, readUnformattedSpectrum, requestFastBufferSpectrum, readFastBufferSpectrum, triggerMode);

    this->protocols.push_back(ooiProtocol);
}
