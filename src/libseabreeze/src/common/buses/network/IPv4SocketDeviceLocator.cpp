/***************************************************//**
 * @file    IPv4SocketDeviceLocator.cpp
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

/* Includes */
#include "common/buses/network/IPv4SocketDeviceLocator.h"
#include "common/buses/network/IPv4NetworkProtocol.h"
#include "common/buses/BusFamilies.h"

#include <stdio.h>
#include <stdexcept>

using namespace seabreeze;
using namespace std;

IPv4SocketDeviceLocator::IPv4SocketDeviceLocator(const IPv4NetworkProtocol &proto,
        string ip, int portNumber) : protocol(proto), ipAddr(ip), port(portNumber) {
    
    this->locationHash = computeLocationHash();
}

IPv4SocketDeviceLocator::~IPv4SocketDeviceLocator() {
    
}

string IPv4SocketDeviceLocator::getIPv4Address() {
    return this->ipAddr;
}

int IPv4SocketDeviceLocator::getPort() {
    return this->port;
}

IPv4NetworkProtocol IPv4SocketDeviceLocator::getIPv4NetworkProtocol() {
    return this->protocol;
}

unsigned long IPv4SocketDeviceLocator::getUniqueLocation() const {
    return this->locationHash;
}

bool IPv4SocketDeviceLocator::equals(DeviceLocatorInterface &that) {
    IPv4SocketDeviceLocator *loc;

    loc = dynamic_cast<IPv4SocketDeviceLocator *>(&that);
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

string IPv4SocketDeviceLocator::getDescription() {
    char buffer[80];
    /* Produce a string in the following format: protocol:address:port */
#ifdef _WINDOWS
	_snprintf(buffer, 79, "%s:%s:%d", this->protocol.getName().c_str(),
		this->ipAddr.c_str(), this->port);
#else
    snprintf(buffer, 79, "%s:%s:%d", this->protocol.getName().c_str(),
            this->ipAddr.c_str(), this->port);
#endif
    string retval(buffer);
    return retval;
}

BusFamily IPv4SocketDeviceLocator::getBusFamily() const {
    IPv4NetworkProtocols networkProtocols;
    BusFamilies families;
    
    if(this->protocol.equals(networkProtocols.TCP_IP4)) {
        return families.TCPIPv4;
    } else if(this->protocol.equals(networkProtocols.UDP_IP4)) {
        return families.UDPIPv4;
    }
    
    throw runtime_error("Internal error: unknown IPv4 protocol");
}

DeviceLocatorInterface *IPv4SocketDeviceLocator::clone() const {
    IPv4SocketDeviceLocator *retval = new IPv4SocketDeviceLocator(
            this->protocol, this->ipAddr, this->port);

    return retval;
}

unsigned long IPv4SocketDeviceLocator::computeLocationHash() {
    /* Iterate over the devicePath and compute a sort of hash */
    unsigned long hash = 1;
    string::iterator iter;
    
    string desc = getDescription();

    for(iter = desc.begin(); iter != desc.end(); iter++) {
        /* Overflow here does not cause any problems. */
        hash = 31 * hash + (*iter);
    }

    return hash;
}
