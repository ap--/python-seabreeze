/***************************************************//**
 * @file    FastBufferFeatureInterface.h
 * @date    February 2017
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

#ifndef FASTBUFFERFEATUREINTERFACE_H
#define FASTBUFFERFEATUREINTERFACE_H

#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    typedef unsigned char FastBufferIndex_t;
    typedef FastBufferIndex_t FastBufferCount_t;
    typedef unsigned int FastBufferSampleCount_t;
    typedef unsigned long FastBufferElementCount_t;

    class FastBufferFeatureInterface {
    public:
        virtual ~FastBufferFeatureInterface() = 0;

		virtual FastBufferIndex_t getBufferingEnable(const Protocol &protocol,
			const Bus &bus, const FastBufferIndex_t bufferIndex) = 0;
		virtual void setBufferingEnable(const Protocol &protocol, const Bus &bus,
			const FastBufferIndex_t bufferIndex,
			const FastBufferIndex_t isEnabled) = 0;
		virtual FastBufferSampleCount_t getConsecutiveSampleCount(const Protocol &protocol,
			const Bus &bus, const FastBufferIndex_t bufferIndex) = 0;
		virtual void setConsecutiveSampleCount(const Protocol &protocol, const Bus &bus,
			const FastBufferIndex_t bufferIndex,
			const FastBufferSampleCount_t consecutiveSampleCount) = 0;

    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline FastBufferFeatureInterface::~FastBufferFeatureInterface() {}

} /* end namespace */


#endif /* FASTBUFFERFEATUREINTERFACE_H */
