/***************************************************//**
 * @file    NetworkConfigurationFeatureAdapter.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze NetworkConfigurationFeatureInterface instances.
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
#include "api/seabreezeapi/NetworkConfigurationFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

NetworkConfigurationFeatureAdapter::NetworkConfigurationFeatureAdapter(
	NetworkConfigurationFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
	: FeatureAdapterTemplate<NetworkConfigurationFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

NetworkConfigurationFeatureAdapter::~NetworkConfigurationFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char NetworkConfigurationFeatureAdapter::getNumberOfNetworkInterfaces(int *errorCode)
{
	unsigned char enableStatus = 0;
	try {
		enableStatus = this->feature->getNumberOfNetworkInterfaces(*this->protocol, *this->bus);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return enableStatus;
}

unsigned char NetworkConfigurationFeatureAdapter::getNetworkInterfaceConnectionType(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char interfaceType = 0;
	try {
		interfaceType = this->feature->getNetworkInterfaceConnectionType(*this->protocol, *this->bus, interfaceIndex);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return interfaceType;
}

unsigned char NetworkConfigurationFeatureAdapter::getNetworkInterfaceEnableState(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableStatus = 0;
    try {
		enableStatus = this->feature->getNetworkInterfaceEnableState(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return enableStatus;
}

void NetworkConfigurationFeatureAdapter::setNetworkInterfaceEnableState(int *errorCode, unsigned char interfaceIndex, unsigned char enableStatus)
{
    try
	{
        this->feature->setNetworkInterfaceEnableState(*this->protocol, *this->bus, interfaceIndex, enableStatus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned char NetworkConfigurationFeatureAdapter::runNetworkInterfaceSelfTest(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char result = 0;
	try {
		result = this->feature->runNetworkInterfaceSelfTest(*this->protocol, *this->bus, interfaceIndex);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return result;
}

void NetworkConfigurationFeatureAdapter::saveNetworkInterfaceConnectionSettings(int *errorCode, unsigned char interfaceIndex)
{
	try
	{
		this->feature->saveNetworkInterfaceConnectionSettings(*this->protocol, *this->bus, interfaceIndex);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}
