/***************************************************//**
 * @file    OBPTemperatureProtocol.cpp
 * @date    January 2012
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

#include "common/globals.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPTemperatureProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetTemperatureExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetAllTemperaturesExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetTemperatureCountExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPTemperatureProtocol::OBPTemperatureProtocol()
        : TemperatureProtocolInterface(new OceanBinaryProtocol()) {

}

OBPTemperatureProtocol::~OBPTemperatureProtocol() {

}


unsigned char OBPTemperatureProtocol::readTemperatureCount(const Bus &bus)
{
    int count = 0;
    vector<unsigned char> *countResult;

    OBPGetTemperatureCountExchange countExchange;

    TransferHelper *helper = bus.getHelper(countExchange.getHints());
    if(NULL == helper)
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    countResult = countExchange.queryDevice(helper);

    count = (*countResult)[0];
    delete countResult;

    return count;
}

double OBPTemperatureProtocol::readTemperature(const Bus &bus, int index)
{
    vector<unsigned char> *result = NULL;
    float temperature;
    unsigned char *bptr;
    int count = 0;
    vector<unsigned char> *countResult;

    OBPGetTemperatureExchange xchange;
    OBPGetTemperatureCountExchange countExchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper)
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    // although the number of temperatures is not needed for the query, it is nice to
    //  confirm that the index is in bounds
    countResult = countExchange.queryDevice(helper);
    if(NULL == countResult || (*countResult)[0] > 16)
    {
        /* Device is incapable of providing temperature */
        return 0;
    }

    count = (*countResult)[0];
    delete countResult;

    if ((index>=0) && (index<count)) {
        xchange.setTemperatureIndex(index);
        result = xchange.queryDevice(helper);
        if(NULL == result) {
            string error("Expected Transfer::transfer to produce a non-null result "
                "containing temperature.  Without this data, it is not possible to "
                "continue.");
            throw ProtocolException(error);
        }

        // queryDevice returns a byte stream, turn that into a float... mind our endians.
        bptr = (unsigned char *)&temperature;
        for(unsigned int j = 0; j < sizeof(float); j++) { // four bytes returned
            //printf("unsigned char %d=%x\n", j, (*result)[j]);
            bptr[j] = (*result)[j];  // get a little endian float
        }
        delete result;
    }
    else {
        string error("Bad Argument::The temperature index was out of bounds.");
        throw ProtocolException(error);
    }

    return temperature;
}


vector<double> *OBPTemperatureProtocol::readAllTemperatures(const Bus &bus) {

    vector<unsigned char> *result = NULL;
    unsigned int i;
    vector<double> *retval; // temperatures
    unsigned char *bptr;
    float temperatureBuffer;
    int count = 0;
    vector<unsigned char> *countResult;

    OBPGetAllTemperaturesExchange xchange;
    OBPGetTemperatureCountExchange countExchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    countResult = countExchange.queryDevice(helper);
    if(NULL == countResult || (*countResult)[0] > 16) {
        /* Device is incapable of providing temperature */
        return NULL;
    }

    count = (*countResult)[0];
    delete countResult;

    retval = new vector<double>(count); // temperature array to be returned
    // query device returns a generic byte array,
    // not temperature floats as defined by the actual command
    result = xchange.queryDevice(helper);
    if(NULL == result) {
        string error("Expected Transfer::transfer to produce a non-null result "
            "containing temperature.  Without this data, it is not possible to "
            "continue.");
        delete retval;
        throw ProtocolException(error);
    }
    else {

        // the bytes must be transferred to floats for the return temperatures
        for(i = 0; i < retval->size(); i++) {

            bptr = (unsigned char *)&temperatureBuffer;
            for(unsigned int j = 0; j < sizeof(float); j++) {
                bptr[j] = (*result)[j+(i*sizeof(float))];
            }

            // fill the return array with the temperatures
            (*retval)[i] = (double)temperatureBuffer;
        }
    }
    delete result;
    return retval;
}
