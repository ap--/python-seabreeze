/***************************************************//**
 * @file    OOIRS232Interface.cpp
 * @date    April 2011
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
#include "common/buses/rs232/RS232TransferHelper.h"
#include "common/buses/rs232/RS232DeviceLocator.h"
#include "common/exceptions/IllegalArgumentException.h"
#include "vendors/OceanOptics/buses/rs232/OOIRS232Interface.h"

using namespace seabreeze;
using namespace std;

seabreeze::OOIRS232Interface::OOIRS232Interface() {
    this->rs232 = NULL;
}

seabreeze::OOIRS232Interface::~OOIRS232Interface() {
    if(NULL != this->rs232) {
        if(true == this->rs232->isOpened()) {
            this->rs232->close();
        }
        delete this->rs232;
    }
}

bool seabreeze::OOIRS232Interface::open() {
    if(NULL == this->deviceLocator || NULL == this->rs232) {
        return false;
    }

    bool flag = false;
    flag = this->rs232->open();
    this->rs232Helper = new RS232TransferHelper(this->rs232);
    return flag;
}

void seabreeze::OOIRS232Interface::close() {
    if(NULL == this->rs232) {
        return;
    }
    this->rs232->close();
}

TransferHelper *seabreeze::OOIRS232Interface::getHelper(const vector<ProtocolHint *> &hints) const {
    /* RS232 only really needs a single helper. */
    return this->rs232Helper;
}

void seabreeze::OOIRS232Interface::setLocation(const DeviceLocatorInterface &location) throw (IllegalArgumentException) {
    RS232DeviceLocator *rs232loc = NULL;
    DeviceLocatorInterface *dli;

    BusFamily family = location.getBusFamily();
    if(false == family.equals(this->getBusFamily())) {
        string cause("Invalid bus on device locator.");
        throw IllegalArgumentException(cause);
    }

    /* Cloning this helps to get around const limitations with dynamic_cast */
    dli = location.clone();
    rs232loc = dynamic_cast<RS232DeviceLocator *>(dli);

    if(NULL == rs232loc) {
        string cause("Invalid device locator for RS232 interface.");
        throw IllegalArgumentException(cause);
    }

    if(NULL != this->rs232) {
        delete this->rs232;
    }

    /* This may also throw an IllegalArgumentException */
    this->rs232 = new RS232(rs232loc->getDevicePath().c_str(), rs232loc->getBaudRate());

    delete dli;

    /* Delegate to the base class to copy the location instance */
    RS232Interface::setLocation(location);
}

