/***************************************************//**
 * @file    IPv4Feature.cpp
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
#include "vendors/OceanOptics/features/ipv4/IPv4Feature.h"
#include "vendors/OceanOptics/protocols/interfaces/IPv4ProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

IPv4Feature::IPv4Feature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

IPv4Feature::~IPv4Feature()
{

}

unsigned char IPv4Feature::get_IPv4_DHCP_Enable_State(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;
	unsigned char enableStatus;

	try {
		proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		enableStatus = IPv4PI->get_IPv4_DHCP_Enable_State(bus, interfaceIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return enableStatus;
}

void IPv4Feature::set_IPv4_DHCP_Enable_State(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char enableStatus)
{
	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		IPv4PI->set_IPv4_DHCP_Enable_State(bus, interfaceIndex, enableStatus);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

}

unsigned char IPv4Feature::get_Number_Of_IPv4_Addresses(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;
	unsigned char numberOfAddresses;

	try {
		proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		numberOfAddresses = IPv4PI->get_Number_Of_IPv4_Addresses(bus, interfaceIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return numberOfAddresses;
}

void IPv4Feature::get_IPv4_Address(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char addressIndex,  vector<unsigned char> *IPv4_Address,  unsigned char *netMask)
{

    vector<unsigned char> data;
	IPv4ProtocolInterface *IPv4PI = NULL;
    ProtocolHelper *proto;

    try
    {
        proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
    }
	catch (FeatureProtocolNotFoundException &e)
	{
        string error(
        "Could not find matching protocol implementation to data.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try
	{
        IPv4PI->get_IPv4_Address(bus, interfaceIndex, addressIndex, IPv4_Address, netMask);
    }
	catch (ProtocolException &pe)
	{
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

vector<unsigned char> IPv4Feature::get_IPv4_Default_Gateway(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{

	vector<unsigned char> data;
	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;

	try
	{
		proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e)
	{
		string error(
			"Could not find matching protocol implementation to data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try
	{
		data = IPv4PI->get_IPv4_Default_Gateway(bus, interfaceIndex);
	}
	catch (ProtocolException &pe)
	{
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return data;
}

void IPv4Feature::set_IPv4_Default_Gateway(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> gatewayAddress)
{

	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get data.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		IPv4PI->set_IPv4_Default_Gateway(bus, interfaceIndex, gatewayAddress);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

void IPv4Feature::add_IPv4_Address(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> IPv4_Address, unsigned char netMask)
{

	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		IPv4PI->add_IPv4_Address(bus, interfaceIndex, IPv4_Address, netMask);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}
void IPv4Feature::delete_IPv4_Address(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char addressIndex)
{

	IPv4ProtocolInterface *IPv4PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		IPv4PI = static_cast<IPv4ProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		IPv4PI->delete_IPv4_Address(bus, interfaceIndex, addressIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}

FeatureFamily IPv4Feature::getFeatureFamily() {
    FeatureFamilies families;

	return families.IPV4_ADDRESS;
}
