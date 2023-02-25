/***************************************************//**
 * @file    FastBufferFeatureAdapter.h
 * @date    February 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows access to SeaBreeze
 * DataBufferFeatureInterface instances.
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

#ifndef SEABREEZE_FAST_BUFFER_FEATURE_ADAPTER_H
#define SEABREEZE_FAST_BUFFER_FEATURE_ADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/fast_buffer/FastBufferFeatureInterface.h"

namespace seabreeze {
    namespace api {

        class FastBufferFeatureAdapter
                : public FeatureAdapterTemplate<FastBufferFeatureInterface> {
        public:
            FastBufferFeatureAdapter(FastBufferFeatureInterface *intf,
                const FeatureFamily &f,
                Protocol *p, Bus *b, unsigned short instanceIndex);
            virtual ~FastBufferFeatureAdapter();

            /* Data buffer functions */
            unsigned char getBufferingEnable(int *errorCode);
            void setBufferingEnable(int *errorCode, unsigned char isEnabled);
			unsigned int getConsecutiveSampleCount(int *errorCode);
			void setConsecutiveSampleCount(int *errorCode, unsigned int consecutiveSampleCount);
        };

    } /* end namespace api */
} /* end namespace seabreeze */

#endif /* SEABREEZE_FAST_BUFFER_FEATURE_ADAPTER_H */
