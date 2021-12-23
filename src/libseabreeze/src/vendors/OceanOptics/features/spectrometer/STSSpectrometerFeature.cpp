/***************************************************//**
 * @file    STSSpectrometerFeature.cpp
 * @date    January 2011
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
#include "vendors/OceanOptics/features/spectrometer/STSSpectrometerFeature.h"
#include "vendors/OceanOptics/features/wavecal/WaveCalFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/WaveCalProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPIntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPRequestSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPTriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPSpectrometerProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPWaveCalProtocol.h"

using namespace seabreeze;
using namespace oceanBinaryProtocol;
using namespace std;

const long STSSpectrometerFeature::INTEGRATION_TIME_MINIMUM = 10;
const long STSSpectrometerFeature::INTEGRATION_TIME_MAXIMUM = 85000000;
const long STSSpectrometerFeature::INTEGRATION_TIME_INCREMENT = 1;
const long STSSpectrometerFeature::INTEGRATION_TIME_BASE = 1;

STSSpectrometerFeature::STSSpectrometerFeature()
    : binningFactor(0) {

    this->numberOfPixels = unbinnedNumberOfPixels;
	this->numberOfBytesPerPixel = sizeof(unsigned short);
    this->maxIntensity = 16383;

    this->integrationTimeMinimum = STSSpectrometerFeature::INTEGRATION_TIME_MINIMUM;
    this->integrationTimeMaximum = STSSpectrometerFeature::INTEGRATION_TIME_MAXIMUM;
    this->integrationTimeBase = STSSpectrometerFeature::INTEGRATION_TIME_BASE;
    this->integrationTimeIncrement = STSSpectrometerFeature::INTEGRATION_TIME_INCREMENT;

    OBPIntegrationTimeExchange *intTime = new OBPIntegrationTimeExchange(
            STSSpectrometerFeature::INTEGRATION_TIME_BASE);



	Transfer *requestFormattedSpectrum = new OBPRequestSpectrumExchange();
    readFormattedSpectrum = new OBPReadSpectrumExchange((this->numberOfPixels * 2) + 64, this->numberOfPixels);
	Transfer *requestUnformattedSpectrum = new OBPRequestSpectrumExchange();
	readUnformattedSpectrum = new OBPReadRawSpectrumExchange((this->numberOfPixels * 2) + 64, this->numberOfPixels);
	Transfer *requestFastBufferSpectrum = new OBPRequestSpectrumExchange();
	readFastBufferSpectrum = new OBPReadRawSpectrumExchange((this->numberOfPixels * 2) + 64, this->numberOfPixels);
    OBPTriggerModeExchange *triggerMode = new OBPTriggerModeExchange();

    OBPSpectrometerProtocol *obpProtocol = new OBPSpectrometerProtocol(intTime, requestFormattedSpectrum, readFormattedSpectrum,
		requestUnformattedSpectrum, readUnformattedSpectrum, requestFastBufferSpectrum, readFastBufferSpectrum, triggerMode);
    this->protocols.push_back(obpProtocol);

    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_OBP_NORMAL));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_OBP_EXTERNAL));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_OBP_INTERNAL));

}

STSSpectrometerFeature::~STSSpectrometerFeature() {

}

void STSSpectrometerFeature::setPixelBinningFactor(unsigned char factor) {
    binningFactor = factor;
    numberOfPixels = unbinnedNumberOfPixels >> binningFactor;
    readUnformattedSpectrum->setNumberOfPixels(numberOfPixels * 2 + 64, numberOfPixels);
	readFastBufferSpectrum->setNumberOfPixels(numberOfPixels * 2 + 64, numberOfPixels);
    readFormattedSpectrum->setNumberOfPixels(numberOfPixels * 2 + 64, numberOfPixels);
}

vector<double> *STSSpectrometerFeature::getWavelengths(const Protocol &protocol,
            const Bus &bus) {

    /* FIXME: this probably ought to attempt to create an instance based on
     * any available protocol.  However, this should suffice.
     */
    vector<ProtocolHelper *> helpers;
    helpers.push_back(new OBPWaveCalProtocol());

    WaveCalFeature WaveCal(helpers, unbinnedNumberOfPixels);

    vector<double> *wavelengths = WaveCal.readWavelengths(protocol, bus);
    vector<double> &w = *wavelengths;

    if (binningFactor > 0) {
        const size_t step = (size_t)1 << binningFactor;
        for (size_t start = 0, destination = 0; start < unbinnedNumberOfPixels; start += step, ++destination) {
            double sum = 0.0;
            for (size_t i = start, count = 0; count < step; ++i, ++count) {
                sum += w[i];
            }
            w[destination] = sum / step;
        }
        wavelengths->resize(numberOfPixels);
    }

    return wavelengths;
}
