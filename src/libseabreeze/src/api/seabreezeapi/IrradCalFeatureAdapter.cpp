/***************************************************//**
 * @file    IrradCalFeatureAdapter.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze IrradCalFeatureInterface instances.
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
#include "api/seabreezeapi/IrradCalFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

IrradCalFeatureAdapter::IrradCalFeatureAdapter(
        IrradCalFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<IrradCalFeatureInterface>(intf, f, p, b, instanceIndex) {

    /* Nothing else to do here, the initialization list takes care of it */
}

IrradCalFeatureAdapter::~IrradCalFeatureAdapter() {
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
int IrradCalFeatureAdapter::readIrradCalibration(int *errorCode, float *buffer,
                    int bufferLength) {
    int floatsCopied = 0;

    vector<float> *cal;

    try {
        cal = this->feature->readIrradCalibration(*this->protocol, *this->bus);
        int floats = (int) cal->size();
        floatsCopied = (floats < bufferLength) ? floats : bufferLength;
        memcpy(buffer, &((*cal)[0]), floatsCopied * sizeof(float));

        delete cal;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
    return floatsCopied;
}

int IrradCalFeatureAdapter::writeIrradCalibration(int *errorCode, float *buffer,
        int bufferLength) {
    int floatsCopied = 0;

    vector<float> *floatVector = new vector<float>;
    floatVector->resize(bufferLength);
    memcpy(&((*floatVector)[0]), buffer, bufferLength * sizeof(float));

    try {
        floatsCopied = this->feature->writeIrradCalibration(
            *this->protocol, *this->bus, *floatVector);
        delete floatVector;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete floatVector;
        return 0;
    }

    return floatsCopied;
}

int IrradCalFeatureAdapter::hasIrradCollectionArea(int *errorCode) {
    int retval;

    /* hasCollectionArea() does not throw any exceptions. */
    retval = this->feature->hasCollectionArea(*this->protocol, *this->bus);
    SET_ERROR_CODE(ERROR_SUCCESS);
    return retval;
}

float IrradCalFeatureAdapter::readIrradCollectionArea(int *errorCode) {
    float area = 1.0;   /* Safe default since it removes area from calculations */

    try {
        area = (float) this->feature->readCollectionArea(*this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return area;
}

void IrradCalFeatureAdapter::writeIrradCollectionArea(int *errorCode, float area) {
    try {
        this->feature->writeCollectionArea(*this->protocol, *this->bus, area);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}
