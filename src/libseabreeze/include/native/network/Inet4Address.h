/***************************************************//**
 * @file    Inet4Address.h
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

#ifndef SEABREEZE_INET4ADDRESS_H
#define SEABREEZE_INET4ADDRESS_H

#include <string>
#include "common/exceptions/IllegalArgumentException.h"
#include "common/SeaBreeze.h"

#ifdef _WINDOWS
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace seabreeze {
    class Inet4Address {
    public:
        Inet4Address();
        Inet4Address(std::string ipAddressQuads)
            throw (IllegalArgumentException);
        Inet4Address(struct in_addr *ipAddr);
        Inet4Address(const Inet4Address &that);
        ~Inet4Address();
        
        struct in_addr getAddress();
        
        std::string getHostAddress();
        
    private:
        struct in_addr in;
    };
}

#endif /* SEABREEZE_INET4ADDRESS_H */
