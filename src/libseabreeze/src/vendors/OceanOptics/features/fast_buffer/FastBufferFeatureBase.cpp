/***************************************************//**
 * @file    FastBufferFeatureBase.cpp
 * @date    February 2017
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the fast buffer
 * for devices with a clean protocol implementation.
 * This is an abstract base class but it does much of
 * the work needed for most implementations that can
 * delegate almost everything to the protocol layer.
 * It is used in conjunction with the data buffer feature
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
#include "vendors/OceanOptics/features/fast_buffer/FastBufferFeatureBase.h"
#include "vendors/OceanOptics/protocols/interfaces/FastBufferProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPFastBufferProtocol.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/exceptions/FeatureControlException.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

FastBufferFeatureBase::FastBufferFeatureBase() {

}

FastBufferFeatureBase::~FastBufferFeatureBase() {

}


FastBufferIndex_t FastBufferFeatureBase::getBufferingEnable(
	const Protocol &protocol, const Bus &bus,
	const FastBufferIndex_t bufferIndex)
{

	FastBufferProtocolInterface *buffer = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		buffer = static_cast<FastBufferProtocolInterface *>(proto);
	}
	catch (const FeatureProtocolNotFoundException &fpnfe) {
		string error(
			"Could not find matching protocol implementation to get data buffer capacity.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	FastBufferIndex_t retval = 0;

	try {
		retval = buffer->getBufferingEnable(bus, bufferIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
	return retval;
}

void FastBufferFeatureBase::setBufferingEnable(const Protocol &protocol,
	const Bus &bus, const FastBufferIndex_t bufferIndex,
	const FastBufferIndex_t isEnabled) {

	FastBufferProtocolInterface *buffer = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		buffer = static_cast<FastBufferProtocolInterface *>(proto);
	}
	catch (const FeatureProtocolNotFoundException &fpnfe) {
		string error(
			"Could not find matching protocol implementation to set data buffer capacity.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		buffer->setBufferingEnable(bus, bufferIndex, isEnabled);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}

FastBufferSampleCount_t FastBufferFeatureBase::getConsecutiveSampleCount(
	const Protocol &protocol, const Bus &bus,
	const FastBufferIndex_t bufferIndex) {

	FastBufferProtocolInterface *buffer = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		buffer = static_cast<FastBufferProtocolInterface *>(proto);
	}
	catch (const FeatureProtocolNotFoundException &fpnfe) {
		string error(
			"Could not find matching protocol implementation to get data buffer capacity.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	FastBufferSampleCount_t retval = 0;

	try {
		retval = buffer->getConsecutiveSampleCount(bus, bufferIndex);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
	return retval;
}

void FastBufferFeatureBase::setConsecutiveSampleCount(const Protocol &protocol,
	const Bus &bus, const FastBufferIndex_t bufferIndex,
	const FastBufferSampleCount_t consecutiveSampleCount) {

	FastBufferProtocolInterface *buffer = NULL;
	ProtocolHelper *proto = NULL;

	try {
		proto = lookupProtocolImpl(protocol);
		buffer = static_cast<FastBufferProtocolInterface *>(proto);
	}
	catch (const FeatureProtocolNotFoundException &fpnfe) {
		string error(
			"Could not find matching protocol implementation to set data buffer capacity.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		buffer->setConsecutiveSampleCount(bus, bufferIndex, consecutiveSampleCount);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}

FeatureFamily FastBufferFeatureBase::getFeatureFamily() {
    FeatureFamilies families;

    return families.FAST_BUFFER;
}
