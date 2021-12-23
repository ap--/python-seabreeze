/***************************************************//**
 * @file    OBPSetIPv4DefaultGatewayExchange.cpp
 * @date    March 2017
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
#include <string.h>
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetIPv4DefaultGatewayExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;

OBPSetIPv4DefaultGatewayExchange::OBPSetIPv4DefaultGatewayExchange()
{

    this->hints->push_back(new OBPControlHint());

    this->messageType = OBPMessageTypes::OBP_SET_IPV4_DEFAULT_GATEWAY;

    this->payload.resize(sizeof(unsigned char)+ 4);
}

OBPSetIPv4DefaultGatewayExchange::~OBPSetIPv4DefaultGatewayExchange()
{

}

void OBPSetIPv4DefaultGatewayExchange::setInterfaceIndex(unsigned char interfaceIndex)
{
	this->payload[0] = interfaceIndex;
}

void OBPSetIPv4DefaultGatewayExchange::setDefaultGatewayAddress(std::vector<unsigned char> IPv4_Address)
{
	for (int i = 0; i < 4; i++)
	{
		this->payload[1 + i] = IPv4_Address[i];
	}
}
