/***************************************************//**
 * @file    OBPWaveCalProtocol.cpp
 * @date    January 2011
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPWaveCalProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetWaveCalExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPWaveCalProtocol::OBPWaveCalProtocol() : WaveCalProtocolInterface(new OceanBinaryProtocol()) {

}

OBPWaveCalProtocol::~OBPWaveCalProtocol() {

}

vector<double> *OBPWaveCalProtocol::readWavelengthCoeffs(const Bus &bus) {
    vector<unsigned char> *result = NULL;
    unsigned int i;
    vector<double> *retval;
    float coeff;
    unsigned char *bptr;

    OBPGetWaveCalExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    retval = new vector<double>(4);
    for(i = 0; i < retval->size(); i++) {
        xchange.setCoefficientIndex(i);
        result = xchange.queryDevice(helper);
        if(NULL == result) {
            string error("Expected Transfer::transfer to produce a non-null result "
                "containing wavelength coefficient.  Without this data, it is not possible to "
                "continue.");
            delete retval;
            throw ProtocolException(error);
        }

        bptr = (unsigned char *)&coeff;
        for(unsigned int j = 0; j < sizeof(float); j++) {
            bptr[j] = (*result)[j];
        }

        (*retval)[i] = coeff;  /* Only one is returned per request */

        delete result;
    }

    return retval;
}
