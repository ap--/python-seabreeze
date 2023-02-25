/***************************************************//**
 * @file    OpticalBenchFeatureAdapter.cpp
 * @date    January 2015
 * @author  Ocean Optics, Inc., Kirk Clendinning, Heliospectra
 *
 * This is a wrapper that allows access to SeaBreeze
 * OpticalBenchFeatureInterface instances.
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
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/seabreezeapi/OpticalBenchFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

OpticalBenchFeatureAdapter::OpticalBenchFeatureAdapter(
        OpticalBenchFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<OpticalBenchFeatureInterface>(intf, f, p, b, instanceIndex) {

    /* Nothing else to do here, the initialization list takes care of it */
}

OpticalBenchFeatureAdapter::~OpticalBenchFeatureAdapter() {
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif


unsigned short int OpticalBenchFeatureAdapter::readOpticalBenchFiberDiameterMicrons(int *errorCode) {
	unsigned short int returnValue;

	// no memory allocated, just pass it through
	try {
    	returnValue=this->feature->readOpticalBenchFiberDiameterMicrons(*this->protocol, *this->bus);
    	SET_ERROR_CODE(ERROR_SUCCESS);
    }
    catch (FeatureException &fe) {
    	returnValue=0;
    	SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return returnValue;
}

unsigned short int OpticalBenchFeatureAdapter::readOpticalBenchSlitWidthMicrons(int *errorCode) {
	unsigned short int returnValue;

	// no memory allocated, just pass it through
	try {
    	returnValue=this->feature->readOpticalBenchSlitWidthMicrons(*this->protocol, *this->bus);
    	SET_ERROR_CODE(ERROR_SUCCESS);
    }
    catch (FeatureException &fe) {
    	returnValue=0;
    	SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return returnValue;
}


// This method frees the memory allocated by readOpticalBenchSerialNumber so the calling
//  method doesn't have to. Nice way to prevent memory leaks.
int OpticalBenchFeatureAdapter::readOpticalBenchID(int *errorCode,
            char *buffer, int buffer_length) {

    string *id;
    try {
        id = this->feature->readOpticalBenchID(
            *this->protocol, *this->bus);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);

        return 0;
    }

    if(NULL == id) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    memset(buffer, (int)0, buffer_length);

    string::iterator iter;
    int i = 0;
    for(iter = id->begin(); iter != id->end() && i < buffer_length; iter++, i++) {
        buffer[i] = *iter;
    }
    delete id;

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}

// frees memory allocated by the feature method

int OpticalBenchFeatureAdapter::readOpticalBenchSerialNumber(int *errorCode,
            char *buffer, int buffer_length) {

    string *serialNumber;

    try {
        serialNumber = this->feature->readOpticalBenchSerialNumber(
            *this->protocol, *this->bus);
    } catch (FeatureException &fe) {
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


// frees memory allocated by the feature method
int OpticalBenchFeatureAdapter::readOpticalBenchCoating(int *errorCode,
            char *buffer, int buffer_length) {

    string *coating;

    try {
        coating = this->feature->readOpticalBenchCoating(
            *this->protocol, *this->bus);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    if(NULL == coating) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    memset(buffer, (int)0, buffer_length);

    string::iterator iter;
    int i = 0;
    for(iter = coating->begin(); iter != coating->end() && i < buffer_length; iter++, i++) {
        buffer[i] = *iter;
    }
    delete coating;

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}

// frees memory allocated by the feature method
int OpticalBenchFeatureAdapter::readOpticalBenchFilter(int *errorCode,
            char *buffer, int buffer_length) {

    string *filter;

    try {
        filter = this->feature->readOpticalBenchFilter(
            *this->protocol, *this->bus);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    if(NULL == filter) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    memset(buffer, (int)0, buffer_length);

    string::iterator iter;
    int i = 0;
    for(iter = filter->begin(); iter != filter->end() && i < buffer_length; iter++, i++) {
        buffer[i] = *iter;
    }
    delete filter;

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}

// frees memory allocated by the feature method
int OpticalBenchFeatureAdapter::readOpticalBenchGrating(int *errorCode,
            char *buffer, int buffer_length) {

    string *grating;

    try {
        grating = this->feature->readOpticalBenchGrating(
            *this->protocol, *this->bus);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    if(NULL == grating) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    memset(buffer, (int)0, buffer_length);

    string::iterator iter;
    int i = 0;
    for(iter = grating->begin(); iter != grating->end() && i < buffer_length; iter++, i++) {
        buffer[i] = *iter;
    }
    delete grating;

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}
