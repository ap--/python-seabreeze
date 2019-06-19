/***************************************************//**
 * @file    FeatureAdapterInterface.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This interface allows device features to be treated
 * consistently regardless of the actual capabilities.
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

#ifndef SEABREEZE_FEATUREADAPTERINTERFACE_H
#define SEABREEZE_FEATUREADAPTERINTERFACE_H

#include "common/features/FeatureFamily.h"

namespace seabreeze {
    namespace api {

        class FeatureAdapterInterface {
        public:
            virtual ~FeatureAdapterInterface() = 0;

            /* This gets a semi-unique integer ID for this feature instance */
            virtual long getID() = 0;

            /* Gets the general category of the feature, if any */
            virtual FeatureFamily &getFeatureFamily() = 0;
        };

        /* Default empty destructor for otherwise abstract class */
        inline FeatureAdapterInterface::~FeatureAdapterInterface() { }
    }
}

#endif
