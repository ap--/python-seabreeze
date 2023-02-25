/***************************************************//**
 * @file    FPGARegisterProtocol.cpp
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
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"
#include "vendors/OceanOptics/protocols/ooi/impls/FPGARegisterProtocol.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/FPGARegisterReadExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/FPGARegisterWriteExchange.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOIProtocol.h"
#include <string>

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using std::string;
using std::vector;

FPGARegisterProtocol::FPGARegisterProtocol() : FPGARegisterProtocolInterface(new OOIProtocol()) {

}

FPGARegisterProtocol::~FPGARegisterProtocol() {

}

unsigned int FPGARegisterProtocol::readRegister(const Bus &bus, unsigned char address) {

    unsigned int retval = 0;

    FPGARegisterReadExchange exchange(address);

    TransferHelper *helper = bus.getHelper(exchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    Data* result = exchange.transfer(helper);
    if(NULL == result) {
        string error("Expected non-NULL result from FPGARegisterReadExchange");
        throw ProtocolException(error);
    }

    ByteVector *bv = dynamic_cast<ByteVector *>(result);
    if(NULL == bv) {
        throw ProtocolException(string("Expected ByteVector from FPGARegisterReadExchange"));
    }

    vector<unsigned char> byteVec = bv->getByteVector();
    if(3 != byteVec.size()) {
        throw ProtocolException(string("Expected 3 bytes from FPGARegisterReadExchange"));
    }

    // Response is 3 bytes (address echo, LSB, MSB)
    // TODO: this will need to be updated when we have devices with 32-bit registers
    retval = (unsigned int) (byteVec[1] | ((unsigned int) byteVec[2] << 8));

    delete result;

    return retval;
}

void FPGARegisterProtocol::writeRegister(const Bus &bus, unsigned char address,
        unsigned int value) {

    FPGARegisterWriteExchange exchange(address, value);

    TransferHelper *helper = bus.getHelper(exchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    Data *result = exchange.transfer(helper);
    if (NULL != result) {
        delete result;
    }
}
