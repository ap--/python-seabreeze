/***************************************************//**
 * @file    NativeSocketPOSIX.h
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

#ifndef SEABREEZE_NATIVESOCKETPOSIX_H
#define SEABREEZE_NATIVESOCKETPOSIX_H

#include "native/network/Socket.h"
#include "native/network/Inet4Address.h"
#include "common/exceptions/BusConnectException.h"
#include <string>

namespace seabreeze {
    class NativeSocketPOSIX : public Socket {
    public:
        NativeSocketPOSIX();
        virtual ~NativeSocketPOSIX();

        virtual void connect(Inet4Address &addr, int port);
        virtual void connect(const std::string hostname, int port);

        virtual void close();
        virtual bool isClosed();
        virtual bool isBound();

        virtual int getSOLinger();
        virtual void setSOLinger(bool enable, int linger);
        virtual unsigned long getReadTimeoutMillis();
        virtual void setReadTimeoutMillis(unsigned long timeout);

        virtual int read(unsigned char *buffer, unsigned long length);
        virtual int write(const unsigned char *buffer, unsigned long length);

    private:
        int sock;
        bool bound;
        bool closed;
        Inet4Address address;
    };
}

#endif /* SEABREEZE_NATIVESOCKETPOSIX_H */
