/***************************************************//**
 * @file    OBPSetAcquisitionDelayExchange.cpp
 * @date    November 2015
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetAcquisitionDelayExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"
#include <vector>

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPSetAcquisitionDelayExchange::OBPSetAcquisitionDelayExchange() {
    this->hints->push_back(new OBPControlHint());

    this->messageType = OBPMessageTypes::OBP_SET_TRIG_DELAY_USEC;

	this->payload.resize(sizeof(unsigned int));
}

OBPSetAcquisitionDelayExchange::~OBPSetAcquisitionDelayExchange() {

}

void OBPSetAcquisitionDelayExchange::setAcquisitionDelayMicros(
        unsigned long delayMicros) {

    unsigned char *cptr;
    int i;

    cptr = (unsigned char *)&delayMicros;

    for(i = 0; i < 4; i++) {
        this->payload[i] = cptr[i];
    }
}
