/***************************************************//**
 * @file    NativeSocketWindows.h
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

#ifndef SEABREEZE_NATIVESOCKETWINDOWS_H
#define SEABREEZE_NATIVESOCKETWINDOWS_H

#include "common/SeaBreeze.h"
#include "native/network/Socket.h"
#include "native/network/Inet4Address.h"
#include "common/exceptions/BusConnectException.h"
#include <Winsock2.h>
#include <string>

namespace seabreeze {
    class NativeSocketWindows : public Socket {
    public:
        NativeSocketWindows();
        virtual ~NativeSocketWindows();
        
        virtual void connect(Inet4Address &addr, int port)
                throw (UnknownHostException, BusConnectException);
        virtual void connect(const std::string hostname, int port)
                throw (UnknownHostException, BusConnectException);
        
        virtual void close() throw (BusException);
        virtual bool isClosed();
        virtual bool isBound();
        
        virtual int getSOLinger() throw (SocketException);
        virtual void setSOLinger(bool enable, int linger) throw (SocketException);
        virtual unsigned long getReadTimeoutMillis() throw (SocketException);
        virtual void setReadTimeoutMillis(unsigned long timeout) throw (SocketException);
        
        virtual int read(unsigned char *buffer, unsigned long length)
            throw (BusTransferException);
        virtual int write(const unsigned char *buffer, unsigned long length)
            throw (BusTransferException);
        
    private:
        SOCKET sock;
        bool bound;
        bool closed;
        Inet4Address address;
    };
}

#endif /* SEABREEZE_NATIVESOCKETWINDOWS_H */
