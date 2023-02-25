/***************************************************//**
 * @file    WifiConfigurationFeatureAdapter.cpp
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze WifiConfigurationFeatureInterface instances.
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
#include "api/seabreezeapi/WifiConfigurationFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

WifiConfigurationFeatureAdapter::WifiConfigurationFeatureAdapter(
        WifiConfigurationFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<WifiConfigurationFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

WifiConfigurationFeatureAdapter::~WifiConfigurationFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char WifiConfigurationFeatureAdapter::getMode(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char mode = 0;
    try {
		mode = this->feature->getMode(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return mode;
}

void WifiConfigurationFeatureAdapter::setMode(int *errorCode, unsigned char interfaceIndex, unsigned char mode)
{
    try
	{
        this->feature->setMode(*this->protocol, *this->bus, interfaceIndex, mode);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned char WifiConfigurationFeatureAdapter::getSecurityType(int *errorCode, unsigned char interfaceIndex)
{
	unsigned char securityType = 0;
    try {
		securityType = this->feature->getSecurityType(*this->protocol, *this->bus, interfaceIndex);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return securityType;
}

void WifiConfigurationFeatureAdapter::setSecurityType(int *errorCode, unsigned char interfaceIndex, unsigned char securityType)
{
    try
	{
        this->feature->setSecurityType(*this->protocol, *this->bus, interfaceIndex, securityType);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned char WifiConfigurationFeatureAdapter::getSSID(int *errorCode, unsigned char interfaceIndex, unsigned char (*ssid)[32])
{

    vector<unsigned char> ssidVector;

    try
	{

		ssidVector = this->feature->getSSID(*this->protocol, *this->bus, interfaceIndex);
		// remove the null termination
		//ssidVector.resize(std::find(ssidVector.begin(), ssidVector.end(), 0) - ssidVector.begin());

        // can't use c++11 yet
        unsigned char index;
        for(index = 0; index<ssidVector.size(); index++)
        {
        	if(ssidVector[index] == 0)
        	{
        	    break;
        	}
        }
        ssidVector.resize(index);

        memcpy(ssid, &ssidVector[0], ssidVector.size());

        SET_ERROR_CODE(ERROR_SUCCESS);
		return ssidVector.size() & 0xFF;

    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		return 0;
    }
}

void WifiConfigurationFeatureAdapter::setSSID(int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
{

    vector<unsigned char> *ssidVector = new vector<unsigned char>(length);
    memcpy(&((*ssidVector)[0]), ssid, length);

    try {
        this->feature->setSSID(*this->protocol, *this->bus, interfaceIndex, *ssidVector);
        delete ssidVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete ssidVector;

    }
}

void WifiConfigurationFeatureAdapter::setPassPhrase(int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength)
{

    vector<unsigned char> *passPhraseVector = new vector<unsigned char>(passPhraseLength);
    memcpy(&((*passPhraseVector)[0]), passPhrase, passPhraseLength);

    try {
        this->feature->setPassPhrase(*this->protocol, *this->bus, interfaceIndex, *passPhraseVector);
        delete passPhraseVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete passPhraseVector;

    }
}
