/***************************************************//**
 * @file    EthernetConfigurationFeatureAdapter.cpp
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze EthernetConfigurationFeatureInterface instances.
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
#include "api/seabreezeapi/EthernetConfigurationFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

EthernetConfigurationFeatureAdapter::EthernetConfigurationFeatureAdapter(
        EthernetConfigurationFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<EthernetConfigurationFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

EthernetConfigurationFeatureAdapter::~EthernetConfigurationFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

void EthernetConfigurationFeatureAdapter::get_MAC_Address(int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6])
{

    vector<unsigned char> macAddressVector;

    try
	{
        macAddressVector = this->feature->get_MAC_Address(*this->protocol, *this->bus, interfaceIndex);

        memcpy(macAddress, &(macAddressVector[0]), 6);

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

void EthernetConfigurationFeatureAdapter::set_MAC_Address(int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6])
{

    vector<unsigned char> *macAddressVector = new vector<unsigned char>(6);
    memcpy(&((*macAddressVector)[0]), macAddress, 6);

    try {
        this->feature->set_MAC_Address(*this->protocol, *this->bus, interfaceIndex, *macAddressVector);
        delete macAddressVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete macAddressVector;

    }
}


unsigned char EthernetConfigurationFeatureAdapter::get_GbE_Enable_Status(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableStatus = 0;
    try {
		enableStatus = this->feature->get_GbE_Enable_Status(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return enableStatus;
}

void EthernetConfigurationFeatureAdapter::set_GbE_Enable_Status(int *errorCode, unsigned char interfaceIndex, unsigned char enableStatus)
{
    try
	{
        this->feature->set_GbE_Enable_Status(*this->protocol, *this->bus, interfaceIndex, enableStatus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}
