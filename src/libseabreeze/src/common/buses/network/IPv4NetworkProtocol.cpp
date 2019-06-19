/***************************************************//**
 * @file    IPv4NetworkProtocol.cpp
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
#include "common/buses/network/IPv4NetworkProtocol.h"

using namespace seabreeze;
using namespace std;

/* Constants */
#define IP4_PROTOCOL_ID_TCPIP   0
#define IP4_PROTOCOL_ID_UDPIP   1

IPv4NetworkProtocol::IPv4NetworkProtocol(string name, int id) {
    this->protocolName = name;
    this->type = id;
}

IPv4NetworkProtocol::~IPv4NetworkProtocol() {
    
}

string IPv4NetworkProtocol::getName() const {
    return this->protocolName;
}

bool IPv4NetworkProtocol::equals(const IPv4NetworkProtocol &that) const {
    return this->type == that.type;
}

TCP_IPv4::TCP_IPv4() : IPv4NetworkProtocol("TCP/IPv4", IP4_PROTOCOL_ID_TCPIP) {
    
}

TCP_IPv4::~TCP_IPv4() {
    
}

UDP_IPv4::UDP_IPv4() : IPv4NetworkProtocol("UDP/IPv4", IP4_PROTOCOL_ID_UDPIP) {
    
}

UDP_IPv4::~UDP_IPv4() {
    
}

IPv4NetworkProtocols::IPv4NetworkProtocols() {
    
}

IPv4NetworkProtocols::~IPv4NetworkProtocols() {
    
}

vector<IPv4NetworkProtocol *> IPv4NetworkProtocols::getAllIPv4NetworkProtocols() {
    vector<IPv4NetworkProtocol *> retval;

    /* This creates new instances of these so the class-wide fields do not risk
     * having their const flags ignored.
     */
    IPv4NetworkProtocol *tcp_ipv4 = new TCP_IPv4();
    IPv4NetworkProtocol *udp_ipv4 = new UDP_IPv4();
    
    retval.push_back(tcp_ipv4);
    retval.push_back(udp_ipv4);
    
    return retval;
}
