/***************************************************//**
 * @file    OBPTriggerModeExchange.cpp
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPTriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;

OBPTriggerModeExchange::OBPTriggerModeExchange() {

    this->hints->push_back(new OBPControlHint());

    this->payload.resize(sizeof(unsigned char));

    /* In case the caller does not specify a trigger mode, pick something
     * relatively innocuous.  The normal mode is what the spectrometer
     * defaults to on startup.
     */
    this->payload[0] = SPECTROMETER_TRIGGER_MODE_OBP_NORMAL;

    this->messageType = OBPMessageTypes::OBP_SET_TRIG_MODE;
}

OBPTriggerModeExchange::~OBPTriggerModeExchange() {

}

void OBPTriggerModeExchange::setTriggerMode(SpectrometerTriggerMode &mode) {
    this->payload[0] = mode.getTriggerMode();
}
