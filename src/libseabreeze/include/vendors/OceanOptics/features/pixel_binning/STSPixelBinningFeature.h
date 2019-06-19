/***************************************************//**
 * @file    STSPixelBinningFeature.h
 * @date    October 2015
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the thermo-
 * electric unit for devices with a clean
 * protocol implementation.  This is an abstract base
 * class but it does much of the work needed for most
 * implementations that can delegate almost everything
 * to the protocol layer.
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

#ifndef STSPIXELBINNINGFEATURE_H
#define STSPIXELBINNINGFEATURE_H

#include "common/buses/Bus.h"
#include "common/protocols/Protocol.h"
#include "common/protocols/ProtocolHelper.h"
#include "common/features/FeatureImpl.h"
#include "common/exceptions/FeatureException.h"
#include "common/exceptions/IllegalArgumentException.h"
#include "vendors/OceanOptics/features/pixel_binning/PixelBinningFeatureInterface.h"

namespace seabreeze {

    class STSSpectrometerFeature;

    class STSPixelBinningFeature : public FeatureImpl,
            public PixelBinningFeatureInterface {
    public:
        STSPixelBinningFeature(std::vector<ProtocolHelper *> helpers,
                STSSpectrometerFeature *spectroFeature);
        virtual ~STSPixelBinningFeature();

        virtual void setPixelBinningFactor(const Protocol &protocol,
                const Bus &bus, const unsigned char binningFactor)
                throw (FeatureException);
        virtual unsigned char getPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) throw (FeatureException);
        virtual void setDefaultPixelBinningFactor(const Protocol &protocol,
                const Bus &bus, const unsigned char binningFactor)
                throw (FeatureException);
        virtual void setDefaultPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) throw (FeatureException);
        virtual unsigned char getDefaultPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) throw (FeatureException);
        virtual unsigned char getMaxPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) throw (FeatureException);

        /* Inherited from Feature */
        /* This is still abstract in case the device needs to do anything
         * special with regards to defaults when it is initialized.
         */
        virtual bool initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException);


        virtual FeatureFamily getFeatureFamily();

    protected:
        STSSpectrometerFeature *spectrometerFeature;
    };
}

#endif /* STSPIXELBINNINGFEATURE_H */
