/***************************************************//**
 * @file    DHCPServerFeatureAdapter.cpp
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze DHCPServerFeatureInterface instances.
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
#include "api/seabreezeapi/DHCPServerFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

DHCPServerFeatureAdapter::DHCPServerFeatureAdapter(
        DHCPServerFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<DHCPServerFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

DHCPServerFeatureAdapter::~DHCPServerFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

void DHCPServerFeatureAdapter::getServerAddress(int *errorCode, unsigned char interfaceIndex, unsigned char (*serverAddress)[4], unsigned char *netMask)
{

    vector<unsigned char> serverAddressVector;

    try
	{
		this->feature->getServerAddress(*this->protocol, *this->bus, interfaceIndex, &serverAddressVector, netMask);

        memcpy(serverAddress, &(serverAddressVector[0]), 4);

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (const FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

void DHCPServerFeatureAdapter::setServerAddress(int *errorCode, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask)
{

    vector<unsigned char> *serverAddressVector = new vector<unsigned char>(4);
    memcpy(&((*serverAddressVector)[0]), serverAddress, 4);

    try {
        this->feature->setServerAddress(*this->protocol, *this->bus, interfaceIndex, *serverAddressVector, netMask);
        delete serverAddressVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete serverAddressVector;

    }
}


unsigned char DHCPServerFeatureAdapter::getServerEnableState(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;
    try {
		enableState = this->feature->getServerEnableState(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return enableState;
}

void DHCPServerFeatureAdapter::setServerEnableState(int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
    try
	{
        this->feature->setServerEnableState(*this->protocol, *this->bus, interfaceIndex, enableState);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (const FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}
