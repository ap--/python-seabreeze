/***************************************************//**
 * @file    OOIIrradCalProtocol.cpp
 * @date    March 2010
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

#include <math.h>
#include <string>

#include "vendors/OceanOptics/protocols/ooi/impls/OOIIrradCalProtocol.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/OOIReadIrradCalExchange.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOIProtocol.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

OOIIrradCalProtocol::OOIIrradCalProtocol(unsigned int pixelCount)
        : IrradCalProtocolInterface(new OOIProtocol()) {
    this->numberOfPixels = pixelCount;
}


OOIIrradCalProtocol::~OOIIrradCalProtocol() {

}

vector<float> *OOIIrradCalProtocol::readIrradCal(const Bus &bus) {
    TransferHelper *helper;
    OOIReadIrradCalExchange readCalExchange(this->numberOfPixels);

    helper = bus.getHelper(readCalExchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    Data *result = readCalExchange.transfer(helper);
    if (NULL == result) {
        string error("Expected Transfer::transfer to produce a non-null result "
            "containing calibration data.  Without this data, it is not possible to "
            "generate a calibration array.");
        throw ProtocolException(error);
    }

    /* FIXME: this cast is known to be safe for now, but this needs
     * to do some sort of check to make sure the cast is valid.
     */
    ByteVector *bv = static_cast<ByteVector *>(result);
    vector<unsigned char> raw = bv->getByteVector();
    vector<float> *retval = new vector<float>;

    for(unsigned int i = 0; i < raw.size(); i += 4) {
        float value;
        unsigned int *fptr = (unsigned int *)&value;
        /* The convention here is MSB first */
        *fptr =   ((raw[i] & 0x00FF) << 24)
                | ((raw[i + 1] & 0x00FF) << 16)
                | ((raw[i + 2] & 0x00FF) << 8)
                | ((raw[i + 3] & 0x00FF));
        retval->push_back(value);
    }
    delete result;

    return retval;
}


int OOIIrradCalProtocol::writeIrradCal(const Bus &bus, const vector<float> &cal) {
    TransferHelper *helper;
    int writeLength = 0;
    OOIWriteIrradCalExchange writeCalExchange(this->numberOfPixels);

    helper = bus.getHelper(writeCalExchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* Push the calibration to the exchange */
    writeLength = writeCalExchange.setCalibration(cal);

    /* This transfer() may cause a ProtocolException to be thrown. */
    writeCalExchange.transfer(helper);

    return writeLength;
}

int OOIIrradCalProtocol::hasCollectionArea(const Bus &bus) {
    /* This protocol does not provide a mechanism to store a collection area.
     * If the caller checks this before attempting to read or write a
     * calibration then they can avoid having an exception thrown.
     */
    return 0;
}

float OOIIrradCalProtocol::readCollectionArea(const Bus &bus) {
    string error("OOI protocol does not support reading collection area.");
    throw ProtocolException(error);
}

void OOIIrradCalProtocol::writeCollectionArea(const Bus &bus, float area) {
    string error("OOI protocol does not support writing collection area.");
    throw ProtocolException(error);
}
