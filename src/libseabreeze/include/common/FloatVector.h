/***************************************************//**
 * @file    FloatVector.h
 * @date    March 2010
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

#ifndef SEABREEZE_FLOATVECTOR_H
#define SEABREEZE_FLOATVECTOR_H

#include <vector>
#include "common/SeaBreeze.h"
#include "common/Data.h"

namespace seabreeze {

    class FloatVector : public Data {
    public:
        FloatVector();
        FloatVector(const std::vector<float> &that);
        virtual ~FloatVector();
        /* Dimensionality of data.  0 for scalar, 1 for vector,
         * 2 for a pair of related vectors (e.g. [X, Y] or matrix),
         * 3 for 3D, etc.
         */
        virtual int getNumberOfDimensions();

        /* Get all of the unit descriptors associated with this Data. */
        virtual std::vector<UnitDescriptor *> *getUnits();

        /* Get the data associated with this instance */
        std::vector<float> &getFloatVector();

    private:
        std::vector<float> *data;
    };

}

#endif
