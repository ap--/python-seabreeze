/***************************************************//**
 * @file    QEProSpectrometerFeature.cpp
 * @date    September 2013
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
#include "vendors/OceanOptics/features/spectrometer/QEProSpectrometerFeature.h"
#include "vendors/OceanOptics/features/wavecal/WaveCalFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/WaveCalProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPIntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadSpectrum32AndMetadataExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPRequestBufferedSpectrum32AndMetadataExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPTriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPSpectrometerProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPWaveCalProtocol.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

const long QEProSpectrometerFeature::INTEGRATION_TIME_MINIMUM = 8000;
const long QEProSpectrometerFeature::INTEGRATION_TIME_MAXIMUM = 1600000000;
const long QEProSpectrometerFeature::INTEGRATION_TIME_INCREMENT = 1;
const long QEProSpectrometerFeature::INTEGRATION_TIME_BASE = 1;

QEProSpectrometerFeature::QEProSpectrometerFeature() {

    this->numberOfPixels = 1044;
	this->numberOfBytesPerPixel = sizeof(unsigned int);
    this->maxIntensity = 200000; // technically 18-bit, but internally limited

    this->integrationTimeMinimum = QEProSpectrometerFeature::INTEGRATION_TIME_MINIMUM;
    this->integrationTimeMaximum = QEProSpectrometerFeature::INTEGRATION_TIME_MAXIMUM;
    this->integrationTimeBase = QEProSpectrometerFeature::INTEGRATION_TIME_BASE;
    this->integrationTimeIncrement = QEProSpectrometerFeature::INTEGRATION_TIME_INCREMENT;

    /* This device has two bands of electric dark pixels, one at each end of the
     * pixel array.
     */
    for(int i = 0; i < 4; i++) {
        this->electricDarkPixelIndices.push_back(i);
    }
    for(int i = 1040; i < 1044; i++) {
        this->electricDarkPixelIndices.push_back(i);
    }

    OBPIntegrationTimeExchange *intTime = new OBPIntegrationTimeExchange(QEProSpectrometerFeature::INTEGRATION_TIME_BASE);
	Transfer *requestFormattedSpectrum = new OBPRequestBufferedSpectrum32AndMetadataExchange();
    Transfer *readFormattedSpectrum = new OBPReadSpectrum32AndMetadataExchange(this->numberOfPixels);
	Transfer *requestUnformattedSpectrum = new OBPRequestBufferedSpectrum32AndMetadataExchange();
	Transfer *readUnformattedSpectrum = new OBPReadRawSpectrum32AndMetadataExchange(this->numberOfPixels);
	Transfer *requestFastBufferSpectrum = new OBPRequestBufferedSpectrum32AndMetadataExchange();
	Transfer *readFastBufferSpectrum = new OBPReadRawSpectrum32AndMetadataExchange(this->numberOfPixels);

    OBPTriggerModeExchange *triggerMode = new OBPTriggerModeExchange();

    OBPSpectrometerProtocol *obpProtocol = new OBPSpectrometerProtocol(intTime, requestFormattedSpectrum, readFormattedSpectrum,
		requestUnformattedSpectrum,readUnformattedSpectrum, requestFastBufferSpectrum, readFastBufferSpectrum, triggerMode);
    this->protocols.push_back(obpProtocol);

    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_NORMAL));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_LEVEL));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_SYNCHRONIZATION));
    this->triggerModes.push_back(
        new SpectrometerTriggerMode(SPECTROMETER_TRIGGER_MODE_EDGE));

}

QEProSpectrometerFeature::~QEProSpectrometerFeature() {

}

vector<double> *QEProSpectrometerFeature::getWavelengths(const Protocol &protocol,
            const Bus &bus) {

    /* FIXME: this probably ought to attempt to create an instance based on
     * any available protocol.  However, this should suffice.
     */
    vector<ProtocolHelper *> helpers;
    helpers.push_back(new OBPWaveCalProtocol());

    WaveCalFeature WaveCal(helpers, this->numberOfPixels);

    return WaveCal.readWavelengths(protocol, bus);
}
