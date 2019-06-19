/***************************************************//**
 * @file    TemperatureFeatureInterface.h
 * @date    January 2015
 * @author  Ocean Optics, Inc., Kirk Clendinning, Heliospectra
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

#ifndef TEMPERATUREFEATUREINTERFACE_H
#define TEMPERATUREFEATUREINTERFACE_H

#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    class TemperatureFeatureInterface {
    public:
        virtual ~TemperatureFeatureInterface() = 0;
        virtual unsigned char readTemperatureCount(const Protocol &protocol,
                const Bus &bus) throw (FeatureException) = 0;
        virtual double readTemperature(const Protocol &protocol,
                const Bus &bus, int index) throw (FeatureException) = 0;
        virtual std::vector<double> *readAllTemperatures(const Protocol &protocol,
                const Bus &bus) throw (FeatureException) = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline TemperatureFeatureInterface::~TemperatureFeatureInterface() {}
}

#endif /* TEMPERATUREFEATUREINTERFACE_H */