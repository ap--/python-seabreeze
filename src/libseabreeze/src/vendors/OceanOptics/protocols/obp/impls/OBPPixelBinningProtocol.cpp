/***************************************************//**
 * @file    OBPPixelBinningProtocol.cpp
 * @date    October 2015
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPPixelBinningProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetPixelBinningExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetPixelBinningExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetDefaultPixelBinningExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetDefaultPixelBinningExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetMaxPixelBinningExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPPixelBinningProtocol::OBPPixelBinningProtocol()
        : PixelBinningProtocolInterface(new OceanBinaryProtocol()) {

}

OBPPixelBinningProtocol::~OBPPixelBinningProtocol() {

}

unsigned char OBPPixelBinningProtocol::readPixelBinningFactor(const Bus &bus) throw (ProtocolException) {


    OBPGetPixelBinningExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    vector<byte> *result = xchange.queryDevice(helper);
    if(NULL == result || result->size() == 0) {
        string error("Expected queryDevice to produce a non-empty result "
            "containing a pixel binning factor.  Without this data, it is not possible to "
            "continue.");
        throw ProtocolException(error);
    }

    unsigned char binningFactor = (*result)[0];

    delete result;

    return binningFactor;
}


void OBPPixelBinningProtocol::writePixelBinningFactor(const Bus &bus, const unsigned char binningFactor) throw (ProtocolException) {

    OBPSetPixelBinningExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setPixelBinningFactor(binningFactor);

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the enable command.  Is it supported on this hardware?");
        throw ProtocolException(error);
    }
}

unsigned char OBPPixelBinningProtocol::readDefaultPixelBinningFactor(const Bus &bus) throw (ProtocolException) {


    OBPGetDefaultPixelBinningExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    vector<byte> *result = xchange.queryDevice(helper);
    if(NULL == result || result->size() == 0) {
        string error("Expected queryDevice to produce a non-empty result "
            "containing a pixel binning factor.  Without this data, it is not possible to "
            "continue.");
        throw ProtocolException(error);
    }

    unsigned char binningFactor = (*result)[0];

    delete result;

    return binningFactor;
}

void OBPPixelBinningProtocol::writeDefaultPixelBinningFactor(const Bus &bus, const unsigned char binningFactor) throw (ProtocolException) {

    OBPSetDefaultPixelBinningExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setDefaultPixelBinningFactor(binningFactor);

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the enable command.  Is it supported on this hardware?");
        throw ProtocolException(error);
    }
}

void OBPPixelBinningProtocol::writeDefaultPixelBinningFactor(const Bus &bus) throw (ProtocolException) {

    OBPSetDefaultPixelBinningExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setDefaultPixelBinningFactor();

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the enable command.  Is it supported on this hardware?");
        throw ProtocolException(error);
    }
}

unsigned char OBPPixelBinningProtocol::readMaxPixelBinningFactor(const Bus &bus) throw (ProtocolException) {


    OBPGetMaxPixelBinningExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    vector<byte> *result = xchange.queryDevice(helper);
    if(NULL == result || result->size() == 0) {
        string error("Expected queryDevice to produce a non-empty result "
            "containing a pixel binning factor.  Without this data, it is not possible to "
            "continue.");
        throw ProtocolException(error);
    }

    unsigned char binningFactor = (*result)[0];

    delete result;

    return binningFactor;
}