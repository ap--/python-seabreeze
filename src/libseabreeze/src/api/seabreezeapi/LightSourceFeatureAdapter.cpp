/***************************************************//**
 * @file    LightSourceFeatureAdapter.cpp
 * @date    May 2013
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze LightSourceFeatureInterface instances.
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
#include "api/seabreezeapi/LightSourceFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <string>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;


#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

LightSourceFeatureAdapter::LightSourceFeatureAdapter(
        LightSourceFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<LightSourceFeatureInterface>(intf, f, p, b, instanceIndex) {

    /* Nothing else to do here, the initialization list takes care of it */
}

LightSourceFeatureAdapter::~LightSourceFeatureAdapter() {
    /* This is just a wrapper around existing instances -- nothing to delete */
}


int LightSourceFeatureAdapter::getLightSourceCount(int *errorCode) {
    SET_ERROR_CODE(ERROR_SUCCESS);
    try {
        return this->feature->getLightSourceCount(*this->protocol, *this->bus);
    } catch (const FeatureException &ex) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
}

bool LightSourceFeatureAdapter::hasLightSourceEnable(int *errorCode,
            int lightSourceIndex) {
    SET_ERROR_CODE(ERROR_SUCCESS);
    try {
        return this->feature->hasLightSourceEnable(*this->protocol, *this->bus,
                        lightSourceIndex);
    } catch (const FeatureException &ex) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return false;
    }
}


bool LightSourceFeatureAdapter::isLightSourceEnabled(int *errorCode,
            int lightSourceIndex) {
    SET_ERROR_CODE(ERROR_SUCCESS);
    try {
        return this->feature->isLightSourceEnabled(*this->protocol, *this->bus,
                        lightSourceIndex);
    } catch (const FeatureException &ex) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return false;
    }
}


void LightSourceFeatureAdapter::setLightSourceEnable(int *errorCode,
            int lightSourceIndex, bool enable) {

    try {
        this->feature->setLightSourceEnable(*this->protocol, *this->bus,
                lightSourceIndex, enable);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
}


bool LightSourceFeatureAdapter::hasVariableIntensity(int *errorCode,
            int lightSourceIndex) {
    SET_ERROR_CODE(ERROR_SUCCESS);
    try {
        return this->feature->hasVariableIntensity(*this->protocol, *this->bus,
                        lightSourceIndex);
    } catch (const FeatureException &ex) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return false;
    }
}


double LightSourceFeatureAdapter::getLightSourceIntensity(int *errorCode,
            int lightSourceIndex) {
    SET_ERROR_CODE(ERROR_SUCCESS);
    try {
        return this->feature->getLightSourceIntensity(*this->protocol,
                        *this->bus, lightSourceIndex);
    } catch (const FeatureException &ex) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0.0;
    }
}


void LightSourceFeatureAdapter::setLightSourceIntensity(int *errorCode,
                    int lightSourceIndex, double intensity) {

    try {
        this->feature->setLightSourceIntensity(*this->protocol, *this->bus,
                lightSourceIndex, intensity);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
}
