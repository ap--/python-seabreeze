/***************************************************//**
 * @file    OBPWifiConfigurationProtocol.cpp
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

#include "vendors/OceanOptics/protocols/obp/impls/OBPWifiConfigurationProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetWifiConfigurationSSIDExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetWifiConfigurationSSIDExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetWifiConfigurationPassPhraseExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetWifiConfigurationModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetWifiConfigurationModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetWifiConfigurationSecurityExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetWifiConfigurationSecurityExchange.h"

#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPWifiConfigurationProtocol::OBPWifiConfigurationProtocol()
        : WifiConfigurationProtocolInterface(new OceanBinaryProtocol())
{

}


OBPWifiConfigurationProtocol::~OBPWifiConfigurationProtocol()
{

}


#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

vector<unsigned char> OBPWifiConfigurationProtocol::getSSID(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetWifiConfigurationSSIDExchange request;

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

    vector<unsigned char> result = *raw;

    delete raw;

    return result;
}


void OBPWifiConfigurationProtocol::setSSID(const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> ssid)
{
    TransferHelper *helper;
	OBPSetWifiConfigurationSSIDExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* factors is probably a reference to what was passed in by the caller,
     * so make a copy and truncate it to the maximum size.
     */

	command.setInterfaceIndex(interfaceIndex);
    command.setSSID(ssid);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

}

void OBPWifiConfigurationProtocol::setPassPhrase(const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> passPhrase)
{
	TransferHelper *helper;
	OBPSetWifiConfigurationPassPhraseExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* factors is probably a reference to what was passed in by the caller,
	* so make a copy and truncate it to the maximum size.
	*/

	command.setInterfaceIndex(interfaceIndex);
	command.setPassPhrase(passPhrase);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

}


unsigned char OBPWifiConfigurationProtocol::getMode(const Bus &bus, unsigned char interfaceIndex)
{
    TransferHelper *helper;
    OBPGetWifiConfigurationModeExchange request;

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
        string error("Failed to get back expected number of bytes that should"
            " have held collection area.");
        delete raw;
        throw ProtocolException(error);
    }

	int retval = (*raw)[0];

    delete raw;

    return retval;
}


void OBPWifiConfigurationProtocol::setMode(const Bus &bus, unsigned char interfaceIndex, unsigned char mode)
{
    TransferHelper *helper;
    OBPSetWifiConfigurationModeExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	command.setInterfaceIndex(interfaceIndex);
	command.setMode(mode);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
}


unsigned char OBPWifiConfigurationProtocol::getSecurityType(const Bus &bus, unsigned char interfaceIndex)
{
	TransferHelper *helper;
	OBPGetWifiConfigurationSecurityExchange request;

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

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	int retval = (*raw)[0];

	delete raw;

	return retval;
}


void OBPWifiConfigurationProtocol::setSecurityType(const Bus &bus, unsigned char interfaceIndex, unsigned char securityType)
{
	TransferHelper *helper;
	OBPSetWifiConfigurationSecurityExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}
	command.setInterfaceIndex(interfaceIndex);
	command.setSecurityType(securityType);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

	/* FIXME: this could check the return value and react if it did not succeed */
}
