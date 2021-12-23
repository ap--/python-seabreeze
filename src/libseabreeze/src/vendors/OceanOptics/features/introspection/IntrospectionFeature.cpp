/***************************************************//**
 * @file    IntrospectionFeature.cpp
 * @date    January 2017
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the Introspection Feature.
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
#include "vendors/OceanOptics/features/introspection/IntrospectionFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/IntrospectionProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPIntrospectionProtocol.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace seabreeze::api;
using namespace std;

IntrospectionFeature::IntrospectionFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }
}

IntrospectionFeature::~IntrospectionFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned short IntrospectionFeature::getNumberOfPixels(const Protocol &protocol, const Bus &bus)
{

	IntrospectionProtocolInterface *introspection_protocolInterface = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		introspection_protocolInterface = static_cast<IntrospectionProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get temperature.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		return introspection_protocolInterface->getNumberOfPixels(bus);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}


vector<unsigned int> *IntrospectionFeature::getActivePixelRanges(const Protocol &protocol, const Bus &bus) {

	IntrospectionProtocolInterface *introspection_protocolInterface = NULL;
	vector<unsigned int> *pixelIndexPairs = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		introspection_protocolInterface = static_cast<IntrospectionProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get temperature.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		pixelIndexPairs = introspection_protocolInterface->getActivePixelRanges(bus);
		return pixelIndexPairs;
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	/* Unreachable */
	return NULL;
}

vector<unsigned int> *IntrospectionFeature::getElectricDarkPixelRanges(const Protocol &protocol, const Bus &bus) {

	IntrospectionProtocolInterface *introspection_protocolInterface = NULL;
	vector<unsigned int> *pixelIndexPairs = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		introspection_protocolInterface = static_cast<IntrospectionProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get temperature.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		pixelIndexPairs = introspection_protocolInterface->getElectricDarkPixelRanges(bus);
		return pixelIndexPairs;
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	/* Unreachable */
	return NULL;
}

vector<unsigned int> *IntrospectionFeature::getOpticalDarkPixelRanges(const Protocol &protocol, const Bus &bus) {

	IntrospectionProtocolInterface *introspection_protocolInterface = NULL;
	vector<unsigned int> *pixelIndexPairs = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		introspection_protocolInterface = static_cast<IntrospectionProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get temperature.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		pixelIndexPairs = introspection_protocolInterface->getOpticalDarkPixelRanges(bus);
		return pixelIndexPairs;
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	/* Unreachable */
	return NULL;
}

FeatureFamily IntrospectionFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.INTROSPECTION;
}
