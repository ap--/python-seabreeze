/***************************************************//**
 * @file    TCPIPv4SocketBus.cpp
 * @date    February 2016
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2016, Ocean Optics Inc
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

#include "common/buses/network/TCPIPv4SocketBus.h"
#include "common/buses/BusFamilies.h"
#include <cstddef>

using namespace seabreeze;
using namespace std;

TCPIPv4SocketBus::TCPIPv4SocketBus() {
    this->deviceLocator = NULL;
}

TCPIPv4SocketBus::~TCPIPv4SocketBus() {
    if(NULL != this->deviceLocator) {
        delete this->deviceLocator;
    }

    clearHelpers();
}

Socket *TCPIPv4SocketBus::getSocketDescriptor() {
    return this->socket;
}

BusFamily TCPIPv4SocketBus::getBusFamily() const {
    TCPIPv4BusFamily family;
    return family;
}

void TCPIPv4SocketBus::setLocation(
        const DeviceLocatorInterface &location) throw (IllegalArgumentException) {
    if(NULL != this->deviceLocator) {
        delete this->deviceLocator;
    }
    
    this->deviceLocator = location.clone();
}

DeviceLocatorInterface *TCPIPv4SocketBus::getLocation() {
    return this->deviceLocator;
}

void TCPIPv4SocketBus::addHelper(ProtocolHint *hint, TransferHelper *helper) {
    this->helperKeys.push_back(hint);
    this->helperValues.push_back(helper);
}

void TCPIPv4SocketBus::clearHelpers() {
    for(unsigned int i = 0; i < this->helperKeys.size(); i++) {
        delete this->helperKeys[i];
        delete this->helperValues[i];
    }
    this->helperKeys.resize(0);
    this->helperValues.resize(0);
}

TransferHelper *TCPIPv4SocketBus::getHelper(const vector<ProtocolHint *> &hints) const {
    /* Just grab the first hint and use that to look up a helper.
     * The helpers for Ocean Optics USB devices are 1:1 with respect to hints.
     */

    /* Note: this should really be done with a map or hashmap, but I am just not able
     * to get that to work (it was returning bad values).  Feel free to reimplement
     * this in a cleaner fashion.
     */
    unsigned int i;
    for(i = 0; i < this->helperKeys.size(); i++) {
        if((*(this->helperKeys[i])) == (*hints[0])) {
            return this->helperValues[i];
        }
    }
    return NULL;
}
