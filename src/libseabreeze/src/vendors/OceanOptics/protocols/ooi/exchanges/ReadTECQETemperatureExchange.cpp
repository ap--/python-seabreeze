/***************************************************//**
 * @file    ReadTECQETemperatureExchange.cpp
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
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadTECQETemperatureExchange.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"
#include "vendors/OceanOptics/protocols/ooi/constants/QETECConstants.h"
#include "common/DoubleVector.h"
#include "common/ByteVector.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

/* TEC cooler understands increments of 0.1 degrees C per encoder count. */

ReadTECQETemperatureExchange::ReadTECQETemperatureExchange() {

    vector<ProtocolHint *> *requestHints = new vector<ProtocolHint *>;
    requestHints->push_back(new ControlHint());

    vector<byte> *requestBuffer = new vector<byte>;
    requestBuffer->resize(1);
    (*(requestBuffer))[0] = OpCodes::OP_READTEC_QE;

    Transfer *request = new Transfer(requestHints, requestBuffer, Transfer::TO_DEVICE, 1);

    vector<ProtocolHint *> *responseHints = new vector<ProtocolHint *>;
    responseHints->push_back(new ControlHint());

    vector<byte> *responseBuffer = new vector<byte>;
    responseBuffer->resize(2);

    Transfer *response = new Transfer(responseHints, responseBuffer, Transfer::FROM_DEVICE, 2);

    this->addTransfer(request);
    this->addTransfer(response);
}

ReadTECQETemperatureExchange::~ReadTECQETemperatureExchange() {

}

Data *ReadTECQETemperatureExchange::transfer(TransferHelper *helper)
throw (ProtocolException) {
    Data *xfer;
    DoubleVector *retval;
    double temp;
    short counts;

    /* Delegate to the superclass to move the buffers. */
    /* This transfer() may cause a ProtocolException to be thrown. */
    xfer = Transaction::transfer(helper);
    if(NULL == xfer) {
        string error("Expected Transfer::transfer to produce a non-null result "
                "containing raw temperature data.  Without this data, it is not possible to "
                "generate a valid temperature reading.");
        throw ProtocolException(error);
    }

    /* FIXME: this is known to be safe for now, but should really do an explicit
     * check for type safety.
     */
    ByteVector *resp = (ByteVector *)xfer;
    vector<byte> raw = resp->getByteVector();

    /* Need to put this into a short to preserve the sign bit */
    counts = ((raw[1] << 8) & 0xFF00) | (raw[0] & 0x00FF);
    temp = counts / (double)QE_TEC_COUNTS_PER_DEGREE_C;

    vector<double> dv;
    dv.resize(1);
    dv[0] = temp;

    delete resp;

    retval = new DoubleVector(dv);
    return retval;
}
