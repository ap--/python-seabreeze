/***************************************************//**
 * @file    Socket.h
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

#ifndef SEABREEZE_SOCKET_H
#define SEABREEZE_SOCKET_H

/* Includes */
#include "common/SeaBreeze.h"
#include "common/exceptions/BusTransferException.h"
#include "common/exceptions/BusConnectException.h"
#include "native/network/SocketException.h"
#include "native/network/UnknownHostException.h"
#include "native/network/Inet4Address.h"
#include <string>

namespace seabreeze {
    
    class Socket {
    public:
        static Socket *create();
        
        virtual ~Socket();
        
        virtual void connect(Inet4Address &addr, int port)
                throw (UnknownHostException, BusConnectException) = 0;
        virtual void connect(const std::string host, int port)
                throw (UnknownHostException, BusConnectException) = 0;
        
        virtual void close() throw (BusException) = 0;
        virtual bool isClosed() = 0;
        virtual bool isBound() = 0;
        
        /* Socket options */
        virtual int getSOLinger() throw (SocketException) = 0;
        virtual void setSOLinger(bool enable, int linger)
            throw (SocketException) = 0;
        virtual unsigned long getReadTimeoutMillis()
            throw (SocketException) = 0;
        virtual void setReadTimeoutMillis(unsigned long timeout)
            throw (SocketException) = 0;
        
        /* Data transfer */
        virtual int read(unsigned char *buffer, unsigned long length)
            throw (BusTransferException) = 0;
        virtual int write(const unsigned char *buffer, unsigned long length)
            throw (BusTransferException) = 0;
        
    };
    
    /* Default implementation for (otherwise) pure virtual destructor */
    inline Socket::~Socket() {}
}

#endif /* SEABREEZE_SOCKET_H */
