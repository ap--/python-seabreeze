/***************************************************//**
 * @file    OBPIntegrationTimeExchange.cpp
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPIntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPMessage.h"

using namespace seabreeze;
using namespace oceanBinaryProtocol;

OBPIntegrationTimeExchange::OBPIntegrationTimeExchange(
        unsigned long intTimeBase_usec) {
    this->integrationTimeBase_usec = intTimeBase_usec;

    this->messageType = OBPMessageTypes::OBP_SET_ITIME_USEC;

    this->hints->push_back(new OBPControlHint());
	this->payload.resize(sizeof(unsigned int));


    /* In case the caller does not specify an integration time, pick something
     * relatively innocuous.  100ms is fast enough that the overhead to change
     * to something else is relatively low, while still being well inside the
     * supported range.
     */
    this->integrationTime_usec = 100000;
}

OBPIntegrationTimeExchange::~OBPIntegrationTimeExchange() {

}

void OBPIntegrationTimeExchange::setIntegrationTimeMicros(
        unsigned long intTime_usec) {
    /* Scale this so that it is in the units that the given device
     * expects.
     */
    this->integrationTime_usec = intTime_usec / this->integrationTimeBase_usec;

    this->payload[0] = (unsigned char) (this->integrationTime_usec & 0x00FF);
    this->payload[1] = (unsigned char) ((this->integrationTime_usec >> 8) & 0x00FF);
    this->payload[2] = (unsigned char) ((this->integrationTime_usec >> 16) & 0x00FF);
    this->payload[3] = (unsigned char) ((this->integrationTime_usec >> 24) & 0x00FF);
}
