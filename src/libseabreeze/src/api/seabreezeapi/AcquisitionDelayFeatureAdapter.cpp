/***************************************************//**
 * @file    AcquisitionDelayFeatureAdapter.cpp
 * @date    November 2015
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze AcquisitionDelayFeatureInterface instances.
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

#include "common/globals.h"
#include "api/seabreezeapi/AcquisitionDelayFeatureAdapter.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

using namespace seabreeze;
using namespace seabreeze::api;

AcquisitionDelayFeatureAdapter::AcquisitionDelayFeatureAdapter(AcquisitionDelayFeatureInterface *intf,
            const FeatureFamily &f, Protocol *p, Bus *b,
            unsigned short instanceIndex)
        : FeatureAdapterTemplate<AcquisitionDelayFeatureInterface>(intf, f, p, b, instanceIndex) {

}


AcquisitionDelayFeatureAdapter::~AcquisitionDelayFeatureAdapter() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
unsigned long AcquisitionDelayFeatureAdapter::getAcquisitionDelayIncrementMicroseconds(
        int *errorCode) {

    unsigned long retval = 0;

    try {
        retval = this->feature->getAcquisitionDelayIncrementMicroseconds(*this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}

unsigned long AcquisitionDelayFeatureAdapter::getAcquisitionDelayMaximumMicroseconds(
        int *errorCode) {

    unsigned long retval = 0;

    try {
        retval = this->feature->getAcquisitionDelayMaximumMicroseconds(*this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}

unsigned long AcquisitionDelayFeatureAdapter::getAcquisitionDelayMinimumMicroseconds(
        int *errorCode) {

    unsigned long retval = 0;

    try {
        retval = this->feature->getAcquisitionDelayMinimumMicroseconds(*this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}

unsigned long AcquisitionDelayFeatureAdapter::getAcquisitionDelayMicroseconds(
        int *errorCode) {

    unsigned long retval = 0;

    try {
        retval = this->feature->getAcquisitionDelayMicroseconds(*this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}

void AcquisitionDelayFeatureAdapter::setAcquisitionDelayMicroseconds(int *errorCode,
        const unsigned long delay_usec) {

    try {
        this->feature->setAcquisitionDelayMicroseconds(*this->protocol,
            *this->bus, delay_usec);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

