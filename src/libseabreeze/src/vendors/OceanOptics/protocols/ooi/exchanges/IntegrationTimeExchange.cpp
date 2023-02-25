/***************************************************//**
 * @file    IntegrationTimeExchange.cpp
 * @date    February 2009
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
#include "vendors/OceanOptics/protocols/ooi/exchanges/IntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;

IntegrationTimeExchange::IntegrationTimeExchange(
        unsigned long intTimeBase_usec) {
    this->integrationTimeBase_usec = intTimeBase_usec;

    this->hints->push_back(new ControlHint());
    this->buffer->resize(5);
    this->length = 5;
    this->direction = Transfer::TO_DEVICE;
    checkBufferSize();

    /* In case the caller does not specify an integration time, pick something
     * relatively innocuous.  100ms is fast enough that the overhead to change
     * to something else is relatively low (including the stability scan)
     * but high enough that most if not all devices should accept this value.
     */
    this->integrationTime_usec = 100000;
}

IntegrationTimeExchange::~IntegrationTimeExchange() {

}

void IntegrationTimeExchange::setIntegrationTimeMicros(
        unsigned long intTime_usec) {
    /* Scale this so that it is in the units that the given device
     * expects.  Some (HR2000) expect milliseconds, others (HR2000+) expect microseconds.
     * Most likely, after this->integrationTime_usec is set, it will be transferred over the
     * bus, so this just gets everything primed for that transfer.
     */
    this->integrationTime_usec = intTime_usec / this->integrationTimeBase_usec;
}

Data *IntegrationTimeExchange::transfer(TransferHelper *helper) {
    /* Note that it might be cleaner to populate the buffer when the integration
     * time is first written in, but this guarantees that it only happens once
     * per transfer.
     */

    /* Start with the protocol command number */
    (*(this->buffer))[0] = OpCodes::OP_ITIME;

    /* Then fill in the integration time, 4 bytes, LSB first. */
    (*(this->buffer))[1] = (unsigned char)(this->integrationTime_usec & 0x00000000FF);
    (*(this->buffer))[2] = (unsigned char)((this->integrationTime_usec & 0x000000FF00) >> 8);
    (*(this->buffer))[3] = (unsigned char)((this->integrationTime_usec & 0x0000FF0000) >> 16);
    (*(this->buffer))[4] = (unsigned char)((this->integrationTime_usec & 0x00FF000000) >> 24);

    /* Now delegate to the superclass to move the buffer. */
    return Transfer::transfer(helper);
}
