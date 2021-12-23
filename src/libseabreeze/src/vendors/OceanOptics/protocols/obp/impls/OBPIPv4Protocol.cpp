/***************************************************//**
 * @file    OBPIPv4Protocol.cpp
 * @date    March 2017
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

#include "vendors/OceanOptics/protocols/obp/impls/OBPIPv4Protocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPAddIPv4AddressExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPDeleteIPv4AddressExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetIPv4AddressExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetIPv4DefaultGatewayExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetIPv4DHCPEnableStateExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetIPv4NumberOfAddressesExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetIPv4DefaultGatewayExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetIPv4DHCPEnableStateExchange.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPIPv4Protocol::OBPIPv4Protocol()
        : IPv4ProtocolInterface(new OceanBinaryProtocol())
{

}


OBPIPv4Protocol::~OBPIPv4Protocol()
{

}



#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif


void OBPIPv4Protocol::get_IPv4_Address(const Bus &bus, unsigned char interfaceIndex, unsigned char addressIndex,  vector<unsigned char> *IPv4_Address,  unsigned char *netMask)
{
    TransferHelper *helper;
    OBPGetIPv4AddressExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	request.setInterfaceIndex(interfaceIndex);
	request.setAddressIndex(addressIndex);

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result, without data, it is not possible to continue");
        throw ProtocolException(error);
    }

    vector<unsigned char> result = *raw;
    // c++11 not used yet
	//IPv4_Address.assign(raw->cbegin(), prev(raw->cend()));
	IPv4_Address->assign(raw->begin(), raw->end()-1);
	*netMask = raw->back();

    delete raw;
}

vector<unsigned char> OBPIPv4Protocol::get_IPv4_Default_Gateway(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetIPv4DefaultGatewayExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	request.setInterfaceIndex(interfaceIndex);

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result, without  data, it is not possible to continue.");
        throw ProtocolException(error);
    }

    vector<unsigned char> result = *raw;

    delete raw;

    return result;
}


void OBPIPv4Protocol::set_IPv4_Default_Gateway(const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> defaultGatewayAddress)
{
    TransferHelper *helper;
	OBPSetIPv4DefaultGatewayExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* factors is probably a reference to what was passed in by the caller,
     * so make a copy and truncate it to the maximum size.
     */

	command.setInterfaceIndex(interfaceIndex);
	command.setDefaultGatewayAddress(defaultGatewayAddress);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

}


unsigned char OBPIPv4Protocol::get_IPv4_DHCP_Enable_State(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
	OBPGetIPv4DHCPEnableStateExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	request.setInterfaceIndex(interfaceIndex);

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result, without this, it is not possible to continue");
        throw ProtocolException(error);
    }

    if(raw->size() < sizeof(unsigned char)) {
        string error("Failed to get back expected number of bytes that should have held the data.");
        delete raw;
        throw ProtocolException(error);
    }

	int retval = (*raw)[0];

    delete raw;

    return retval;
}


void OBPIPv4Protocol::set_IPv4_DHCP_Enable_State(const Bus &bus, unsigned char interfaceIndex, unsigned char enableStatus)
{
    TransferHelper *helper;
	OBPSetIPv4DHCPEnableStateExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	command.setInterfaceIndex(interfaceIndex);
	command.setEnable(enableStatus);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
}

unsigned char OBPIPv4Protocol::get_Number_Of_IPv4_Addresses(const Bus &bus, unsigned char interfaceIndex)
{
	TransferHelper *helper;
	OBPGetIPv4NumberOfAddressesExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	request.setInterfaceIndex(interfaceIndex);

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result, without this, it is not possible to continue");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should have held the data.");
		delete raw;
		throw ProtocolException(error);
	}

	int retval = (*raw)[0];

	delete raw;

	return retval;
}

void OBPIPv4Protocol::add_IPv4_Address(const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> IPv4_Address, unsigned char netMask)
{
	TransferHelper *helper;
	OBPAddIPv4AddressExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* factors is probably a reference to what was passed in by the caller,
	* so make a copy and truncate it to the maximum size.
	*/

	command.setInterfaceIndex(interfaceIndex);
	command.setAddress(IPv4_Address);
	command.setNetMask(netMask);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

}

void OBPIPv4Protocol::delete_IPv4_Address(const Bus &bus, unsigned char interfaceIndex, unsigned char IPv4_Address_Index)
{
	TransferHelper *helper;
	OBPDeleteIPv4AddressExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* factors is probably a reference to what was passed in by the caller,
	* so make a copy and truncate it to the maximum size.
	*/

	command.setInterfaceIndex(interfaceIndex);
	command.setAddressIndex(IPv4_Address_Index);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

}
