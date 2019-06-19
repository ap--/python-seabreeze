/***************************************************//**
 * @file    OBPRevisionProtocol.cpp
 * @date    January 2012
 * @author  Ocean Optics, Inc.
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPRevisionProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetHardwareRevisionExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetFirmwareRevisionExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPRevisionProtocol::OBPRevisionProtocol()
        : RevisionProtocolInterface(new OceanBinaryProtocol()) {

}

OBPRevisionProtocol::~OBPRevisionProtocol() {

}


unsigned char OBPRevisionProtocol::readHardwareRevision(const Bus &bus) throw (ProtocolException) 
{
    vector<byte> *result = NULL;
    unsigned char hardwareRevision;
    
    OBPGetHardwareRevisionExchange xchange;
	
    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) 
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }
    
	result = xchange.queryDevice(helper);
	if(NULL == result) 
	{
		string error("Expected Transfer::transfer to produce a non-null result "
			"containing temperature.  Without this data, it is not possible to "
			"continue.");
		throw ProtocolException(error);
	}
		
	hardwareRevision=(*result)[0]; 
	delete result;
	
	return hardwareRevision;
}

unsigned short int OBPRevisionProtocol::readFirmwareRevision(const Bus &bus) throw (ProtocolException) 
{
    vector<byte> *result = NULL;
    unsigned short int firmwareRevision;
    byte *bptr;
    
    OBPGetFirmwareRevisionExchange xchange;
	
    TransferHelper *helper = bus.getHelper(xchange.getHints());
    if(NULL == helper) 
    {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }
    
	result = xchange.queryDevice(helper);
	if(NULL == result) 
	{
		string error("Expected Transfer::transfer to produce a non-null result "
			"containing temperature.  Without this data, it is not possible to "
			"continue.");
		throw ProtocolException(error);
	}
	

	// queryDevice returns a byte stream, turn that into an unsigned int... mind our endians.
	bptr = (byte *)&firmwareRevision;
	for(unsigned int j = 0; j < sizeof(unsigned short int); j++) 
	{
		//printf("byte %d=%x\n", j, (*result)[j]);
		bptr[j] = (*result)[j];  // little endian 2-byte integer
	}

	delete result;
	
	return firmwareRevision;
}


