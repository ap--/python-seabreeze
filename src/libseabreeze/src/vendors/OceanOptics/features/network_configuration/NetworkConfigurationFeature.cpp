/***************************************************//**
 * @file    NetworkConfigurationFeature.cpp
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
#include "vendors/OceanOptics/features/network_configuration/NetworkConfigurationFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/NetworkConfigurationProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

NetworkConfigurationFeature::NetworkConfigurationFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

NetworkConfigurationFeature::~NetworkConfigurationFeature()
{

}

unsigned char NetworkConfigurationFeature::getNumberOfNetworkInterfaces(const Protocol &protocol, const Bus &bus)
{

    unsigned char data;
	NetworkConfigurationProtocolInterface *networkConfigurationPI = NULL;
    ProtocolHelper *proto;

    try
    {
        proto = lookupProtocolImpl(protocol);
		networkConfigurationPI = static_cast<NetworkConfigurationProtocolInterface *>(proto);
    }
	catch (FeatureProtocolNotFoundException &e)
	{
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try
	{
        data = networkConfigurationPI->getNumberOfNetworkInterfaces(bus);
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

unsigned char NetworkConfigurationFeature::runNetworkInterfaceSelfTest(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	NetworkConfigurationProtocolInterface *networkConfigurationPI = NULL;
	ProtocolHelper *proto;
	unsigned char selftestStatus;

	try {
		proto = lookupProtocolImpl(protocol);
		networkConfigurationPI = static_cast<NetworkConfigurationProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to read collection area.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		selftestStatus = networkConfigurationPI->runNetworkInterfaceSelfTest(bus, interfaceIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return selftestStatus;
}
unsigned char NetworkConfigurationFeature::getNetworkInterfaceConnectionType(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	NetworkConfigurationProtocolInterface *networkConfigurationPI = NULL;
	ProtocolHelper *proto;
	unsigned char networkType;

	try {
		proto = lookupProtocolImpl(protocol);
		networkConfigurationPI = static_cast<NetworkConfigurationProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to read collection area.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		networkType = networkConfigurationPI->getNetworkInterfaceConnectionType(bus, interfaceIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return networkType;
}

unsigned char NetworkConfigurationFeature::getNetworkInterfaceEnableState(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	NetworkConfigurationProtocolInterface *networkConfigurationPI = NULL;
	ProtocolHelper *proto;
	unsigned char enableStatus;

	try {
		proto = lookupProtocolImpl(protocol);
		networkConfigurationPI = static_cast<NetworkConfigurationProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to read collection area.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		enableStatus = networkConfigurationPI->getNetworkInterfaceConnectionType(bus, interfaceIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return enableStatus;
}


void NetworkConfigurationFeature::setNetworkInterfaceEnableState(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char enableStatus)
{
	NetworkConfigurationProtocolInterface *networkConfigurationPI = NULL;
	ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
		networkConfigurationPI = static_cast<NetworkConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to write collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		networkConfigurationPI->setNetworkInterfaceEnableState(bus, interfaceIndex, enableStatus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}

void NetworkConfigurationFeature::saveNetworkInterfaceConnectionSettings(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	NetworkConfigurationProtocolInterface *networkConfigurationPI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		networkConfigurationPI = static_cast<NetworkConfigurationProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write collection area.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		networkConfigurationPI->saveNetworkInterfaceConnectionSettings(bus, interfaceIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

}

FeatureFamily NetworkConfigurationFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.NETWORK_CONFIGURATION;
}
