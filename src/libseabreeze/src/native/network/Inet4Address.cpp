/***************************************************//**
 * @file    Inet4Address.cpp
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

#include "native/network/Inet4Address.h"
#include <string.h>

#ifndef _WINDOWS
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

using namespace seabreeze;
using namespace std;

Inet4Address::Inet4Address() {
    memset(&(this->in), 0, sizeof(struct in_addr));
}

Inet4Address::Inet4Address(std::string ipAddressQuads) {

    const char *addrString = ipAddressQuads.c_str();

#ifdef _WINDOWS
    this->in.S_un.S_addr = inet_addr(addrString);
#else
    if(0 == inet_aton(addrString, &(this->in))) {
        throw IllegalArgumentException("Invalid IPv4 address");
    }
#endif
}

Inet4Address::Inet4Address(struct in_addr *ipAddr) {
    memcpy(&(this->in), ipAddr, sizeof(struct in_addr));
}

Inet4Address::Inet4Address(const Inet4Address &that) {
    memcpy(&(this->in), &(that.in), sizeof(struct in_addr));
}

Inet4Address::~Inet4Address() {

}

struct in_addr Inet4Address::getAddress() {
    return this->in;
}

string Inet4Address::getHostAddress() {
    char *addr = inet_ntoa(this->in);
    return string(addr);
}
