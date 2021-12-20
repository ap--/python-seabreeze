/***************************************************//**
 * @file    MayaProSpectrumExchange.cpp
 * @date    November 2009
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
#include <vector>
#include "vendors/OceanOptics/protocols/ooi/exchanges/MayaProSpectrumExchange.h"
#include "common/DoubleVector.h"
#include "common/exceptions/ProtocolFormatException.h"
#include "common/Log.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

MayaProSpectrumExchange::MayaProSpectrumExchange(
        unsigned int readoutLength, unsigned int numberOfPixels,
        GainAdjustedSpectrometerFeature *spectrometer)
            : ReadSpectrumExchange(readoutLength, numberOfPixels) {
    this->spectrometerFeature = spectrometer;
}

MayaProSpectrumExchange::~MayaProSpectrumExchange() {

}

Data *MayaProSpectrumExchange::transfer(TransferHelper *helper) {
    LOG(__FUNCTION__);

    unsigned int i;
    Data *xfer;
    unsigned char lsb;
    unsigned char msb;
    double maxIntensity;
    double saturationLevel;
    double scalingFactor;

    /* Use the superclass to move the data into a local buffer. */
    xfer = Transfer::transfer(helper);
    if(NULL == xfer) {
        string error("MayaProSpectrumExchange::transfer: "
                "Expected Transfer::transfer to produce a non-null result "
                "containing raw spectral data.  Without this data, it is not possible to "
                "generate a valid formatted spectrum.");
        logger.error(error.c_str());
        throw ProtocolException(error);
    }

    if(NULL == this->spectrometerFeature) {
        /* FIXME: should this throw an illegal state exception instead? */
        logger.error("no spectrometerFeature");
        return xfer;
    }

    maxIntensity = this->spectrometerFeature->getMaximumIntensity();
    saturationLevel = this->spectrometerFeature->getSaturationLevel();

    scalingFactor = maxIntensity/(double)saturationLevel;

    /* xfer is just a copy of what is already stored in this, so delete it. */
    delete xfer;
    /* At this point, this->buffer should have the raw spectrum data. */

    /* In this style of transfer, the last byte should be 0x69.  If it is not, then
     * we have probably lost synchronization with the data stream.
     */
    if((*(this->buffer))[this->length - 1] != 0x69) {
        string synchError("MayaProSpectrumExchange::transfer: "
                "Did not find expected synch byte (0x69) at the end of spectral data "
                "transfer.  This suggests that the data stream is now out of synchronization, "
                "or possibly that an underlying read operation failed prematurely due to bus "
                "issues.");
        logger.error(synchError.c_str());
        throw ProtocolFormatException(synchError);
    }

    /* Get a local variable by reference to point to that buffer */
    vector<double> formatted(this->numberOfPixels);

    for(i = 0; i < this->numberOfPixels; i++) {
        unsigned int pixel;
        double processedPixel;
        lsb = (*(this->buffer))[i * 2];
        msb = ((*(this->buffer))[(i * 2) + 1]);
        /* Assemble the MSB and LSB into a single 16-bit integer */
        pixel = ((msb << 8) & 0x00FF00) | (lsb & 0x00FF);
        /* Cleanly convert to a double */
        processedPixel = pixel;
        if(processedPixel >= saturationLevel) {
            /* If we had a saturation indicator, it would be set here */
            processedPixel = saturationLevel;
        }
        /* Apply the gain adjustment */
        processedPixel =  pixel * scalingFactor;
        if(processedPixel > maxIntensity) {
            processedPixel = maxIntensity;
        }

        formatted[i] = processedPixel;
        // logger.debug("MayaProSpectrumExchange::transfer: autonulling changed pixel %4u from %8u to %8.2lf (%5.2lf%%)", i, pixel, formatted[i], 100.0 * formatted[i] / pixel);
    }
    DoubleVector *retval = new DoubleVector(formatted);

    return retval;
}
