/***************************************************//**
 * @file    LightSourceFeatureAdapter.h
 * @date    May 2013
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze LightSourceFeatureInterface instances.
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

#ifndef SEABREEZE_LIGHTSOURCEFEATUREADAPTER_H
#define SEABREEZE_LIGHTSOURCEFEATUREADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/light_source/LightSourceFeatureInterface.h"

namespace seabreeze {
    namespace api {

        class LightSourceFeatureAdapter
                : public FeatureAdapterTemplate<LightSourceFeatureInterface> {
        public:
            LightSourceFeatureAdapter(LightSourceFeatureInterface *intf,
                    const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex);
            virtual ~LightSourceFeatureAdapter();

            int getLightSourceCount(int *errorCode);

            bool hasLightSourceEnable(int *errorCode, int lightSourceIndex);
            bool isLightSourceEnabled(int *errorCode, int lightSourceIndex);
            void setLightSourceEnable(int *errorCode, int lightSourceIndex,
                    bool enable);

            /* The intensity is normalized over the range [0, 1] where 0 is
             * the minimum programmable intensity and 1 is the maximum
             */
            bool hasVariableIntensity(int *errorCode, int lightSourceIndex);
            double getLightSourceIntensity(int *errorCode, int lightSourceIndex);
            void setLightSourceIntensity(int *errorCode, int lightSourceIndex,
                    double intensity);
        };

    }
}

#endif
