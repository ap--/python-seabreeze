/***************************************************//**
 * @file    SerialNumberFeatureAdapter.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows access to SeaBreeze
 * SerialNumberFeatureInterface instances.
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
#include "api/seabreezeapi/SerialNumberFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <string>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

SerialNumberFeatureAdapter::SerialNumberFeatureAdapter(
        SerialNumberFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<SerialNumberFeatureInterface>(intf, f, p, b, instanceIndex) {

    /* Nothing else to do here, the initialization list takes care of it */
}

SerialNumberFeatureAdapter::~SerialNumberFeatureAdapter() {
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

int SerialNumberFeatureAdapter::getSerialNumber(int *errorCode,
            char *buffer, int buffer_length) {

    string *serialNumber;

    try {
        serialNumber = this->feature->readSerialNumber(
            *this->protocol, *this->bus);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    if(NULL == serialNumber) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    memset(buffer, (int)0, buffer_length);

    string::iterator iter;
    int i = 0;
    for(iter = serialNumber->begin(); iter != serialNumber->end() && i < buffer_length; iter++, i++) {
        buffer[i] = *iter;
    }
    delete serialNumber;

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}

unsigned char SerialNumberFeatureAdapter::getSerialNumberMaximumLength(int *errorCode) {

    unsigned char length;

    try {
        length = this->feature->readSerialNumberMaximumLength(
            *this->protocol, *this->bus);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return length;
}
