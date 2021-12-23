/***************************************************//**
 * @file    IPv4NetworkProtocol.h
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

#ifndef SEABREEZE_IPV4NETWORKPROTOCOL_H
#define SEABREEZE_IPV4NETWORKPROTOCOL_H

#include <string>
#include <vector>

namespace seabreeze {
    class IPv4NetworkProtocol {
    public:
        virtual ~IPv4NetworkProtocol();
        virtual std::string getName() const;
        virtual bool equals(const IPv4NetworkProtocol &that) const;

    protected:
        IPv4NetworkProtocol(std::string name, int id);

    private:
        std::string protocolName;
        int type;
    };

    class TCP_IPv4 : public IPv4NetworkProtocol {
    public:
        TCP_IPv4();
        virtual ~TCP_IPv4();
    };

    class UDP_IPv4 : public IPv4NetworkProtocol {
    public:
        UDP_IPv4();
        virtual ~UDP_IPv4();
    };

    class IPv4NetworkProtocols {
    public:
        const TCP_IPv4 TCP_IP4;
        const UDP_IPv4 UDP_IP4;

        IPv4NetworkProtocols();
        ~IPv4NetworkProtocols();
        std::vector<IPv4NetworkProtocol *> getAllIPv4NetworkProtocols();
    };
}

#endif /* SEABREEZE_IPV4NETWORKPROTOCOL_H */
