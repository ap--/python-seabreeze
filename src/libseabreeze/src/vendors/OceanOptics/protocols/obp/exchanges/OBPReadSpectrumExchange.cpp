/***************************************************//**
 * @file    OBPReadSpectrumExchange.cpp
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadSpectrumExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPSpectrumHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPMessage.h"
#include "common/UShortVector.h"
#include "common/ByteVector.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPReadSpectrumExchange::OBPReadSpectrumExchange(
        unsigned int readoutLength, unsigned int numPixels)
        : OBPReadRawSpectrumExchange(readoutLength, numPixels) {

}

OBPReadSpectrumExchange::~OBPReadSpectrumExchange() {

}

Data *OBPReadSpectrumExchange::transfer(TransferHelper *helper)
        throw (ProtocolException) {
    Data *xfer;
    byte lsb;
    byte msb;

    /* This will use the superclass to transfer data from the device, and will
     * then strip off the message header and footer so that only the
     * pixel data remains.
     */
    xfer = OBPReadRawSpectrumExchange::transfer(helper);
    if(NULL == xfer) {
        string error("Expected Transfer::transfer to produce a non-null result "
                        "containing raw spectral data.  Without this data, it is not "
                        "possible to generate a valid formatted spectrum.");
        throw ProtocolException(error);
    }
    /* xfer should contain a ByteVector */

    /* Extract the pixel data from the byte vector */
    ByteVector *bv = static_cast<ByteVector *>(xfer);
    vector<byte> bytes = bv->getByteVector();

    vector<unsigned short> formatted(this->numberOfPixels);
    for(unsigned int i = 0; i < this->numberOfPixels; i++) {
        lsb = bytes[i * 2];
        msb = bytes[(i * 2) + 1];
        formatted[i] = ((msb & 0x00FF) << 8) | (lsb & 0x00FF);
    }
    delete xfer;  /* Equivalent to deleting bv and bytes */

    UShortVector *retval = new UShortVector(formatted);

    return retval;
}
