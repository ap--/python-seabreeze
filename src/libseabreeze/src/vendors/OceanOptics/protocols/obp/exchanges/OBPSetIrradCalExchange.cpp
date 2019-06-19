/***************************************************//**
 * @file    OBPSetIrradCalExchange.cpp
 * @date    February 2012
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
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetIrradCalExchange.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include "vendors/OceanOptics/protocols/obp/constants/OBPMessageTypes.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPSetIrradCalExchange::OBPSetIrradCalExchange(int numPixels) {

    this->hints->push_back(new OBPControlHint());

    this->messageType = OBPMessageTypes::OBP_SET_IRRAD_CAL_ALL;

    this->payload.resize(numPixels * sizeof(float));
}

OBPSetIrradCalExchange::~OBPSetIrradCalExchange() {

}

void OBPSetIrradCalExchange::setIrradianceCalibration(vector<float> &values) {
    unsigned int i;
    unsigned int *fptr;  /* int pointer for raw access to floating point bits */

    this->payload.resize(values.size() * sizeof(float));

    for(i = 0; i < values.size(); i++) {
        /* This explicitly copies the values into LSB, MSB order */
        fptr = (unsigned int *)&(values[i]);
        this->payload[i*sizeof(float)] = (*fptr) & 0x00FF;
        this->payload[i*sizeof(float) + 1] = ((*fptr) >> 8) & 0x00FF;
        this->payload[i*sizeof(float) + 2] = ((*fptr) >> 16) & 0x00FF;
        this->payload[i*sizeof(float) + 3] = ((*fptr) >> 24) & 0x00FF;
    }
}
