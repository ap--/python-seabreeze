/***************************************************//**
 * @file    AcquisitionDelayFeatureInterface.h
 * @date    November 2015
 * @author  Ocean Optics, Inc.
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

#ifndef SEABREEZE_ACQUISITION_DELAY_FEATURE_INTERFACE_H
#define SEABREEZE_ACQUISITION_DELAY_FEATURE_INTERFACE_H

#include "common/features/Feature.h"
#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    class AcquisitionDelayFeatureInterface {
    public:
        virtual ~AcquisitionDelayFeatureInterface();
        virtual unsigned long getAcquisitionDelayIncrementMicroseconds(
                    const Protocol &protocol, const Bus &bus)
                    throw (FeatureException) = 0;
        virtual unsigned long getAcquisitionDelayMaximumMicroseconds(
                    const Protocol &protocol, const Bus &bus)
                    throw (FeatureException) = 0;
        virtual unsigned long getAcquisitionDelayMinimumMicroseconds(
                    const Protocol &protocol, const Bus &bus)
                    throw (FeatureException) = 0;
        virtual unsigned long getAcquisitionDelayMicroseconds(
                    const Protocol &protocol, const Bus &bus)
                    throw (FeatureException) = 0;
        virtual void setAcquisitionDelayMicroseconds(
                    const Protocol &Protocol, const Bus &bus,
                    const unsigned long delayMicros)
                    throw (FeatureException) = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline AcquisitionDelayFeatureInterface::~AcquisitionDelayFeatureInterface() {}

} /* end namespace */

#endif /* SEABREEZE_ACQUISITION_DELAY_FEATURE_INTERFACE_H */

