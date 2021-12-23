/***************************************************//**
 * @file    OBPEthernetConfigurationProtocol.cpp
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

#include "vendors/OceanOptics/protocols/obp/impls/OBPEthernetConfigurationProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetEthernetConfigurationMACAddressExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetEthernetConfigurationMACAddressExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetEthernetConfigurationGbEEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetEthernetConfigurationGbEEnableExchange.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPEthernetConfigurationProtocol::OBPEthernetConfigurationProtocol()
        : EthernetConfigurationProtocolInterface(new OceanBinaryProtocol())
{

}


OBPEthernetConfigurationProtocol::~OBPEthernetConfigurationProtocol()
{

}

vector<unsigned char> OBPEthernetConfigurationProtocol::get_MAC_Address(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetEthernetConfigurationMACAddressExchange request;

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


void OBPEthernetConfigurationProtocol::set_MAC_Address(const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> macAddress)
{
    TransferHelper *helper;
	OBPSetEthernetConfigurationMacAddressExchange command;

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

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif


unsigned char OBPEthernetConfigurationProtocol::get_GbE_Enable_Status(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetEthernetConfigurationGbEEnableExchange request;

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

    if(raw->size() < sizeof(unsigned char)) {
        string error("Failed to get back expected number of bytes that should"
            " have held collection area.");
        delete raw;
        throw ProtocolException(error);
    }

	int retval = (*raw)[0];

    delete raw;

    return retval;
}


void OBPEthernetConfigurationProtocol::set_GbE_Enable_Status(const Bus &bus, unsigned char interfaceIndex, unsigned char enableStatus)
{
    TransferHelper *helper;
    OBPSetEthernetConfigurationGbEEnableExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	command.setInterfaceIndex(interfaceIndex);
	command.setGbE_Enable(enableStatus);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
}
