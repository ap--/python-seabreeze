/***************************************************//**
 * @file    DataBufferFeatureBase.cpp
 * @date    October 2017
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the data buffer
 * for devices with a clean protocol implementation.
 * This is an abstract base class but it does much of
 * the work needed for most implementations that can
 * delegate almost everything to the protocol layer.
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
#include "vendors/OceanOptics/features/data_buffer/DataBufferFeatureBase.h"
#include "vendors/OceanOptics/protocols/interfaces/DataBufferProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPDataBufferProtocol.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/exceptions/FeatureControlException.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

DataBufferFeatureBase::DataBufferFeatureBase() {

}

DataBufferFeatureBase::~DataBufferFeatureBase() {

}

DataBufferCount_t DataBufferFeatureBase::getNumberOfBuffers() {
    return this->numberOfBuffers;
}

void DataBufferFeatureBase::clearBuffer(const Protocol &protocol,
        const Bus &bus, const DataBufferIndex_t bufferIndex) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to clear data buffer.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        buffer->clearBuffer(bus, bufferIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}

void DataBufferFeatureBase::removeOldestSpectraFromBuffer(const Protocol &protocol,
        const Bus &bus, const DataBufferIndex_t bufferIndex, unsigned int numberOfSpectra) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to remove the oldest spectra from the data buffer.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        buffer->removeOldestSpectraFromBuffer(bus, bufferIndex, numberOfSpectra);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}

DataBufferElementCount_t DataBufferFeatureBase::getNumberOfElements(
    const Protocol &protocol, const Bus &bus,
    const DataBufferIndex_t bufferIndex) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to get data buffer element count.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    DataBufferElementCount_t retval = 0;

    try {
        retval = buffer->getNumberOfElements(bus, bufferIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

DataBufferElementCount_t DataBufferFeatureBase::getBufferCapacity(
        const Protocol &protocol, const Bus &bus,
        const DataBufferIndex_t bufferIndex) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to get data buffer capacity.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    DataBufferElementCount_t retval = 0;

    try {
        retval = buffer->getBufferCapacity(bus, bufferIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}


DataBufferElementCount_t DataBufferFeatureBase::getBufferCapacityMinimum(
        const Protocol &protocol, const Bus &bus,
        const DataBufferIndex_t bufferIndex) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to get data buffer minimum capacity.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    DataBufferElementCount_t retval = 0;

    try {
        retval = buffer->getBufferCapacityMinimum(bus, bufferIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

DataBufferElementCount_t DataBufferFeatureBase::getBufferCapacityMaximum(
        const Protocol &protocol, const Bus &bus,
        const DataBufferIndex_t bufferIndex) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to get data buffer maximum capacity.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    DataBufferElementCount_t retval = 0;

    try {
        retval = buffer->getBufferCapacityMaximum(bus, bufferIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

void DataBufferFeatureBase::setBufferCapacity(const Protocol &protocol,
        const Bus &bus, const DataBufferIndex_t bufferIndex,
        const DataBufferElementCount_t bufferSize) {

    DataBufferProtocolInterface *buffer = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        buffer = static_cast<DataBufferProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to set data buffer capacity.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        buffer->setBufferCapacity(bus, bufferIndex, bufferSize);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

FeatureFamily DataBufferFeatureBase::getFeatureFamily() {
    FeatureFamilies families;

    return families.DATA_BUFFER;
}
