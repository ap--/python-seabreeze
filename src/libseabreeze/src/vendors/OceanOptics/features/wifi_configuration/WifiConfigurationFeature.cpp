/***************************************************//**
 * @file    WifiConfigurationFeature.cpp
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
#include "vendors/OceanOptics/features/wifi_configuration/WifiConfigurationFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/WifiConfigurationProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

WifiConfigurationFeature::WifiConfigurationFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

WifiConfigurationFeature::~WifiConfigurationFeature()
{

}


unsigned char WifiConfigurationFeature::getMode(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
	ProtocolHelper *proto;
	unsigned char mode;

    try {
        proto = lookupProtocolImpl(protocol);
		wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to read collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        mode = wifiConfigurationPI->getMode(bus, interfaceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return mode;
}

void WifiConfigurationFeature::setMode(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char mode)
{
	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
	ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
		wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to write wifi mode.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		wifiConfigurationPI->setMode(bus, interfaceIndex, mode);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}

unsigned char WifiConfigurationFeature::getSecurityType(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
	ProtocolHelper *proto;
	unsigned char securityType;

    try {
        proto = lookupProtocolImpl(protocol);
		wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to read collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        securityType = wifiConfigurationPI->getSecurityType(bus, interfaceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return securityType;
}

void WifiConfigurationFeature::setSecurityType(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char securityType)
{
	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
	ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
		wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to write wifi mode.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		wifiConfigurationPI->setMode(bus, interfaceIndex, securityType);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}


vector<unsigned char> WifiConfigurationFeature::getSSID(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{

    vector<unsigned char> data;
	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
    ProtocolHelper *proto;

    try
    {
        proto = lookupProtocolImpl(protocol);
		wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    }
	catch (FeatureProtocolNotFoundException &e)
	{
        string error(
        "Could not find matching protocol implementation to get wifi configuration");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try
	{
        data = wifiConfigurationPI->getSSID(bus, interfaceIndex);
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

void WifiConfigurationFeature::setSSID(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> ssid)
{

	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		wifiConfigurationPI->setSSID(bus, interfaceIndex, ssid);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

void WifiConfigurationFeature::setPassPhrase(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> passPhrase)
{

	WifiConfigurationProtocolInterface *wifiConfigurationPI = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        wifiConfigurationPI = static_cast<WifiConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		wifiConfigurationPI->setSSID(bus, interfaceIndex, passPhrase);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}



FeatureFamily WifiConfigurationFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.WIFI_CONFIGURATION;
}
