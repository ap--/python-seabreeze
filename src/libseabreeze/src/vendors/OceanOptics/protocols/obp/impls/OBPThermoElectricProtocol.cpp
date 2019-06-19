/***************************************************//**
 * @file    OBPThermoElectricProtocol.cpp
 * @date    March 2013
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPThermoElectricProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetThermoElectricTemperatureExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetThermoElectricEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetThermoElectricSetpointExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPThermoElectricProtocol::OBPThermoElectricProtocol()
        : ThermoElectricProtocolInterface(new OceanBinaryProtocol()) {

}

OBPThermoElectricProtocol::~OBPThermoElectricProtocol() {

}

double OBPThermoElectricProtocol::readThermoElectricTemperatureCelsius(
            const Bus &bus) throw (ProtocolException) {

    vector<byte> *result;
    float temp = 0;
    unsigned int i;
    byte *cptr;

    OBPGetThermoElectricTemperatureExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    result = xchange.queryDevice(helper);
    if(NULL == result) {
        string error("Expected queryDevice to produce a non-null result "
            "containing a temperature.  Without this data, it is not possible to "
            "continue.");
        throw ProtocolException(error);
    }

    vector<byte>::iterator iter;
    cptr = (byte *)&temp;
    for(iter = result->begin(), i = 0;
                iter != result->end() && i < sizeof(float); iter++, i++) {
        /* FIXME: this assumes the host is little-endian.  Is this safe? */
        cptr[i] = *iter;
    }

    delete result;

    return temp;
}


void OBPThermoElectricProtocol::writeThermoElectricEnable(const Bus &bus,
            bool enable) throw (ProtocolException) {

    OBPSetThermoElectricEnableExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setThermoElectricEnable(enable);

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the enable command.  Is it supported on this hardware?");
        throw ProtocolException(error);
    }
}


void OBPThermoElectricProtocol::writeThermoElectricSetPointCelsius(
            const Bus &bus, double degreesC) throw (ProtocolException) {

    OBPSetThermoElectricSetpointExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setThermoElectricSetpointCelsius((float) degreesC);

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the setpoint.  Was it out of bounds?");
        throw ProtocolException(error);
    }
}
