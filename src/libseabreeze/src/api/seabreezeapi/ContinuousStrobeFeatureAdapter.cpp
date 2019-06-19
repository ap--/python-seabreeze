/***************************************************//**
 * @file    ContinuousStrobeFeatureAdapter.cpp
 * @date    October 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows access to SeaBreeze
 * ContinuousStrobeFeatureInterface instances.
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

#include "common/globals.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/seabreezeapi/ContinuousStrobeFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <string>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

ContinuousStrobeFeatureAdapter::ContinuousStrobeFeatureAdapter(
        ContinuousStrobeFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<ContinuousStrobeFeatureInterface>(intf, f, p, b, instanceIndex) {
}

ContinuousStrobeFeatureAdapter::~ContinuousStrobeFeatureAdapter() {
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
void ContinuousStrobeFeatureAdapter::setContinuousStrobePeriodMicroseconds(int *errorCode,
        unsigned long period_usec) {

    try {
        this->feature->setContinuousStrobePeriodMicroseconds(*this->protocol, *this->bus, this->index, period_usec);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
}

void ContinuousStrobeFeatureAdapter::setContinuousStrobeEnable(int *errorCode,
        bool enable) {

    try {
        this->feature->setContinuousStrobeEnable(*this->protocol, *this->bus, this->index, enable);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
}
