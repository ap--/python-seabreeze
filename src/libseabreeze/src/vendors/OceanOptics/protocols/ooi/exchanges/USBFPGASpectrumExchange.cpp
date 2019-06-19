/***************************************************//**
 * @file    USBFPGASpectrumExchange.cpp
 * @date    July 2009
 * @author  Ocean Optics, Inc.
 *
 * This is intended for the USB2000+ and USB4000 that
 * require gain (saturation level) adjustment
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
#include "vendors/OceanOptics/protocols/ooi/exchanges/USBFPGASpectrumExchange.h"
#include "common/UShortVector.h"
#include "common/DoubleVector.h"
#include "common/exceptions/ProtocolFormatException.h"
#include "common/Log.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

USBFPGASpectrumExchange::USBFPGASpectrumExchange(
    unsigned int readoutLength, unsigned int numberOfPixels,
            GainAdjustedSpectrometerFeature *spectrometer)
        : FPGASpectrumExchange(readoutLength, numberOfPixels) {
    this->spectrometerFeature = spectrometer;
}

USBFPGASpectrumExchange::~USBFPGASpectrumExchange() {

}

Data *USBFPGASpectrumExchange::transfer(TransferHelper *helper)
        throw (ProtocolException) {

    LOG(__FUNCTION__);

    unsigned int i;
    Data *xfer;
    double maxIntensity;
    double saturationLevel;

    /* Use the superclass to get an array of formatted by uncorrected values. */
    xfer = FPGASpectrumExchange::transfer(helper);
    if(NULL == xfer) {
        string error("Expected FPGASpectrumExchange::transfer to produce a non-null result "
                "containing raw spectral data.  Without this data, it is not possible to "
                "generate a valid formatted spectrum.");
        logger.error(error.c_str());
        throw ProtocolException(error);
    }

    if(NULL == this->spectrometerFeature) {
        /* FIXME: should this throw an illegal state exception instead? */
        return xfer;
    }

    maxIntensity = this->spectrometerFeature->getMaximumIntensity();
    saturationLevel = this->spectrometerFeature->getSaturationLevel();

    /* Cast the formatted values so that we can get to the array of shorts */
    UShortVector *usv = static_cast<UShortVector *>(xfer);
    vector<unsigned short> shortVec = usv->getUShortVector();

    /* Create a buffer to store the gain-adjusted values.  This is local. */
    vector<double> adjusted(this->numberOfPixels);

    for(i = 0; i < this->numberOfPixels; i++) {
        double temp = shortVec[i] * maxIntensity / saturationLevel;
        if(temp > maxIntensity) {
            temp = maxIntensity;
        }
        adjusted[i] = temp;
    }

    /* It might speed things up to dynamically allocate the buffer and
     * hand it off to retval rather than letting it make a copy.
     */
    DoubleVector *retval = new DoubleVector(adjusted);

    delete xfer;

    return retval;
}
