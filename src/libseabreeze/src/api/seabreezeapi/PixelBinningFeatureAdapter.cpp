/***************************************************//**
 * @file    PixelBinningFeatureAdapter.cpp
 * @date    October 2015
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze pixel binning instances.
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
#include "api/seabreezeapi/PixelBinningFeatureAdapter.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

using namespace seabreeze;
using namespace seabreeze::api;

PixelBinningFeatureAdapter::PixelBinningFeatureAdapter(
        PixelBinningFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<PixelBinningFeatureInterface>(intf, f, p, b, instanceIndex) {

}

PixelBinningFeatureAdapter::~PixelBinningFeatureAdapter() {

}

unsigned char PixelBinningFeatureAdapter::getPixelBinningFactor(int *errorCode) {
    
    unsigned char retval = 0;

    try {
        retval = feature->getPixelBinningFactor(*protocol, *bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}

void PixelBinningFeatureAdapter::setPixelBinningFactor(int *errorCode, const unsigned char pixelBinning) {

    try {
        feature->setPixelBinningFactor(*protocol, *bus, pixelBinning);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    } catch (IllegalArgumentException) {
        SET_ERROR_CODE(ERROR_INVALID_ERROR);
    }
}

unsigned char PixelBinningFeatureAdapter::getDefaultPixelBinningFactor(int *errorCode) {
    
    unsigned char retval = 0;

    try {
        retval = feature->getDefaultPixelBinningFactor(*protocol, *bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}

void PixelBinningFeatureAdapter::setDefaultPixelBinningFactor(int *errorCode, const unsigned char pixelBinning) {

    try {
        feature->setDefaultPixelBinningFactor(*protocol, *bus, pixelBinning);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    } catch (IllegalArgumentException) {
        SET_ERROR_CODE(ERROR_INVALID_ERROR);
    }
}

void PixelBinningFeatureAdapter::setDefaultPixelBinningFactor(int *errorCode) {

    try {
        feature->setDefaultPixelBinningFactor(*protocol, *bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    } catch (IllegalArgumentException) {
        SET_ERROR_CODE(ERROR_INVALID_ERROR);
    }
}

unsigned char PixelBinningFeatureAdapter::getMaxPixelBinningFactor(int *errorCode) {
    
    unsigned char retval = 0;

    try {
        retval = feature->getMaxPixelBinningFactor(*protocol, *bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }

    return retval;
}