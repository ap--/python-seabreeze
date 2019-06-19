/***************************************************//**
 * @file    TECQEEnableExchange.cpp
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This contains the particular implementation for the
 * TEC found on the QE65000 and related spectrometers,
 * such as the NIRQuest.
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
#include "vendors/OceanOptics/protocols/ooi/exchanges/TECQEEnableExchange.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;

TECQEEnableExchange::TECQEEnableExchange() {

    this->hints->push_back(new ControlHint());
    this->buffer->resize(3);
    this->length = 3;
    this->direction = Transfer::TO_DEVICE;

    /* Initialize with the protocol command number and otherwise zero */
    (*(this->buffer))[0] = OpCodes::OP_TECENABLE_QE;
    (*(this->buffer))[1] = 0x00;
    (*(this->buffer))[2] = 0x00;

    /* If this is used without a mode being specified, disable the TEC. */
    this->tecEnable = false;
}

TECQEEnableExchange::~TECQEEnableExchange() {

}

void TECQEEnableExchange::setEnable(bool enable) {
    this->tecEnable = enable;
}

Data *TECQEEnableExchange::transfer(TransferHelper *helper)
throw (ProtocolException) {

    (*(this->buffer))[1] = (false == this->tecEnable) ? 0x00 : 0x01;

    /* Now delegate to the superclass to move the buffer. */
    /* This transfer() may cause a ProtocolException to be thrown. */
    return Transfer::transfer(helper);
}
