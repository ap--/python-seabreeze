/***************************************************//**
 * @file    TemperatureFeature.h
 * @date    January 2015
 * @author  Kirk Clendinning, Heliospectra
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc, Heliospectra AB
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

#ifndef TEMPERATUREFEATURE_H
#define TEMPERATUREFEATURE_H

#include <vector>

#include "vendors/OceanOptics/features/temperature/TemperatureFeatureInterface.h"
#include "common/protocols/Protocol.h"
#include "common/features/FeatureImpl.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    class TemperatureFeature
                : public FeatureImpl, public TemperatureFeatureInterface {
    public:
        TemperatureFeature(std::vector<ProtocolHelper *> helpers);
        virtual ~TemperatureFeature();

        virtual unsigned char readTemperatureCount(const Protocol &protocol,
                const Bus &bus);

        virtual double readTemperature(const Protocol &protocol,
                const Bus &bus, int index);

        virtual std::vector<double> *readAllTemperatures(
                const Protocol &protocol, const Bus &bus);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();
    };

}

#endif /* TEMPERATUREFEATURE_H */
