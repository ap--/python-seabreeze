/***************************************************//**
 * @file    EthernetConfigurationFeature.cpp
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
#include "vendors/OceanOptics/features/ethernet_configuration/EthernetConfigurationFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/EthernetConfigurationProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

EthernetConfigurationFeature::EthernetConfigurationFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

EthernetConfigurationFeature::~EthernetConfigurationFeature()
{

}

vector<unsigned char> EthernetConfigurationFeature::get_MAC_Address(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{

    vector<unsigned char> data;
	EthernetConfigurationProtocolInterface *ethernetConfigurationPI = NULL;
    ProtocolHelper *proto;

    try
    {
        proto = lookupProtocolImpl(protocol);
		ethernetConfigurationPI = static_cast<EthernetConfigurationProtocolInterface *>(proto);
    }
	catch (FeatureProtocolNotFoundException &e)
	{
        string error(
        "Could not find matching protocol implementation to get ethernet configuration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try
	{
        data = ethernetConfigurationPI->get_MAC_Address(bus, interfaceIndex);
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

void EthernetConfigurationFeature::set_MAC_Address(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> macAddress)
{

    EthernetConfigurationProtocolInterface *ethernetConfigurationPI = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        ethernetConfigurationPI = static_cast<EthernetConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		ethernetConfigurationPI->set_MAC_Address(bus, interfaceIndex, macAddress);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}


unsigned char EthernetConfigurationFeature::get_GbE_Enable_Status(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	EthernetConfigurationProtocolInterface *ethernetConfigurationPI = NULL;
	ProtocolHelper *proto;
	unsigned char enableStatus;

    try {
        proto = lookupProtocolImpl(protocol);
		ethernetConfigurationPI = static_cast<EthernetConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to read collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        enableStatus = ethernetConfigurationPI->get_GbE_Enable_Status(bus, interfaceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return enableStatus;
}

void EthernetConfigurationFeature::set_GbE_Enable_Status(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char enableStatus)
{
	EthernetConfigurationProtocolInterface *ethernetConfigurationPI = NULL;
	ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
		ethernetConfigurationPI = static_cast<EthernetConfigurationProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to write collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		ethernetConfigurationPI->set_GbE_Enable_Status(bus, interfaceIndex, enableStatus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}


FeatureFamily EthernetConfigurationFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.ETHERNET_CONFIGURATION;
}
