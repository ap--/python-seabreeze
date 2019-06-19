/***************************************************//**
 * @file    TCPIPv4SocketBus.h
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

#ifndef SEABREEZE_TCPIPV4SOCKETBUS_H
#define SEABREEZE_TCPIPV4SOCKETBUS_H

#include "common/buses/Bus.h"
#include "common/exceptions/IllegalArgumentException.h"
#include "native/network/Socket.h"
#include <vector>

namespace seabreeze {
    class TCPIPv4SocketBus : public Bus {
    public:
        TCPIPv4SocketBus();
        virtual ~TCPIPv4SocketBus();
        
        virtual Socket *getSocketDescriptor();
        
        virtual BusFamily getBusFamily() const;
        
        virtual void setLocation(const DeviceLocatorInterface &location)
                throw (IllegalArgumentException);
        virtual DeviceLocatorInterface *getLocation();
        virtual TransferHelper *getHelper(
                const std::vector<ProtocolHint *> &hints) const;
        
        /* Pure virtual methods */
        virtual bool open() = 0;
        virtual void close() = 0;
        
        
    protected:
        void addHelper(ProtocolHint *hint, TransferHelper *helper);
        void clearHelpers();
        
        Socket *socket;
        DeviceLocatorInterface *deviceLocator;
        
        /* These vectors should really be in a map, but that didn't want to
         * work easily.  Since there will likely be about 2 entries in here,
         * storing in a pair of vectors for now won't hurt anything.
         */
        std::vector<ProtocolHint *> helperKeys;
        std::vector<TransferHelper *> helperValues;
    };
}

#endif /* SEABREEZE_TCPIPV4SOCKETBUS_H */
