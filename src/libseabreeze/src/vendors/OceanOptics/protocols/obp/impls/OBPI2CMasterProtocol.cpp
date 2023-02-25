/***************************************************//**
 * @file    OBPI2CMasterProtocol.cpp
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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

#include "vendors/OceanOptics/protocols/obp/impls/OBPI2CMasterProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetI2CMasterNumberOfBusesExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadI2CMasterBusExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPWriteI2CMasterBusExchange.h"

#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPI2CMasterProtocol::OBPI2CMasterProtocol()
        : I2CMasterProtocolInterface(new OceanBinaryProtocol())
{

}


OBPI2CMasterProtocol::~OBPI2CMasterProtocol()
{

}


#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char OBPI2CMasterProtocol::i2cMasterGetNumberOfBuses(const Bus &bus)
{
    TransferHelper *helper;
    OBPGetI2CMasterNumberOfBusesExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected to produce a non-null result "
            "containing the number of i2c buses.  Without this data, it is not possible to continue.");
        throw ProtocolException(error);
    }

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned char result = (*raw)[0];

    delete raw;

    return result;
}


std::vector<unsigned char> OBPI2CMasterProtocol::i2cMasterReadBus(const Bus &bus, unsigned char busIndex, unsigned char slaveAddress, unsigned short numberOfBytes)
{
	TransferHelper *helper;
	OBPReadI2CMasterBusExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	request.setBusIndex(busIndex);
	request.setSlaveAddress(slaveAddress);
	request.setNumberOfBytes(numberOfBytes);

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result "
			"containing calibration data.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	vector<unsigned char> result = *raw;

	delete raw;

	return result;
}

unsigned short OBPI2CMasterProtocol::i2cMasterWriteBus(const Bus &bus, unsigned char busIndex, unsigned char slaveAddress, const std::vector<unsigned char> writeData)
{
	TransferHelper *helper;
	OBPWriteI2CMasterBusExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	request.setBusIndex(busIndex);
	request.setSlaveAddress(slaveAddress);
	request.dataToWrite(writeData);

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected to produce a non-null result "
			"containing the number of i2c buses.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned short result = (*raw)[0];

	delete raw;

	return result;
}
