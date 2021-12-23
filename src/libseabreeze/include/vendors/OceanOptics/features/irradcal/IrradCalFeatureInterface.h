/***************************************************//**
 * @file    IrradCalFeatureInterface.h
 * @date    February 2011
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the
 * irradiance calibration storage available on some
 * devices (such as the USB2000+).
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

#ifndef IRRADCALFEATUREINTERFACE_H
#define IRRADCALFEATUREINTERFACE_H

#include "common/buses/Bus.h"
#include "common/protocols/Protocol.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    class IrradCalFeatureInterface {
    public:
        virtual ~IrradCalFeatureInterface() = 0;

        virtual std::vector<float> *readIrradCalibration(const Protocol &protocol,
            const Bus &bus) = 0;
        virtual int writeIrradCalibration(const Protocol &protocol,
            const Bus &bus, const std::vector<float> &values) = 0;
        virtual int hasCollectionArea(const Protocol &protocol,
            const Bus &bus) = 0;
        virtual double readCollectionArea(const Protocol &protocol,
            const Bus &bus) = 0;
        virtual void writeCollectionArea(const Protocol &protocol,
            const Bus &bus, double area) = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline IrradCalFeatureInterface::~IrradCalFeatureInterface() {}
}

#endif /* IRRADCALFEATUREINTERFACE_H */
