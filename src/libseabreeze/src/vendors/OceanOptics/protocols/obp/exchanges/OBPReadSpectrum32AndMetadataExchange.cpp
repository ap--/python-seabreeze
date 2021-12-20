/***************************************************//**
 * @file    OBPReadSpectrum32AndMetadataExchange.cpp
 * @date    September 2013
 * @author  Ocean Optics, Inc.
 *
 * This message type is intended for the QE-PRO which
 * aligns pixel data to 32-bit words and includes a
 * metadata block at the start of each spectrum.
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

#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadSpectrum32AndMetadataExchange.h"
#include "common/U32Vector.h"
#include "common/ByteVector.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPReadSpectrum32AndMetadataExchange::OBPReadSpectrum32AndMetadataExchange(unsigned int pixels)
    : OBPReadRawSpectrum32AndMetadataExchange(pixels) {
}


OBPReadSpectrum32AndMetadataExchange::~OBPReadSpectrum32AndMetadataExchange() {
    /* Nothing to do here */
}

Data *OBPReadSpectrum32AndMetadataExchange::transfer(TransferHelper *helper) {
    Data *xfer;
    unsigned char lswlsb;
    unsigned char lswmsb;
    unsigned char mswlsb;
    unsigned char mswmsb;

    /* This will use the superclass to transfer data from the device, and will
     * then strip off the message header and footer so that only the
     * pixel data remains.
     */
    xfer = OBPReadRawSpectrum32AndMetadataExchange::transfer(helper);
    if(NULL == xfer) {
        string error("Expected Transfer::transfer to produce a non-null result "
                        "containing raw spectral data.  Without this data, it is not "
                        "possible to generate a valid formatted spectrum.");
        throw ProtocolException(error);
    }
    /* xfer should contain a ByteVector */

    /* Extract the pixel data from the byte vector */
    ByteVector *bv = static_cast<ByteVector *>(xfer);
    vector<unsigned char> bytes = bv->getByteVector();

    vector<unsigned int> formatted(this->numberOfPixels);
    for(unsigned int i = 0; i < this->numberOfPixels; i++) {
        lswlsb = bytes[i * 4 + this->metadataLength];
        lswmsb = bytes[(i * 4) + 1 + this->metadataLength];
        mswlsb = bytes[(i * 4) + 2 + this->metadataLength];
        mswmsb = bytes[(i * 4) + 3 + this->metadataLength];
        formatted[i] =    ((mswmsb & 0x00FF) << 24)
                        | ((mswlsb & 0x00FF) << 16)
                        | ((lswmsb & 0x00FF) << 8)
                        |  (lswlsb & 0x00FF);
    }
    delete xfer;  /* Equivalent to deleting bv and bytes */

    U32Vector *retval = new U32Vector(formatted);

    return retval;
}
