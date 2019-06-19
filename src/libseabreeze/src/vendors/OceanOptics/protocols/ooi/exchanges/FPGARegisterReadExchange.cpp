/***************************************************//**
 * @file    FPGARegisterReadExchange.cpp
 * @date    October 2012
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
#include "vendors/OceanOptics/protocols/ooi/exchanges/FPGARegisterReadExchange.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using std::vector;

FPGARegisterReadExchange::FPGARegisterReadExchange(byte address) {

    vector<ProtocolHint *> *requestHints  = new vector<ProtocolHint *>;
    vector<ProtocolHint *> *responseHints = new vector<ProtocolHint *>;

    requestHints ->push_back(new ControlHint());
    responseHints->push_back(new ControlHint());

    vector<byte> *requestBuffer  = new vector<byte>(2);
    vector<byte> *responseBuffer = new vector<byte>(3);

    // TODO: future systems such as the QE18 might have 32-bit registers,
    //       requiring a larger response buffer

    requestBuffer->at(0) = OpCodes::OP_READ_REGISTER;
    requestBuffer->at(1) = address;

    Transfer *request = new Transfer(requestHints, requestBuffer, Transfer::TO_DEVICE,
        (unsigned int) requestBuffer->size());
    Transfer *response = new Transfer(responseHints, responseBuffer, Transfer::FROM_DEVICE,
        (unsigned int) responseBuffer->size());

    this->addTransfer(request);
    this->addTransfer(response);
}

FPGARegisterReadExchange::~FPGARegisterReadExchange() { }
