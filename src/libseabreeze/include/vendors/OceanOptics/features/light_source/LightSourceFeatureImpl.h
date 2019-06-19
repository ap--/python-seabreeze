/***************************************************//**
 * @file    LightSourceFeatureImpl.h
 * @date    April 2013
 * @author  Ocean Optics, Inc.
 *
 * This is a trivial implementation of LightSourceFeatureInterface
 * that can manage a single module with one or more light sources
 * (e.g. light bulbs, lasers, LEDs).
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

#ifndef LIGHTSOURCEFEATUREIMPL_H
#define LIGHTSOURCEFEATUREIMPL_H

#include "vendors/OceanOptics/features/light_source/LightSourceFeatureBase.h"

namespace seabreeze {

    class LightSourceFeatureImpl : public LightSourceFeatureBase {
    public:
        LightSourceFeatureImpl(std::vector<ProtocolHelper *> helpers,
            int lampModuleIndex, int lightSources);
        virtual ~LightSourceFeatureImpl();

        /* Override from LightSourceFeatureBase */
        virtual int getLightSourceCount(const Protocol &protocol,
            const Bus &bus) throw (FeatureException);

    protected:
        int lightSourceCount;
    };

}

#endif /* LIGHTSOURCEFEATUREIMPL_H */
