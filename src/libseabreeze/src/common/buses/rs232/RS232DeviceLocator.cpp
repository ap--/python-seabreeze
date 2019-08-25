/***************************************************//**
 * @file    RS232DeviceLocator.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This class encapsulates the information needed to open
 * a device on an RS232 bus.
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
#include <stdio.h>
#include "common/buses/rs232/RS232DeviceLocator.h"
#include "common/buses/BusFamilies.h"

#ifdef _WINDOWS
#define snprintf _snprintf
#endif

using namespace seabreeze;
using namespace std;

seabreeze::RS232DeviceLocator::RS232DeviceLocator(string path, int baud) {
    this->devicePath = path;
    this->baudRate = baud;
    computeLocationHash();
}

seabreeze::RS232DeviceLocator::~RS232DeviceLocator() {

}

string &seabreeze::RS232DeviceLocator::getDevicePath() {
    return this->devicePath;
}

int seabreeze::RS232DeviceLocator::getBaudRate() {
    return this->baudRate;
}

unsigned long seabreeze::RS232DeviceLocator::getUniqueLocation() const {
    return this->locationHash;
}

void seabreeze::RS232DeviceLocator::computeLocationHash() {
    /* Iterate over the devicePath and compute a sort of hash */
    unsigned long hash = 1;
    string::iterator iter;

    /* Incorporate the baud rate as part of the value */
    hash = 2129 * hash + this->baudRate;

    for(iter = this->devicePath.begin(); iter != this->devicePath.end(); iter++) {
        /* Overflow here does not cause any problems. */
        hash = 31 * hash + (*iter);
    }

    this->locationHash = hash;
}

bool seabreeze::RS232DeviceLocator::equals(DeviceLocatorInterface &that) {
    RS232DeviceLocator *loc;

    loc = dynamic_cast<RS232DeviceLocator *>(&that);
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

string seabreeze::RS232DeviceLocator::getDescription() {
    char buffer[80];
    /* Produce a string in the following format: path@baud */
    snprintf(buffer, 79, "%s@%d", this->devicePath.c_str(), this->baudRate);
    string retval(buffer);
    return retval;
}

BusFamily seabreeze::RS232DeviceLocator::getBusFamily() const {
    RS232BusFamily family;
    return family;
}

DeviceLocatorInterface *seabreeze::RS232DeviceLocator::clone() const {
    RS232DeviceLocator *retval = new RS232DeviceLocator(
            this->devicePath, this->baudRate);

    return retval;
}
