/***************************************************//**
 * @file    MulticastFeature.cpp
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
#include "vendors/OceanOptics/features/multicast/MulticastFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/MulticastProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

MulticastFeature::MulticastFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

MulticastFeature::~MulticastFeature()
{

}

#if 0
vector<unsigned char> MulticastFeature::getGroupAddress(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{

    vector<unsigned char> data;
	MulticastProtocolInterface *multicastPI = NULL;
    ProtocolHelper *proto;

    try
    {
        proto = lookupProtocolImpl(protocol);
		multicastPI = static_cast<MulticastProtocolInterface *>(proto);
    }
	catch (FeatureProtocolNotFoundException &e)
	{
        string error(
        "Could not find matching protocol implementation to get group address.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try
	{
        data = multicastPI->getGroupAddress(bus, interfaceIndex);
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

void MulticastFeature::setGroupAddress(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, const vector<unsigned char> macAddress)
{

    MulticastProtocolInterface *multicastPI = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        multicastPI = static_cast<MulticastProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		multicastPI->set_MAC_Address(bus, interfaceIndex, macAddress);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}
#endif

unsigned char MulticastFeature::getEnableState(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex)
{
	MulticastProtocolInterface *multicastPI = NULL;
	ProtocolHelper *proto;
	unsigned char enableStatus;

    try {
        proto = lookupProtocolImpl(protocol);
		multicastPI = static_cast<MulticastProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to read collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        enableStatus = multicastPI->getEnableState(bus, interfaceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return enableStatus;
}

void MulticastFeature::setEnableState(const Protocol &protocol, const Bus &bus, unsigned char interfaceIndex, unsigned char enableState)
{
	MulticastProtocolInterface *multicastPI = NULL;
	ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
		multicastPI = static_cast<MulticastProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to write enable state");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
		multicastPI->setEnableState(bus, interfaceIndex, enableState);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}


FeatureFamily MulticastFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.IPV4_MULTICAST;
}
