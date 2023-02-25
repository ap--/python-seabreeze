/***************************************************//**
 * @file    RawUSBBusAccessFeature.h
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

#ifndef SEABREEZE_RAW_USB_BUS_ACCESS_FEATURE_H
#define SEABREEZE_RAW_USB_BUS_ACCESS_FEATURE_H

#include "common/features/FeatureImpl.h"
#include "common/exceptions/FeatureException.h"
#include "vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeatureInterface.h"
#include "common/buses/usb/USBInterface.h"

namespace seabreeze {

    class RawUSBBusAccessFeature : public FeatureImpl,
            public RawUSBBusAccessFeatureInterface {
    public:
        RawUSBBusAccessFeature();
        virtual ~RawUSBBusAccessFeature();
        virtual std::vector<unsigned char> readUSB(const USBInterface *bus, int endpoint,
            unsigned int length);
        virtual int writeUSB(const USBInterface *bus, int endpoint,
            const std::vector<unsigned char> &data);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();
    };
}

#endif
