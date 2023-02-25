/***************************************************//**
 * @file    OBPIrradCalProtocol.cpp
 * @date    February 2012
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
#include <math.h>

#include "vendors/OceanOptics/protocols/obp/impls/OBPIrradCalProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetIrradCalExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetIrradCalExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetIrradCollectionAreaExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetIrradCollectionAreaExchange.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPIrradCalProtocol::OBPIrradCalProtocol(int maximumNumberOfPixels)
        : IrradCalProtocolInterface(new OceanBinaryProtocol()) {

    this->pixelCountMaximum = maximumNumberOfPixels;
}


OBPIrradCalProtocol::~OBPIrradCalProtocol() {

}

vector<float> *OBPIrradCalProtocol::readIrradCal(const Bus &bus) {
    TransferHelper *helper;
    OBPGetIrradCalExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result "
            "containing calibration data.  Without this data, it is not possible to "
            "generate a calibration array.");
        throw ProtocolException(error);
    }

    vector<unsigned char> result = *raw;

    vector<float> *retval = new vector<float>;

    for(unsigned int i = 0; i < result.size(); i += 4) {
        float value;
        unsigned int *fptr = (unsigned int *)&value;
        *fptr =   ((result[i] & 0x00FF))
                | ((result[i + 1] & 0x00FF) << 8)
                | ((result[i + 2] & 0x00FF) << 16)
                | ((result[i + 3] & 0x00FF) << 24);
        retval->push_back(value);
    }

    delete raw;

    return retval;
}


int OBPIrradCalProtocol::writeIrradCal(const Bus &bus, const vector<float> &cal) {
    TransferHelper *helper;
    OBPSetIrradCalExchange command(this->pixelCountMaximum);

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* factors is probably a reference to what was passed in by the caller,
     * so make a copy and truncate it to the maximum size.
     */
    vector<float> bounded(cal);

    if(cal.size() > this->pixelCountMaximum) {
        bounded.resize(this->pixelCountMaximum);
    }

    command.setIrradianceCalibration(bounded);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
    return (int) bounded.size();
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
int OBPIrradCalProtocol::hasCollectionArea(const Bus &bus) {
    try {
        readCollectionArea(bus);
    } catch (const ProtocolException &pe) {
        return 0;
    }
    return 1;
}

float OBPIrradCalProtocol::readCollectionArea(const Bus &bus) {
    TransferHelper *helper;
    OBPGetIrradCollectionAreaExchange request;
    float retval = 1.0;
    unsigned int *iptr;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result "
            "containing calibration data.  Without this data, it is not possible to "
            "generate a calibration array.");
        throw ProtocolException(error);
    }

    if(raw->size() < sizeof(float)) {
        string error("Failed to get back expected number of bytes that should"
            " have held collection area.");
        delete raw;
        throw ProtocolException(error);
    }

    /* Explicitly copy this in LSB, MSB order */
    iptr = (unsigned int *)&retval;
    *iptr = ((*raw)[0] & 0x00FF)
            | (((*raw)[1] & 0x00FF) << 8)
            | (((*raw)[2] & 0x00FF) << 16)
            | (((*raw)[3] & 0x00FF) << 24);

    delete raw;

    return retval;
}


void OBPIrradCalProtocol::writeCollectionArea(const Bus &bus, float area) {
    TransferHelper *helper;
    OBPSetIrradCollectionAreaExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    command.setCollectionArea(area);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
}
