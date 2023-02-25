/***************************************************//**
 * @file    FPGARegisterFeature.h
 * @date    October 2012
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
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

#ifndef SEABREEZE_FPGA_REGISTER_FEATURE_H
#define SEABREEZE_FPGA_REGISTER_FEATURE_H

#include "common/features/FeatureImpl.h"
#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"
#include "vendors/OceanOptics/features/fpga_register/FPGARegisterFeatureInterface.h"

namespace seabreeze {

    class FPGARegisterFeature : public FeatureImpl, public FPGARegisterFeatureInterface {
    public:
        FPGARegisterFeature();
        virtual ~FPGARegisterFeature();
        virtual unsigned int readRegister(const Bus &bus, unsigned char address);
        virtual void writeRegister(const Bus &bus, unsigned char address, unsigned int value);
        virtual unsigned char getMajorVersion(const Bus &bus);

        virtual FeatureFamily getFeatureFamily();
    };

}

#endif
