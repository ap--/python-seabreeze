/***************************************************//**
 * @file    FastBufferFeature.h
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

#ifndef FASTBUFFERFEATUREBASE_H
#define FASTBUFFERFEATUREBASE_H

#include <vector>

#include "vendors/OceanOptics/features/fast_buffer/FastBufferFeatureInterface.h"
#include "common/features/FeatureImpl.h"

namespace seabreeze {

    class FastBufferFeatureBase : public FeatureImpl, public FastBufferFeatureInterface {
    public:
        FastBufferFeatureBase();
        virtual ~FastBufferFeatureBase();

		virtual FastBufferIndex_t getBufferingEnable(
			const Protocol &protocol,
			const Bus &bus, const FastBufferIndex_t bufferIndex);
		virtual void setBufferingEnable(
			const Protocol &protocol,
			const Bus &bus,
			const FastBufferIndex_t bufferIndex,
			const FastBufferIndex_t bufferSize);
		virtual FastBufferSampleCount_t getConsecutiveSampleCount(
			const Protocol &protocol,
			const Bus &bus,
			const FastBufferIndex_t bufferIndex);
		virtual void setConsecutiveSampleCount(
			const Protocol &protocol,
			const Bus &bus,
			const FastBufferIndex_t bufferIndex,
			const FastBufferSampleCount_t consecutiveSampleCount);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();
    };

} /* end namespace */

#endif /* FASTBUFFERFEATUREBASE_H */
