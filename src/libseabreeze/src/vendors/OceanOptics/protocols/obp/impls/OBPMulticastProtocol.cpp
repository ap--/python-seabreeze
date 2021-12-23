/***************************************************//**
 * @file    OBPMulticastProtocol.cpp
 * @date    February 2017
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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

#include "vendors/OceanOptics/protocols/obp/impls/OBPMulticastProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetMulticastEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetMulticastEnableExchange.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPMulticastProtocol::OBPMulticastProtocol()
        : MulticastProtocolInterface(new OceanBinaryProtocol())
{

}


OBPMulticastProtocol::~OBPMulticastProtocol()
{

}

#if 0
vector<unsigned char> OBPMulticastProtocol::getGroupAddress(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetMulticastGroupAddressExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	request.setInterfaceIndex(interfaceIndex);

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result "
            "containing calibration data.  Without this data, it is not possible to "
            "generate a calibration array.");
        throw ProtocolException(error);
    }

    vector<unsigned char> result = *raw;

    delete raw;

    return result;
}


void OBPMulticastProtocol::setGroupAddress(const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> macAddress)
{
    TransferHelper *helper;
	OBPSetMulticastGroupAddressExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* factors is probably a reference to what was passed in by the caller,
     * so make a copy and truncate it to the maximum size.
     */

	command.setInterfaceIndex(interfaceIndex);
    command.set_MAC_Address(macAddress);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

}
#endif

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif


unsigned char OBPMulticastProtocol::getEnableState(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetMulticastEnableExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	request.setInterfaceIndex(interfaceIndex);

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result "
            "containing calibration data.  Without this data, it is not possible to continue.");
        throw ProtocolException(error);
    }

    if(raw->size() < sizeof(unsigned char)) {
        string error("Failed to get back expected number of bytes that should have held the enable state");
        delete raw;
        throw ProtocolException(error);
    }

	int retval = (*raw)[0];

    delete raw;

    return retval;
}


void OBPMulticastProtocol::setEnableState(const Bus &bus, unsigned char interfaceIndex, unsigned char enableState)
{
    TransferHelper *helper;
    OBPSetMulticastEnableExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	command.setInterfaceIndex(interfaceIndex);
	command.setEnableState(enableState);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
}
