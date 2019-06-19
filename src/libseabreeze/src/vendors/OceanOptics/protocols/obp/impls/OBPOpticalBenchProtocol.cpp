/***************************************************//**
 * @file    OBPOpticalBenchProtocol.cpp
 * @date    January 2015
 * @author  Ocean Optics, Inc., Kirk Clendinning, Heliospectra
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPOpticalBenchProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchCoatingExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchGratingExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchFilterExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchIDExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchSerialNumberExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchFiberDiameterMicronsExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalBenchSlitWidthMicronsExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"
#include "common/exceptions/ProtocolTransactionException.h"

#include <stdlib.h>

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPOpticalBenchProtocol::OBPOpticalBenchProtocol()
        : OpticalBenchProtocolInterface(new OceanBinaryProtocol()) {

}

OBPOpticalBenchProtocol::~OBPOpticalBenchProtocol() {

}


string *OBPOpticalBenchProtocol::readOpticalBenchID(const Bus &bus)
                throw (ProtocolException) {

    vector<byte> *result;
    string *retval = NULL;

    OBPGetOpticalBenchIDExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>('\0');
    }
    
    retval = new string();
    vector<byte>::iterator iter;
    /* This is probably not the most efficient way to copy
     * from a vector of bytes into a string, but at least
     * this way issues of string encoding should be
     * avoided (i.e. the sizeof a string element is not
     * assumed here).  Since this function will not be called
     * continuously nor is the serial number ever very long,
     * this should suffice.
     */
    for(iter = result->begin(); iter != result->end(); iter++) {
        retval->push_back((char) * iter);
        if('\0' == *iter) {
            break;
        }
    }

    delete result;
    return retval;
}

string *OBPOpticalBenchProtocol::readOpticalBenchSerialNumber(const Bus &bus)
                throw (ProtocolException) {

    vector<byte> *result;
    string *retval = NULL;

    OBPGetOpticalBenchSerialNumberExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>('\0');
    }

    retval = new string();
    vector<byte>::iterator iter;
    /* This is probably not the most efficient way to copy
     * from a vector of bytes into a string, but at least
     * this way issues of string encoding should be
     * avoided (i.e. the sizeof a string element is not
     * assumed here).  Since this function will not be called
     * continuously nor is the serial number ever very long,
     * this should suffice.
     */
    for(iter = result->begin(); iter != result->end(); iter++) {
        retval->push_back((char) * iter);
        if('\0' == *iter) {
            break;
        }
    }

    delete result;

    return retval;
}

string *OBPOpticalBenchProtocol::readOpticalBenchCoating(const Bus &bus)
                throw (ProtocolException) {

    vector<byte> *result;
    string *retval = NULL;

    OBPGetOpticalBenchCoatingExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>('\0');
    }

    retval = new string();
    vector<byte>::iterator iter;
    /* This is probably not the most efficient way to copy
     * from a vector of bytes into a string, but at least
     * this way issues of string encoding should be
     * avoided (i.e. the sizeof a string element is not
     * assumed here).  Since this function will not be called
     * continuously nor is the serial number ever very long,
     * this should suffice.
     */
    for(iter = result->begin(); iter != result->end(); iter++) {
        retval->push_back((char) * iter);
        if('\0' == *iter) {
            break;
        }
    }

    delete result;

    return retval;
}

string *OBPOpticalBenchProtocol::readOpticalBenchFilter(const Bus &bus)
                throw (ProtocolException) {

    vector<byte> *result;
    string *retval = NULL;

    OBPGetOpticalBenchFilterExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>('\0');
    }

    retval = new string();
    vector<byte>::iterator iter;
    /* This is probably not the most efficient way to copy
     * from a vector of bytes into a string, but at least
     * this way issues of string encoding should be
     * avoided (i.e. the sizeof a string element is not
     * assumed here).  Since this function will not be called
     * continuously nor is the serial number ever very long,
     * this should suffice.
     */
    for(iter = result->begin(); iter != result->end(); iter++) {
        retval->push_back((char) * iter);
        if('\0' == *iter) {
            break;
        }
    }

    delete result;

    return retval;
}

string *OBPOpticalBenchProtocol::readOpticalBenchGrating(const Bus &bus)
                throw (ProtocolException) {

    vector<byte> *result;
    string *retval = NULL;

    OBPGetOpticalBenchGratingExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>('\0');
    }

    retval = new string();
    vector<byte>::iterator iter;
    /* This is probably not the most efficient way to copy
     * from a vector of bytes into a string, but at least
     * this way issues of string encoding should be
     * avoided (i.e. the sizeof a string element is not
     * assumed here).  Since this function will not be called
     * continuously nor is the serial number ever very long,
     * this should suffice.
     */
    for(iter = result->begin(); iter != result->end(); iter++) {
        retval->push_back((char) * iter);
        if('\0' == *iter) {
            break;
        }
    }

    delete result;

    return retval;
}


unsigned short int OBPOpticalBenchProtocol::readOpticalBenchSlitWidthMicrons(const Bus &bus)
                throw (ProtocolException) 
{
    vector<byte> *result = NULL;
    unsigned short int slitWidth;
    byte *bptr;
        
    OBPGetOpticalBenchSlitWidthMicronsExchange xchange;
    
    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) 
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }
    
    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>((int)0);
        result->resize(sizeof(unsigned short int));
    }
    
    // queryDevice returns a byte stream, turn that into a float... mind our endians.
    bptr = (byte *)&slitWidth;
    for(unsigned int j = 0; j < sizeof(unsigned short int); j++) {
        bptr[j] = (*result)[j];
    }

    delete result;

    return slitWidth;
}

unsigned short int OBPOpticalBenchProtocol::readOpticalBenchFiberDiameterMicrons(const Bus &bus)
                throw (ProtocolException) 
{
    vector<byte> *result = NULL;
    unsigned short int fiberDiameter;
    byte *bptr;
        
    OBPGetOpticalBenchFiberDiameterMicronsExchange xchange;
    
    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) 
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }
    
    result = xchange.queryDevice(helper);
    // some spectromters return null strings to unsupported values
    //  the STS does this. Put an empty c string in result
    if (result==NULL) {
        result = new vector<byte>((int)0);
        result->resize(sizeof(unsigned short int));
    }
    
    // queryDevice returns a byte stream, turn that into a float... mind our endians.
    bptr = (byte *)&fiberDiameter;
    for(unsigned int j = 0; j < sizeof(unsigned short int); j++) {
        bptr[j] = (*result)[j];
    }
    delete result;
        
    return fiberDiameter;
}

