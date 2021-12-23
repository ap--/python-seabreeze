/***************************************************//**
 * @file    NativeSocketWindows.c
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
#include "common/SeaBreeze.h"
#include "native/network/windows/NativeSocketWindows.h"
#include "native/network/SocketTimeoutException.h"

using namespace seabreeze;
using namespace std;

Socket *Socket::create() {
    return new NativeSocketWindows();
}

NativeSocketWindows::NativeSocketWindows() {
    this->sock = -1;
    this->bound = false;
    this->closed = true;
}

NativeSocketWindows::~NativeSocketWindows() {
    close();
}

void NativeSocketWindows::connect(Inet4Address &addr, int port) {
    struct in_addr in;
    struct sockaddr_in sockaddr;
    int addrlen;
    SOCKET server;

    in = addr.getAddress();

    memset((void *)&sockaddr, (int)0, sizeof(struct sockaddr));
    memcpy((char *)&sockaddr.sin_addr.s_addr, &in, sizeof(struct in_addr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    server = socket(PF_INET, SOCK_STREAM, 0);

    addrlen = sizeof(sockaddr);
    if(::connect(server, (struct sockaddr *)&sockaddr, addrlen) < 0) {
        this->sock = -1;
        this->closed = true;
        string error("Socket connect failed: ");
        error += "Error " + WSAGetLastError();
        throw BusConnectException(error);
    }

    this->bound = true;
    this->closed = false;
    this->sock = server;
    this->address = addr;
}

void NativeSocketWindows::connect(const string hostname, int port) {

    struct hostent *host_info;
    struct in_addr in;

    host_info = gethostbyname(hostname.c_str());
    if(0 == host_info) {
        string error("Failed to resolve hostname [");
		error += hostname;
		error += "]: ";
		error += WSAGetLastError();
        throw BusConnectException(error);
    }

    memcpy(&in, host_info->h_addr_list[0], host_info->h_length);

    Inet4Address inet4addr(&in);

    connect(inet4addr, port);
}

void NativeSocketWindows::close() {
    int result;

    if(this->sock >= 0 && false == this->closed) {
        shutdown(this->sock, SD_BOTH);
        result = ::closesocket(this->sock);
        this->sock = -1;
        this->bound = false;
        this->closed = true;

        if(result != 0) {
            string error("Got error when trying to close socket: ");
            error += "Error " + WSAGetLastError();
            throw BusException(error);
        }
    }
}

bool NativeSocketWindows::isClosed() {
    return this->closed;
}

bool NativeSocketWindows::isBound() {
    return this->bound;
}

int NativeSocketWindows::getSOLinger() {
    linger so_linger;
    int length;
    int result;

    if(this->sock < 0) {
        string error("Attempted to get socket options on invalid socket.");
        throw SocketException(error);
    }

    length = sizeof(so_linger);
    result = getsockopt(this->sock, SOL_SOCKET, SO_LINGER, (char *)&so_linger,
            &length);

    if(result < 0 || length != sizeof(so_linger)) {
        string error("Failed to get socket options: ");
        error += "Error " + WSAGetLastError();
        throw SocketException(error);
    }

    if(0 == so_linger.l_onoff) {
        return 0;
    }

    return so_linger.l_linger;
}

void NativeSocketWindows::setSOLinger(bool enable, int linger) {
    struct linger so_linger;
    int result;

    if(this->sock < 0) {
        string error("Attempted to set socket options on invalid socket.");
        throw SocketException(error);
    }

    so_linger.l_onoff = (true == enable ? 1 : 0);
    so_linger.l_linger = linger;

    result = setsockopt(this->sock, SOL_SOCKET, SO_LINGER, (char *)&so_linger,
            sizeof(so_linger));

    if(result < 0) {
        string error("Failed to set socket options: ");
        error += "Error " + WSAGetLastError();
        throw SocketException(error);
    }
}

unsigned long NativeSocketWindows::getReadTimeoutMillis() {
    unsigned long timeoutMillis;
    int result;
    unsigned int length;

    if(this->sock < 0) {
        string error("Attempted to get socket options on invalid socket.");
        throw SocketException(error);
    }

    length = sizeof(timeoutMillis);
    result = getsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO,
                (char *)&timeoutMillis, (int *)&length);

    if(result < 0 || length != sizeof(timeoutMillis)) {
        string error("Failed to get socket options: ");
        error += "Error " + WSAGetLastError();
        throw SocketException(error);
    }

    return timeoutMillis;
}

void NativeSocketWindows::setReadTimeoutMillis(unsigned long timeoutMillis) {
    int result;

    if(this->sock < 0) {
        string error("Attempted to get socket options on invalid socket.");
        throw SocketException(error);
    }

    result = setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO,
                (char *)&timeoutMillis, sizeof(timeoutMillis));

    if(result < 0) {
        string error("Failed to set socket options: ");
        error += "Error " + WSAGetLastError();
        throw SocketException(error);
    }
}

int NativeSocketWindows::read(unsigned char *buf, unsigned long count) {
    int result = ::recv(this->sock, (char *)buf, count, 0);

    if(result < 0) {
        int err = WSAGetLastError();
        if(WSAEWOULDBLOCK == err) {
            string error("No data available on non-blocking socket.");
            throw SocketTimeoutException(error);
        } else {
            string error("Socket error on read: ");
            error += "Error " + WSAGetLastError();
            throw SocketException(error);
        }
    }

    return result;
}

int NativeSocketWindows::write(const unsigned char *buf, unsigned long count) {
    int result = ::send(this->sock, (char *)buf, count, 0);

    if(result < 0) {
        string error("Socket error on write: ");
            error += "Error " + WSAGetLastError();
        throw BusTransferException(error);
    }

    return result;
}
