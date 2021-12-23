/***************************************************//**
 * @file    FPGASpectrumExchange.cpp
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
#include "common/Log.h"
#include <vector>
#include "vendors/OceanOptics/protocols/ooi/exchanges/FPGASpectrumExchange.h"
#include "common/UShortVector.h"
#include "common/exceptions/ProtocolFormatException.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

FPGASpectrumExchange::FPGASpectrumExchange(
        unsigned int readoutLength, unsigned int numberOfPixels)
        : ReadSpectrumExchange(readoutLength, numberOfPixels) {

}

FPGASpectrumExchange::~FPGASpectrumExchange() {

}

Data *FPGASpectrumExchange::transfer(TransferHelper *helper) {
    LOG(__FUNCTION__);

    unsigned int i;
    Data *xfer;
    unsigned char lsb;
    unsigned char msb;

    /* Use the superclass to move the data into a local buffer. */
    xfer = Transfer::transfer(helper);
    if(NULL == xfer) {
        string error("FPGASpectrumExchange::transfer: "
                "Expected Transfer::transfer to produce a non-null result "
                "containing raw spectral data.  Without this data, it is not possible to "
                "generate a valid formatted spectrum.");
        logger.error(error.c_str());
        throw ProtocolException(error);
    }


    /* xfer is just a copy of what is already stored in this, so delete it. */
    delete xfer;
    /* At this point, this->buffer should have the raw spectrum data. */

    /* In this style of transfer, the last byte should be 0x69.  If it is not, then
     * we have probably lost synchronization with the data stream.
     */
    if((*(this->buffer))[this->length - 1] != 0x69) {
        string synchError("FPGASpectrumExchange::transfer: "
                "Did not find expected synch byte (0x69) at the end of spectral data "
                "transfer.  This suggests that the data stream is now out of synchronization, "
                "or possibly that an underlying read operation failed prematurely due to bus "
                "issues.");
        logger.error(synchError.c_str());
        throw ProtocolFormatException(synchError);
    }

    /* Get a local variable by reference to point to that buffer */
    vector<unsigned short> formatted(this->numberOfPixels);

    for(i = 0; i < this->numberOfPixels; i++) {
        lsb = (*(this->buffer))[i * 2];
        msb = ((*(this->buffer))[(i * 2) + 1]);
        formatted[i] = ((msb << 8) & 0x00FF00) | (lsb & 0x00FF);
    }

    UShortVector *retval = new UShortVector(formatted);

    return retval;
}
