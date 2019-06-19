/***************************************************//**
 * @file    FeatureImpl.h
 * @date    March 2016
 * @author  Ocean Optics, Inc.
 *
 * This is a simple base class that other feature types will
 * extend.  A Feature is taken to be some capability of
 * a Device that is relatively self-contained.  For instance,
 * an analog output voltage would be a Feature.  Features can
 * also contain multiple functions; for instance, a TEC may
 * be able to report a temperature, take a temperature as a
 * set point, and turn on or off.  These capabilities are
 * interrelated, and would be considered a single feature.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2016, Ocean Optics Inc
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

#ifndef FEATUREIMPL_H
#define FEATUREIMPL_H

#include "common/SeaBreeze.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"
#include "common/features/Feature.h"
#include "common/protocols/ProtocolHelper.h"
#include <vector>

namespace seabreeze {
    /* This does virtual inheritance from Feature because it is assumed that
     * in some cases there will be diamond inheritance.  This will generally
     * only happen where one top-level Feature is implemented by deriving from
     * another Feature.
     */
    class FeatureImpl : public virtual Feature {
    public:
        FeatureImpl();
        virtual ~FeatureImpl();

        /* Allow the object that represents a given feature to initialize
         * itself by reading from the corresponding feature on the real
         * device, and/or put the real device feature into a known state.
         * Overriding this is not required.  This should return true if
         * the feature is ready to be used, and false otherwise.
         */
        virtual bool initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException);

        virtual FeatureFamily getFeatureFamily() = 0;

    protected:
        std::vector<ProtocolHelper *> protocols;

        /* Protocols are described by their base class (Protocol)
         * and may be designated that way.  However, different
         * functionality within a given command set may be broken
         * into different implementation types, all of which extend
         * the base Protocol class.  This is a simple lookup mechanism
         * to use the Protocol that some anonymous caller might
         * provide as a point of reference to then find the extended
         * Protocol class that can be used to access certain features.
         */
        ProtocolHelper *lookupProtocolImpl(const Protocol &protocol)
                        throw (FeatureProtocolNotFoundException);
    };

}

#endif /* FEATUREIMPL_H */
