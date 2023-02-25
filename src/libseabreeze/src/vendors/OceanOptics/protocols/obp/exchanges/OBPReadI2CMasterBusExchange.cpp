/***************************************************//**
 * @file    OBPReadI2CMasterBusExchange.cpp
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPReadI2CMasterBusExchange.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include <string.h>

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;

OBPReadI2CMasterBusExchange::OBPReadI2CMasterBusExchange() {
    this->messageType = OBPMessageTypes::OBP_READ_12C_MASTER_BUS;

    this->hints->push_back(new OBPControlHint());
	this->payload.resize(sizeof(unsigned char));
	this->payload[sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned short)];
}

void OBPReadI2CMasterBusExchange::setBusIndex(unsigned char busIndex)
{
	this->payload[0] = busIndex;
}

void OBPReadI2CMasterBusExchange::setSlaveAddress(unsigned char slaveAddress)
{
	this->payload[1] = slaveAddress;
}

void OBPReadI2CMasterBusExchange::setNumberOfBytes(unsigned short numberOfBytes)
{
	memcpy(&(this->payload[2]), &numberOfBytes, sizeof(unsigned short));
}

OBPReadI2CMasterBusExchange::~OBPReadI2CMasterBusExchange() {

}
