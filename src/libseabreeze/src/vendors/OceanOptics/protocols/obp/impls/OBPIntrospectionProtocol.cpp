/***************************************************//**
 * @file    OBPIntrospectionProtocol.cpp
 * @date    January 2017
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPIntrospectionProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetNumberOfPixelsExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetActivePixelRangesExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetElectricDarkPixelRangesExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetOpticalDarkPixelRangesExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;


OBPIntrospectionProtocol:: OBPIntrospectionProtocol()
    : IntrospectionProtocolInterface(new OceanBinaryProtocol()) {

}

OBPIntrospectionProtocol::~ OBPIntrospectionProtocol() {

}

unsigned short OBPIntrospectionProtocol::getNumberOfPixels(const Bus &bus)
{
	unsigned short  pixelCount = 0;
	vector<unsigned char> *countResult;

	OBPGetNumberOfPixelsExchange NumberOfPixelsExchange;

	TransferHelper *helper = bus.getHelper(NumberOfPixelsExchange.getHints());
	if (NULL == helper)
	{
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	countResult = NumberOfPixelsExchange.queryDevice(helper);
	if (countResult != 0)
	{
		// FIXME: The ocean binary protocol document states that the return value is an unsigned short,
		//  however the command returns an unsigned int.
		pixelCount = *reinterpret_cast<unsigned short *>(&(*countResult)[0]);
		delete countResult;
	}

	return pixelCount;
}

std::vector<unsigned int> *OBPIntrospectionProtocol::getActivePixelRanges(const Bus &bus)
{
	vector<unsigned char> *queryData = NULL;
	vector<unsigned int> *retval = new vector<unsigned int>(0);
	OBPGetActivePixelRangesExchange activePixelRangesExchange;

	TransferHelper *helper = bus.getHelper(activePixelRangesExchange.getHints());
	if (NULL == helper)
	{
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	queryData = activePixelRangesExchange.queryDevice(helper);
	if (NULL == queryData)
	{
		string error("Expected Transfer::transfer to produce a non-null result "
			"containing pixel pairs.  Without this data, it is not possible to "
			"continue.");
		throw ProtocolException(error);
	}
	else
	{
		// the bytes must be transferred to integers for the return pixel index pairs
		// data is little endian
		for (unsigned int i = 0; i < queryData->size(); i=i+sizeof(unsigned int))
		{
			retval->push_back(*reinterpret_cast<unsigned int *>((&(*queryData)[0] + i)));
		}
	}
	delete queryData;
	return retval;
}

std::vector<unsigned int> *OBPIntrospectionProtocol::getElectricDarkPixelRanges(const Bus &bus)
{
	vector<unsigned char> *queryData = NULL;
	vector<unsigned int> *retval = new vector<unsigned int>(0);
	OBPGetElectricDarkPixelRangesExchange electricDarkPixelRangesExchange;

	TransferHelper *helper = bus.getHelper(electricDarkPixelRangesExchange.getHints());
	if (NULL == helper)
	{
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	queryData = electricDarkPixelRangesExchange.queryDevice(helper);
	if (NULL == queryData)
	{
		string error("Expected Transfer::transfer to produce a non-null result "
			"containing pixel pairs.  Without this data, it is not possible to "
			"continue.");
		throw ProtocolException(error);
	}
	else
	{
		// the bytes must be transferred to integers for the return pixel index pairs
		// data is little endian
		for (unsigned int i = 0; i < queryData->size(); i = i + sizeof(unsigned int))
		{
			retval->push_back(*reinterpret_cast<unsigned int *>((&(*queryData)[0] + i)));
		}
	}
	delete queryData;
	return retval;
}

std::vector<unsigned int> *OBPIntrospectionProtocol::getOpticalDarkPixelRanges(const Bus &bus)
{
	vector<unsigned char> *queryData = NULL;
	vector<unsigned int> *retval = new vector<unsigned int>(0);
	OBPGetOpticalDarkPixelRangesExchange opticalDarkPixelRangesExchange;

	TransferHelper *helper = bus.getHelper(opticalDarkPixelRangesExchange.getHints());
	if (NULL == helper)
	{
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	queryData = opticalDarkPixelRangesExchange.queryDevice(helper);
	if (NULL == queryData)
	{
		string error("Expected Transfer::transfer to produce a non-null result "
			"containing pixel pairs.  Without this data, it is not possible to "
			"continue.");
		throw ProtocolException(error);
	}
	else
	{
		// the bytes must be transferred to integers for the return pixel index pairs
		// data is little endian
		for (unsigned int i = 0; i < queryData->size(); i = i + sizeof(unsigned int))
		{
			retval->push_back(*reinterpret_cast<unsigned int *>((&(*queryData)[0] + i)));
		}
	}
	delete queryData;
	return retval;
}
