/***************************************************//**
 * @file    LightSourceFeatureBase.h
 * @date    April 2013
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

#ifndef LIGHTSOURCEFEATUREBASE_H
#define LIGHTSOURCEFEATUREBASE_H

#include "common/features/FeatureImpl.h"
#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"
#include "vendors/OceanOptics/features/light_source/LightSourceFeatureInterface.h"

namespace seabreeze {

    class LightSourceFeatureBase : public FeatureImpl,
            public LightSourceFeatureInterface {
    public:
        LightSourceFeatureBase(std::vector<ProtocolHelper *> helpers, int lampModuleIndex);
        virtual ~LightSourceFeatureBase();

        /* Enable controls */
        virtual bool hasLightSourceEnable(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex) throw (FeatureException);

        virtual bool isLightSourceEnabled(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex) throw (FeatureException);

        virtual void setLightSourceEnable(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex, bool enable) throw (FeatureException);

        /* Intensity controls */
        virtual bool hasVariableIntensity(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex) throw (FeatureException);

        virtual double getLightSourceIntensity(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex) throw (FeatureException);

        virtual void setLightSourceIntensity(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex, double intensity) throw (FeatureException);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();

    protected:
        int moduleIndex;
    };

}

#endif /* LIGHTSOURCEFEATUREBASE_H */
