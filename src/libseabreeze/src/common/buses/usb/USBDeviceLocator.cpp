/***************************************************//**
 * @file    USBDeviceLocator.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This class encapsulates the information needed to open
 * a device on an USB bus.
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
#include "common/buses/usb/USBDeviceLocator.h"
#include "common/buses/BusFamilies.h"
#include <stdio.h>

#ifdef WINDOWS
#define snprintf _snprintf
#endif

using namespace seabreeze;
using namespace std;

USBDeviceLocator::USBDeviceLocator(unsigned long id) {
    this->deviceID = id;
}

USBDeviceLocator::~USBDeviceLocator() {

}

unsigned long USBDeviceLocator::getUniqueLocation() const {
    return this->deviceID;
}

bool USBDeviceLocator::equals(DeviceLocatorInterface &that) {
    USBDeviceLocator *loc;

    loc = dynamic_cast<USBDeviceLocator *>(&that);
    if(NULL == loc) {
        return false;
    }

    if(loc->getUniqueLocation() != this->getUniqueLocation()) {
        return false;
    }

    BusFamily thisBusFamily = this->getBusFamily();
    BusFamily thatBusFamily = loc->getBusFamily();
    if(false == thisBusFamily.equals(thatBusFamily)) {
        return false;
    }

    return true;
}

string USBDeviceLocator::getDescription() {
    char buffer[80];
    /* Produce a string in the following format: path@baud */
    snprintf(buffer, 79, "USB:0x%02lX", this->deviceID);
    string retval(buffer);
    return retval;
}

BusFamily USBDeviceLocator::getBusFamily() const {
    USBBusFamily family;
    return family;
}

DeviceLocatorInterface *USBDeviceLocator::clone() const {
    USBDeviceLocator *retval = new USBDeviceLocator(
            this->deviceID);

    return retval;
}
