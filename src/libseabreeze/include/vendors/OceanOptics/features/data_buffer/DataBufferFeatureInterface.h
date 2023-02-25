/***************************************************//**
 * @file    DataBufferFeatureInterface.h
 * @date    October 2017
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

#ifndef DATABUFFERFEATUREINTERFACE_H
#define DATABUFFERFEATUREINTERFACE_H

#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    typedef unsigned char DataBufferIndex_t;
    typedef DataBufferIndex_t DataBufferCount_t;
    typedef unsigned long DataBufferElementCount_t;

    class DataBufferFeatureInterface {
    public:
        virtual ~DataBufferFeatureInterface() = 0;

        virtual DataBufferCount_t getNumberOfBuffers() = 0;
        virtual void clearBuffer(const Protocol &protocol, const Bus &bus,
                const DataBufferIndex_t bufferIndex) = 0;
        virtual void removeOldestSpectraFromBuffer(const Protocol &protocol, const Bus &bus,
            const DataBufferIndex_t bufferIndex, const unsigned int numberOfSpectra) = 0;
        virtual DataBufferElementCount_t getBufferCapacity(const Protocol &protocol,
                const Bus &bus, const DataBufferIndex_t bufferIndex) = 0;
		virtual DataBufferElementCount_t getBufferCapacityMaximum(
                const Protocol &protocol, const Bus &bus,
                const DataBufferIndex_t bufferIndex) = 0;
        virtual DataBufferElementCount_t getBufferCapacityMinimum(
                const Protocol &protocol, const Bus &bus,
                const DataBufferIndex_t bufferIndex) = 0;
        virtual DataBufferElementCount_t getNumberOfElements(
                const Protocol &protocol, const Bus &bus,
                const DataBufferIndex_t bufferIndex) = 0;
        virtual void setBufferCapacity(const Protocol &protocol, const Bus &bus,
                const DataBufferIndex_t bufferIndex,
                const DataBufferElementCount_t bufferSize) = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline DataBufferFeatureInterface::~DataBufferFeatureInterface() {}

} /* end namespace */


#endif /* DATABUFFERFEATUREINTERFACE_H */
