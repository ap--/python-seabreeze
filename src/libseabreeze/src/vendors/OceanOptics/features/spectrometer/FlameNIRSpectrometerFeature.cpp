/***************************************************//**
 * @file    FlameNIRSpectrometerFeature.cpp
 * @date    Apr 2016
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2016, Ocean Optics Inc
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
#include "vendors/OceanOptics/features/spectrometer/FlameNIRSpectrometerFeature.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/IntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/FlameNIRSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/RequestSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/TriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOISpectrometerProtocol.h"

using namespace seabreeze;
using namespace ooiProtocol;

const long FlameNIRSpectrometerFeature::INTEGRATION_TIME_MINIMUM   = 1000;
const long FlameNIRSpectrometerFeature::INTEGRATION_TIME_MAXIMUM   = 655350000;
const long FlameNIRSpectrometerFeature::INTEGRATION_TIME_INCREMENT = 1000;
const long FlameNIRSpectrometerFeature::INTEGRATION_TIME_BASE      = 1;

FlameNIRSpectrometerFeature::FlameNIRSpectrometerFeature(
        ProgrammableSaturationFeature *saturationFeature)
            : GainAdjustedSpectrometerFeature(saturationFeature) {

    numberOfPixels = 128;
	this->numberOfBytesPerPixel = sizeof(unsigned short);
    maxIntensity = 65535;

    integrationTimeMinimum   = INTEGRATION_TIME_MINIMUM;
    integrationTimeMaximum   = INTEGRATION_TIME_MAXIMUM;
    integrationTimeBase      = INTEGRATION_TIME_BASE;
    integrationTimeIncrement = INTEGRATION_TIME_INCREMENT;

    // NB: Flame-NIR has no electrical dark pixels

    IntegrationTimeExchange *intTime = new IntegrationTimeExchange(INTEGRATION_TIME_BASE);
    Transfer *requestFormattedSpectrum = new RequestSpectrumExchange();
	Transfer *readFormattedSpectrum = new FlameNIRSpectrumExchange(numberOfPixels * 2, numberOfPixels, this);
	Transfer *requestUnformattedSpectrum = new RequestSpectrumExchange();
	Transfer *readUnformattedSpectrum = new ReadSpectrumExchange(numberOfPixels * 2, numberOfPixels);
	Transfer *requestFastBufferSpectrum = new RequestSpectrumExchange();
	Transfer *readFastBufferSpectrum = new ReadSpectrumExchange(numberOfPixels * 2, numberOfPixels);

    TriggerModeExchange *triggerMode = new TriggerModeExchange();

    OOISpectrometerProtocol *ooiProtocol = new OOISpectrometerProtocol(intTime, requestFormattedSpectrum, readFormattedSpectrum,
		requestUnformattedSpectrum, readUnformattedSpectrum, requestFastBufferSpectrum, readFastBufferSpectrum, triggerMode);
    protocols.push_back(ooiProtocol);

    triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_NORMAL));
    triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SOFTWARE));
    triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SYNCHRONIZATION));
    triggerModes.push_back(new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_HARDWARE));
}

FlameNIRSpectrometerFeature::~FlameNIRSpectrometerFeature() { }
