/***************************************************//**
 * @file    OBPContinuousStrobeProtocol.cpp
 * @date    February 2013
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPContinuousStrobeProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPContinuousStrobeProtocol::OBPContinuousStrobeProtocol()
        : ContinuousStrobeProtocolInterface(new OceanBinaryProtocol()) {
    this->setPeriodExchange = new OBPContinuousStrobePeriodExchange();
    this->setEnableExchange = new OBPContinuousStrobeEnableExchange();
}

OBPContinuousStrobeProtocol::~OBPContinuousStrobeProtocol() {
    delete this->setPeriodExchange;
    delete this->setEnableExchange;
}

void OBPContinuousStrobeProtocol::setContinuousStrobeEnable(const Bus &bus,
        unsigned short strobe_id, bool enable) {

    TransferHelper *helper;

    if(0 != strobe_id) {
        string error("This protocol only supports one continuous strobe generator.");
        throw ProtocolException(error);
    }

    helper = bus.getHelper(this->setEnableExchange->getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    this->setEnableExchange->setContinuousStrobeEnable(enable);

    /* This may cause a ProtocolException to be thrown. */
    this->setEnableExchange->sendCommandToDevice(helper);

    /* FIXME: this could check the return value and throw an exception or retry if
     * it was false for any reason.
     */
}

void OBPContinuousStrobeProtocol::setContinuousStrobePeriodMicroseconds(const Bus &bus,
        unsigned short strobe_id, unsigned long period_usec) {

    TransferHelper *helper;

    if(0 != strobe_id) {
        string error("This protocol only supports one continuous strobe generator.");
        throw ProtocolException(error);
    }

    helper = bus.getHelper(this->setPeriodExchange->getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    this->setPeriodExchange->setContinuousStrobePeriodMicroseconds(period_usec);

    /* This may cause a ProtocolException to be thrown. */
    this->setPeriodExchange->sendCommandToDevice(helper);

    /* FIXME: this could check the return value and throw an exception or retry if
     * it was false for any reason.
     */
}
