/***************************************************//**
 * @file    MulticastFeatureAdapter.cpp
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze MulticastFeatureInterface instances.
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
#include "api/seabreezeapi/MulticastFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

MulticastFeatureAdapter::MulticastFeatureAdapter(
        MulticastFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<MulticastFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

MulticastFeatureAdapter::~MulticastFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

#if 0
void MulticastFeatureAdapter::getGroupAddress(int *errorCode, unsigned char interfaceIndex, unsigned char (&groupAddress)[4])
{

    vector<unsigned char> groupAddressVector;

    try
	{
        groupAddressVector = this->feature->getGroupAddress(*this->protocol, *this->bus, interfaceIndex);

        memcpy(groupAddress, &(groupAddressVector[0]), 4);

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

void MulticastFeatureAdapter::setGroupAddress(int *errorCode, unsigned char interfaceIndex, const unsigned char groupAddress[4])
{

    vector<unsigned char> *groupAddressVector = new vector<unsigned char>(4);
    memcpy(&((*groupAddressVector)[0]), groupAddress, 4);

    try {
        this->feature->set_MAC_Address(*this->protocol, *this->bus, interfaceIndex, *groupAddressVector);
        delete groupAddressVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete groupAddressVector;

    }
}
#endif

unsigned char MulticastFeatureAdapter::getEnableState(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;
    try {
		enableState = this->feature->getEnableState(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return enableState;
}

void MulticastFeatureAdapter::setEnableState(int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
    try
	{
        this->feature->setEnableState(*this->protocol, *this->bus, interfaceIndex, enableState);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}
