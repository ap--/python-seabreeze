/***************************************************//**
 * @file    OBPAcquisitionDelayProtocol.cpp
 * @date    November 2015
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPAcquisitionDelayProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetAcquisitionDelayExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPAcquisitionDelayProtocol::OBPAcquisitionDelayProtocol()
    : AcquisitionDelayProtocolInterface(new OceanBinaryProtocol()) {

}

OBPAcquisitionDelayProtocol::~OBPAcquisitionDelayProtocol() {

}

void OBPAcquisitionDelayProtocol::setAcquisitionDelayMicroseconds(
            const Bus &bus, const unsigned long delayMicros) {

    TransferHelper *helper;
    OBPSetAcquisitionDelayExchange exchange;

    helper = bus.getHelper(exchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    exchange.setAcquisitionDelayMicros(delayMicros);
    bool retval = exchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected acquisition delay command.  Is the value legal?");
        throw ProtocolException(error);
    }

}
