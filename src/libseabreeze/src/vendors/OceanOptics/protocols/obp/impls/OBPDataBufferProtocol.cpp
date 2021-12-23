/***************************************************//**
 * @file    OBPDataBufferProtocol.cpp
 * @date    October 2015
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPDataBufferProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPDataBufferClearExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPDataBufferRemoveOldestExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetDataBufferCapacityExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetFastBufferingEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetConsecutiveSampleCountExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetDataBufferElementCountExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetDataBufferMaximumCapacityExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetDataBufferCapacityExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetFastBufferingEnableExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetConsecutiveSampleCountExchange.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPDataBufferProtocol::OBPDataBufferProtocol()
        : DataBufferProtocolInterface(new OceanBinaryProtocol()) {

}

OBPDataBufferProtocol::~OBPDataBufferProtocol() {

}

void OBPDataBufferProtocol::clearBuffer(const Bus &bus,
        unsigned char bufferIndex) {

    if(0 != bufferIndex) {
        /* At present, this protocol only knows how to deal with one buffer
         * in the device.  Just do a sanity check to make sure it is zero.
         */
        string error("This protocol only supports a single buffer.  The buffer "
                     "index should be zero.");
        throw ProtocolException(error);
    }

    TransferHelper *helper;
    OBPDataBufferClearExchange exchange;

    helper = bus.getHelper(exchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    bool retval = exchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected buffer clear command.  Is it supported in this hardware?");
        throw ProtocolException(error);
    }
}

void OBPDataBufferProtocol::removeOldestSpectraFromBuffer(const Bus &bus,
        unsigned char bufferIndex, unsigned int numberOfSpectra) {

    if(0 != bufferIndex) {
        /* At present, this protocol only knows how to deal with one buffer
         * in the device.  Just do a sanity check to make sure it is zero.
         */
        string error("This protocol only supports a single buffer.  The buffer "
                     "index should be zero.");
        throw ProtocolException(error);
    }

    TransferHelper *helper;
    OBPDataBufferRemoveOldestExchange exchange;

    helper = bus.getHelper(exchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    bool retval = exchange.sendCommandToDevice(helper);

    if(false == retval) {
        string error("Device rejected buffer remove oldest command.  Is it supported in this hardware?");
        throw ProtocolException(error);
    }
}

unsigned long OBPDataBufferProtocol::getNumberOfElements(const Bus &bus,
        unsigned char bufferIndex) {

    unsigned long elementCount;
    OBPGetDataBufferElementCountExchange exchange;

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

    elementCount = exchange.queryNumberOfElements(helper);

    return elementCount;
}

unsigned long OBPDataBufferProtocol::getBufferCapacity(const Bus &bus,
        unsigned char bufferIndex) {

    unsigned long capacity;
    OBPGetDataBufferCapacityExchange exchange;

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

    capacity = exchange.queryBufferCapacity(helper);

    return capacity;
}

unsigned long OBPDataBufferProtocol::getBufferCapacityMinimum(
        const Bus &bus, unsigned char bufferIndex){

    if(0 != bufferIndex) {
        /* At present, this protocol only knows how to deal with one buffer
         * in the device.  Just do a sanity check to make sure it is zero.
         */
        string error("This protocol only supports a single buffer.  The buffer "
                     "index should be zero.");
        throw ProtocolException(error);
    }

    /* For now, the Ocean Binary Protocol does not actually define a message
     * to get the minimum capacity.  This will trivially return a constant
     * to keep things going.  In the future, it may be preferable to run an
     * actual query against the device and return a constant if it isn't
     * recognized.
     */
    return 1;
}

unsigned long OBPDataBufferProtocol::getBufferCapacityMaximum(const Bus &bus,
        unsigned char bufferIndex) {

    unsigned long maximumCapacity;
    OBPGetDataBufferMaximumCapacityExchange exchange;

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

    maximumCapacity = exchange.queryBufferMaximumCapacity(helper);

    return maximumCapacity;
}

void OBPDataBufferProtocol::setBufferCapacity(const Bus &bus,
        unsigned char bufferIndex, const unsigned long capacity) {

    if(0 != bufferIndex) {
        /* At present, this protocol only knows how to deal with one buffer
         * in the device.  Just do a sanity check to make sure it is zero.
         */
        string error("This protocol only supports a single buffer.  The buffer "
                     "index should be zero.");
        throw ProtocolException(error);
    }

    TransferHelper *helper;
    OBPSetDataBufferCapacityExchange exchange;

    helper = bus.getHelper(exchange.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    exchange.setBufferCapacity(capacity);
    exchange.sendCommandToDevice(helper);
}
