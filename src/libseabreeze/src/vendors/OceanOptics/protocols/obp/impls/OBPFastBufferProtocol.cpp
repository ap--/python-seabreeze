/***************************************************//**
 * @file    OBPFastBufferProtocol.cpp
 * @date    February 2017
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPFastBufferProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetFastBufferingEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetFastBufferingEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetConsecutiveSampleCountExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetConsecutiveSampleCountExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPFastBufferProtocol::OBPFastBufferProtocol()
        : FastBufferProtocolInterface(new OceanBinaryProtocol()) {

}

OBPFastBufferProtocol::~OBPFastBufferProtocol() {

}

unsigned char OBPFastBufferProtocol::getBufferingEnable(const Bus &bus,
        unsigned char bufferIndex) {

    unsigned char isEnabled;
    OBPGetFastBufferingEnableExchange exchange;

    if(0 != bufferIndex) {
        /* At present, this protocol only knows how to deal with one buffer
         * in the device.  Just do a sanity check to make sure it is zero.
         */
        string error("This protocol only supports a single buffer.  The buffer "
                     "index should be zero.");
        throw ProtocolException(error);
    }

    TransferHelper *helper = bus.getHelper(exchange.getHints());
    if(NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    isEnabled = exchange.queryBufferingEnable(helper);

    return isEnabled;
}

void OBPFastBufferProtocol::setBufferingEnable(const Bus &bus,
        unsigned char bufferIndex, const unsigned char isEnabled) {

    if(0 != bufferIndex) {
        /* At present, this protocol only knows how to deal with one buffer
         * in the device.  Just do a sanity check to make sure it is zero.
         */
        string error("This protocol only supports a single buffer.  The buffer "
                     "index should be zero.");
        throw ProtocolException(error);
    }

    TransferHelper *helper;
    OBPSetFastBufferingEnableExchange exchange;

    helper = bus.getHelper(exchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    exchange.setBufferingEnable(isEnabled);
    exchange.sendCommandToDevice(helper);
}

unsigned int OBPFastBufferProtocol::getConsecutiveSampleCount(const Bus &bus,
	unsigned char bufferIndex) {

	unsigned int consecutiveSampleCount;
	OBPGetConsecutiveSampleCountExchange exchange;

	if (0 != bufferIndex) {
		/* At present, this protocol only knows how to deal with one buffer
		* in the device.  Just do a sanity check to make sure it is zero.
		*/
		string error("This protocol only supports a single buffer.  The buffer index should be zero.");
		throw ProtocolException(error);
	}

	TransferHelper *helper = bus.getHelper(exchange.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	consecutiveSampleCount = exchange.queryConsecutiveSampleCount(helper);

	return consecutiveSampleCount;
}

void OBPFastBufferProtocol::setConsecutiveSampleCount(const Bus &bus,
	unsigned char bufferIndex, const unsigned int consecutiveSampleCount) {

	if (0 != bufferIndex) {
		/* At present, this protocol only knows how to deal with one buffer
		* in the device.  Just do a sanity check to make sure it is zero.
		*/
		string error("This protocol only supports a single buffer.  The buffer index should be zero.");
		throw ProtocolException(error);
	}

	TransferHelper *helper;
	OBPSetConsecutiveSampleCountExchange exchange;

	helper = bus.getHelper(exchange.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	exchange.setConsecutiveSampleCount(consecutiveSampleCount);
	exchange.sendCommandToDevice(helper);
}
