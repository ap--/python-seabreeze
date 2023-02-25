/***************************************************//**
 * @file    AcquisitionDelayFeature_FPGA.h
 * @date    November 2015
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the acquisition
 * delay feature that is controlled by the FPGA in many
 * devices.  This is likely to be derived from to
 * override the device limits.
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

#ifndef ACQUISITION_DELAY_FEATURE_FPGA_H
#define ACQUISITION_DELAY_FEATURE_FPGA_H

#include "vendors/OceanOptics/features/acquisition_delay/AcquisitionDelayFeature.h"
#include <vector>

namespace seabreeze {

    class AcquisitionDelayFeature_FPGA : public AcquisitionDelayFeature {
    public:
        AcquisitionDelayFeature_FPGA(std::vector<ProtocolHelper *> helpers);
        virtual ~AcquisitionDelayFeature_FPGA();

        /* Inherited from AcquisitionDelayFeature */

        virtual void setAcquisitionDelayMicroseconds(
                    const Protocol &protocol, const Bus &bus,
                    const unsigned long delayMicros);

        virtual unsigned long getAcquisitionDelayIncrementMicroseconds(
                    const Protocol &protocol, const Bus &bus);
        virtual unsigned long getAcquisitionDelayMaximumMicroseconds(
                    const Protocol &protocol, const Bus &bus);
        virtual unsigned long getAcquisitionDelayMinimumMicroseconds(
                    const Protocol &protocol, const Bus &bus);

    protected:
        virtual unsigned long countsToMicroseconds(unsigned long counts);
        virtual unsigned long microsecondsToCounts(unsigned long microseconds);

        unsigned char acquisitionDelayRegister;
        unsigned long countsPerMicrosecond;
        unsigned long minimumDelayCounts;
        unsigned long maximumDelayCounts;
        unsigned long incrementMicroseconds;
    };

} /* end namespace seabreeze */

#endif /* ACQUISITION_DELAY_FEATURE_FPGA_H */
