/***************************************************//**
 * @file    IPv4FeatureAdapter.cpp
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze IPv4FeatureInterface instances.
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
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/seabreezeapi/IPv4FeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

IPv4FeatureAdapter::IPv4FeatureAdapter(
        IPv4FeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<IPv4FeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

IPv4FeatureAdapter::~IPv4FeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char IPv4FeatureAdapter::get_IPv4_DHCP_Enable_State(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableStatus = 0;
    try {
		enableStatus = this->feature->get_IPv4_DHCP_Enable_State(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return enableStatus;
}

void IPv4FeatureAdapter::set_IPv4_DHCP_Enable_State(int *errorCode, unsigned char interfaceIndex, unsigned char enableStatus)
{
    try
	{
        this->feature->set_IPv4_DHCP_Enable_State(*this->protocol, *this->bus, interfaceIndex, enableStatus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned char IPv4FeatureAdapter::get_Number_Of_IPv4_Addresses(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char numberOfAddresses = 0;
	try {
		numberOfAddresses = this->feature->get_Number_Of_IPv4_Addresses(*this->protocol, *this->bus, interfaceIndex);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return numberOfAddresses;
}

void IPv4FeatureAdapter::get_IPv4_Address(int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char (*IPv4_Address)[4], unsigned char *netMask)
{

    vector<unsigned char> ipv4AddressVector;

    try
	{
		this->feature->get_IPv4_Address(*this->protocol, *this->bus, interfaceIndex, addressIndex, &ipv4AddressVector, netMask);

        memcpy((*IPv4_Address), &(ipv4AddressVector[0]), 4);

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

void IPv4FeatureAdapter::get_IPv4_Default_Gateway(int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4])
{

	vector<unsigned char> defaultGatewayAddressVector;

	try
	{
		defaultGatewayAddressVector = this->feature->get_IPv4_Default_Gateway(*this->protocol, *this->bus, interfaceIndex);

		memcpy(defaultGatewayAddress, &(defaultGatewayAddressVector[0]), 4);

		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}

void IPv4FeatureAdapter::set_IPv4_Default_Gateway(int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4])
{

    vector<unsigned char> *defaultGatewayAddressVector = new vector<unsigned char>(4);
    memcpy(&((*defaultGatewayAddressVector)[0]), defaultGatewayAddress, 4);

    try {
        this->feature->set_IPv4_Default_Gateway(*this->protocol, *this->bus, interfaceIndex, *defaultGatewayAddressVector);
        delete defaultGatewayAddressVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete defaultGatewayAddressVector;

    }
}


void IPv4FeatureAdapter::add_IPv4_Address(int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
{

	vector<unsigned char> *ipv4AddressVector = new vector<unsigned char>(4);
	memcpy(&((*ipv4AddressVector)[0]), IPv4_Address, 4);

	try {
		this->feature->add_IPv4_Address(*this->protocol, *this->bus, interfaceIndex, *ipv4AddressVector, netMask);
		delete ipv4AddressVector;
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		delete ipv4AddressVector;

	}
}

void IPv4FeatureAdapter::delete_IPv4_Address(int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex)
{
	try
	{
		this->feature->delete_IPv4_Address(*this->protocol, *this->bus, interfaceIndex, addressIndex);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}
