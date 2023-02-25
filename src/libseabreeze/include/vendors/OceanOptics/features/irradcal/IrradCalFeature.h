/***************************************************//**
 * @file    IrradCalFeature.h
 * @date    March 2010
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

#ifndef IRRADCALFEATURE_H
#define IRRADCALFEATURE_H

#include "common/buses/Bus.h"
#include "common/protocols/Protocol.h"
#include "common/features/FeatureImpl.h"
#include "common/exceptions/FeatureException.h"
#include "common/FloatVector.h"
#include "vendors/OceanOptics/features/irradcal/IrradCalFeatureInterface.h"

namespace seabreeze {

    class IrradCalFeature : public FeatureImpl, public IrradCalFeatureInterface {
    public:
        IrradCalFeature(std::vector<ProtocolHelper *> helpers, int numPixels);
        virtual ~IrradCalFeature();

        virtual std::vector<float> *readIrradCalibration(const Protocol &protocol,
            const Bus &bus);
        virtual int writeIrradCalibration(const Protocol &protocol,
            const Bus &bus, const std::vector<float> &values);
        int getNumberOfPixels();
        virtual int hasCollectionArea(const Protocol &protocol,
            const Bus &bus);
        virtual double readCollectionArea(const Protocol &protocol,
            const Bus &bus);
        virtual void writeCollectionArea(const Protocol &protocol,
            const Bus &bus, double area);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();

    private:
        int numberOfPixels;
    };

}

#endif /* IRRADCALFEATURE_H */
