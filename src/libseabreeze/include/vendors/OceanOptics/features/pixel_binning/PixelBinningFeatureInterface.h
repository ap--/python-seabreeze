/***************************************************//**
 * @file    PixelBinningFeatureInterface.h
 * @date    October 2015
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the thermo-
 * electric cooler (TEC) feature.
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

#ifndef PIXELBINNINGFEATUREINTERFACE_H
#define PIXELBINNINGFEATUREINTERFACE_H

#include "common/buses/Bus.h"
#include "common/protocols/Protocol.h"
#include "common/exceptions/FeatureException.h"
#include "common/exceptions/IllegalArgumentException.h"

namespace seabreeze {

    class PixelBinningFeatureInterface {
    public:
        virtual ~PixelBinningFeatureInterface() = 0;
        virtual void setPixelBinningFactor(const Protocol &protocol,
                const Bus &bus, const unsigned char binningFactor) = 0;
        virtual unsigned char getPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) = 0;
        virtual void setDefaultPixelBinningFactor(const Protocol &protocol,
                const Bus &bus, const unsigned char binningFactor) = 0;
        virtual void setDefaultPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) = 0;
        virtual unsigned char getDefaultPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) = 0;
        virtual unsigned char getMaxPixelBinningFactor(const Protocol &protocol,
                const Bus &bus) = 0;
    };
}

#endif /* PIXELBINNINGFEATUREINTERFACE_H */
