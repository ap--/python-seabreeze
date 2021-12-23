/***************************************************//**
 * @file    AcquisitionDelayFeatureAdapter.h
 * @date    November 2015
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows access to SeaBreeze
 * AcquisitionDelayFeatureInterface instances.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc
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

#ifndef SEABREEZE_ACQUISITION_DELAY_FEATURE_ADAPTER_H
#define SEABREEZE_ACQUISITION_DELAY_FEATURE_ADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/acquisition_delay/AcquisitionDelayFeatureInterface.h"

namespace seabreeze {
    namespace api {

        class AcquisitionDelayFeatureAdapter
                : public FeatureAdapterTemplate<AcquisitionDelayFeatureInterface> {
        public:
            AcquisitionDelayFeatureAdapter(AcquisitionDelayFeatureInterface *intf,
                const FeatureFamily &f, Protocol *p, Bus *b,
                unsigned short instanceIndex);

            virtual ~AcquisitionDelayFeatureAdapter();

            /* Acquisition delay functions */
            unsigned long getAcquisitionDelayIncrementMicroseconds(int *errorCode);
            unsigned long getAcquisitionDelayMaximumMicroseconds(int *errorCode);
            unsigned long getAcquisitionDelayMinimumMicroseconds(int *errorCode);
            unsigned long getAcquisitionDelayMicroseconds(int *errorCode);
            void setAcquisitionDelayMicroseconds(int *errorCode, const unsigned long delay_usec);
        };

    } /* end namespace api */
} /* end namespace seabreeze */

#endif /* SEABREEZE_ACQUISITION_DELAY_FEATURE_ADAPTER_H */
