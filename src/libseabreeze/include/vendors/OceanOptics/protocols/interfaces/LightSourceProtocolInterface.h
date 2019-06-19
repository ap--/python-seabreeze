/***************************************************//**
 * @file    LightSourceProtocolInterface.h
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

#ifndef LIGHTSOURCEPROTOCOLINTERFACE_H
#define LIGHTSOURCEPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class LightSourceProtocolInterface : public ProtocolHelper {
    public:
        LightSourceProtocolInterface(Protocol *proto);
        virtual ~LightSourceProtocolInterface();

        virtual bool hasLightSourceEnable(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual bool isLightSourceEnabled(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual bool hasVariableIntensity(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual void setLightSourceEnable(const Bus &bus, int moduleIndex,
            int lightSourceIndex, bool enable) throw (ProtocolException) = 0;
    };

    class LightSourceProtocolInterface_NormalizedIntensity {
    public:
        LightSourceProtocolInterface_NormalizedIntensity();
        virtual ~LightSourceProtocolInterface_NormalizedIntensity();

        virtual double getIntensity(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual void setIntensity(const Bus &bus, int moduleIndex,
            int lightSourceIndex, double intensity) throw (ProtocolException) = 0;

        virtual double getIntensityMinimum(const Bus &bus, int moduleIndex,
            int lightSourceIndex) = 0;

        virtual double getIntensityMaximum(const Bus &bus, int moduleIndex,
            int lightSourceIndex) = 0;
    };

    class LightSourceProtocolInterface_Counts {
    public:
        LightSourceProtocolInterface_Counts();
        virtual ~LightSourceProtocolInterface_Counts();

        virtual int getIntensityCounts(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual int getIntensityMinimumCounts(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual int getIntensityMaximumCounts(const Bus &bus, int moduleIndex,
            int lightSourceIndex) throw (ProtocolException) = 0;

        virtual void setIntensityCounts(const Bus &bus, int moduleIndex,
            int lightSourceIndex, int counts) throw (ProtocolException) = 0;
    };

}

#endif /* LIGHTSOURCEPROTOCOLINTERFACE_H */
