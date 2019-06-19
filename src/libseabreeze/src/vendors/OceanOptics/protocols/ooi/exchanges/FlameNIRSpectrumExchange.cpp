/***************************************************//**
 * @file    FlameNIRSpectrumExchange.cpp
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

#include <vector>

#include "common/globals.h"
#include "common/Log.h"
#include "common/UShortVector.h"
#include "common/DoubleVector.h"
#include "common/exceptions/ProtocolFormatException.h"

#include "vendors/OceanOptics/protocols/ooi/exchanges/FlameNIRSpectrumExchange.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

FlameNIRSpectrumExchange::FlameNIRSpectrumExchange(
    unsigned int readoutLength, unsigned int numberOfPixels,
            GainAdjustedSpectrometerFeature *spectrometer)
        : ReadSpectrumExchange(readoutLength, numberOfPixels) {
    this->spectrometerFeature = spectrometer;
}

FlameNIRSpectrumExchange::~FlameNIRSpectrumExchange() { }

Data *FlameNIRSpectrumExchange::transfer(TransferHelper *helper)
        throw (ProtocolException) {

    LOG(__FUNCTION__);

    unsigned int i;
    Data *xfer;
    double maxIntensity;
    double saturationLevel;
    byte lsb;
    byte msb;

    // Use the superclass to move the data into a local buffer. 
    // This transfer() may cause a ProtocolException to be thrown. 
    xfer = Transfer::transfer(helper);
    if(NULL == xfer) {
        string error("FlameNIRSpectrumExchange::transfer: "
            "Expected Transfer::transfer to produce a non-null result "
            "containing raw spectral data.  Without this data, it is not possible to "
            "generate a valid formatted spectrum.");
        logger.error(error.c_str());
        throw ProtocolException(error);
    }

    // At this point, this->buffer should have the raw spectrum data. 

    // We would normally check for synchronization byte here, but Flame-NIR 
    // does not send one.

    // Get a local variable by reference to point to that buffer 
    logger.debug("demarshalling");
    vector<unsigned short> formatted(this->numberOfPixels);
    for(i = 0; i < this->numberOfPixels; i++) {
        lsb = (*(this->buffer))[i * 2];
        msb = ((*(this->buffer))[(i * 2) + 1]);
        formatted[i] = ((msb << 8) & 0x00ff00) | (lsb & 0x00ff);
    }

    // confirm we can gain-adjust
    if(NULL == this->spectrometerFeature) {
        // FIXME: should this throw an illegal state exception instead? 
        return xfer;
    }

    // xfer is just a copy of what is already stored in this, so delete it. 
    delete xfer;

    maxIntensity = this->spectrometerFeature->getMaximumIntensity();
    saturationLevel = this->spectrometerFeature->getSaturationLevel();

    // Cast the formatted values so that we can get to the array of shorts 
    UShortVector *usv = new UShortVector(formatted);
    vector<unsigned short> shortVec = usv->getUShortVector();

    // Create a local buffer to store the gain-adjusted values  
    vector<double> adjusted(this->numberOfPixels);

    for(i = 0; i < this->numberOfPixels; i++) {
        double temp = shortVec[i] * maxIntensity / saturationLevel;
        if(temp > maxIntensity) {
            temp = maxIntensity;
        }
        adjusted[i] = temp;
    }

    // It might speed things up to dynamically allocate the buffer and
    // hand it off to retval rather than letting it make a copy.
    DoubleVector *retval = new DoubleVector(adjusted);

    return retval;
}
