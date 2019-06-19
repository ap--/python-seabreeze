/***************************************************//**
 * @file    OBPSpectrumProcessingProtocol.cpp
 * @date    February 2015
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPSpectrumProcessingProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetScansToAverageExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetScansToAverageExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetBoxcarWidthExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetBoxcarWidthExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPSpectrumProcessingProtocol::OBPSpectrumProcessingProtocol()
        : SpectrumProcessingProtocolInterface(new OceanBinaryProtocol()) {

}

OBPSpectrumProcessingProtocol::~OBPSpectrumProcessingProtocol() {

}


unsigned short int OBPSpectrumProcessingProtocol::readSpectrumProcessingScansToAverage(const Bus &bus)
                throw (ProtocolException) 
{
    vector<byte> *result = NULL;
    unsigned short int scansToAverage;
    byte *bptr;
        
    OBPGetScansToAverageExchange xchange;
    
    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) 
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }
    
    result = xchange.queryDevice(helper);
    if(NULL == result) {
        string error("Expected queryDevice to produce a non-null result "
            "containing a spectrum protocol scans to average.  Without this data, it is not possible to "
            "continue.");
        throw ProtocolException(error);
    }
    
    // queryDevice returns a byte stream, turn that into a float... mind our endians.
    bptr = (byte *)&scansToAverage;
    for(unsigned int j = 0; j < sizeof(unsigned short int); j++) {
        bptr[j] = (*result)[j];
    }

    delete result;

    return scansToAverage;
}

void OBPSpectrumProcessingProtocol::writeSpectrumProcessingScansToAverage(
            const Bus &bus, unsigned short int scansToAverage) throw (ProtocolException) {

    OBPSetScansToAverageExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setScansToAverage(scansToAverage);

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the setpoint.  Was it out of bounds?");
        throw ProtocolException(error);
    }
}

unsigned char OBPSpectrumProcessingProtocol::readSpectrumProcessingBoxcarWidth(const Bus &bus) throw (ProtocolException) 
{
    vector<byte> *result = NULL;
    unsigned char boxcarWidth;
    
    OBPGetBoxcarWidthExchange xchange;
    
    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) 
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }
    
    result = xchange.queryDevice(helper);
    if(NULL == result) {
        string error("Expected queryDevice to produce a non-null result "
            "containing a spectrum processing boxcar filter width.  Without this data, it is not possible to "
            "continue.");
        throw ProtocolException(error);
    }
        
    boxcarWidth=(*result)[0]; 
    delete result;
    
    return boxcarWidth;
}


void OBPSpectrumProcessingProtocol::writeSpectrumProcessingBoxcarWidth(
            const Bus &bus, unsigned char boxcarWidth) throw (ProtocolException) {

    OBPSetBoxcarWidthExchange xchange;

    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    xchange.setBoxcarWidth(boxcarWidth);

    /* This may cause a ProtocolException to be thrown. */
    bool retval = xchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected the setpoint.  Was it out of bounds?");
        throw ProtocolException(error);
    }
}

