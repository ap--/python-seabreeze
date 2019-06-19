/***************************************************//**
 * @file    IPv4SocketDeviceLocator.h
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

#ifndef SEABREEZE_IPV4SOCKETDEVICELOCATOR_H
#define SEABREEZE_IPV4SOCKETDEVICELOCATOR_H

#include "common/buses/DeviceLocatorInterface.h"
#include "common/buses/network/IPv4NetworkProtocol.h"
#include <string>

namespace seabreeze {
    class IPv4SocketDeviceLocator : public DeviceLocatorInterface {
    public:
        IPv4SocketDeviceLocator(const IPv4NetworkProtocol &proto, std::string ip,
            int portNumber);
        virtual ~IPv4SocketDeviceLocator();
        
        std::string getIPv4Address();
        int getPort();
        IPv4NetworkProtocol getIPv4NetworkProtocol();
        
        /* Inherited from DeviceLocatorInterface */
        virtual unsigned long getUniqueLocation() const;
        virtual bool equals(DeviceLocatorInterface &that);
        virtual std::string getDescription();
        virtual BusFamily getBusFamily() const;
        virtual DeviceLocatorInterface *clone() const;
        
    protected:
        unsigned long computeLocationHash();
        
        IPv4NetworkProtocol protocol;
        std::string ipAddr;
        int port;
        unsigned long locationHash;
    };
}

#endif /* SEABREEZE_IPV4SOCKETDEVICELOCATOR_H */

