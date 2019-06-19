/***************************************************//**
 * @file    Feature.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is an abstract interface that other feature types will
 * implement.  A Feature is taken to be some capability of
 * a Device that is relatively self-contained.  For instance,
 * an analog output voltage would be a Feature.  Features can
 * also contain multiple functions; for instance, a TEC may
 * be able to report a temperature, take a temperature as a
 * set point, and turn on or off.  These capabilities are
 * interrelated, and would be considered a single feature.
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

#ifndef FEATURE_H
#define FEATURE_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include "common/features/FeatureFamily.h"
#include "common/exceptions/FeatureException.h"
#include "common/protocols/Protocol.h"
#include <vector>

namespace seabreeze {

    class Feature {
    public:
        virtual ~Feature() = 0;

        /* Allow the object that represents a given feature to initialize
         * itself by reading from the corresponding feature on the real
         * device, and/or put the real device feature into a known state.
         * This should return true if the feature is ready to be used, and false
         * otherwise.
         */
        virtual bool initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException) = 0;

        virtual FeatureFamily getFeatureFamily() = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline Feature::~Feature() {}
}

#endif /* FEATURE_H */
