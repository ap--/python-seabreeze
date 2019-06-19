/***************************************************//**
 * @file    NativeSocketPOSIX.c
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
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <string.h>

#include "native/network/posix/NativeSocketPOSIX.h"
#include "native/network/SocketTimeoutException.h"

using namespace seabreeze;
using namespace std;

Socket *Socket::create() {
    return new NativeSocketPOSIX();
}

NativeSocketPOSIX::NativeSocketPOSIX() {
    this->sock = -1;
    this->bound = false;
    this->closed = true;
}
        
NativeSocketPOSIX::~NativeSocketPOSIX() {
    close();
}
        
void NativeSocketPOSIX::connect(Inet4Address &addr, int port)
        throw (UnknownHostException, BusConnectException) {
    struct in_addr in;
    struct sockaddr_in sockaddr;
    socklen_t addrlen;
    int server;
    
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
        error += strerror(errno);
        throw BusConnectException(error);
    }
    
    this->bound = true;
    this->closed = false;
    this->sock = server;
    this->address = addr;
}

void NativeSocketPOSIX::connect(const string hostname, int port)
        throw (UnknownHostException, BusConnectException) {
    
    struct hostent *host_info;
    struct in_addr in;
    
    host_info = gethostbyname(hostname.c_str());
    if(0 == host_info) {
        string error("Failed to resolve hostname [");
        error += hostname + "]: " + strerror(errno);
        throw BusConnectException(error);
    }
    
    memcpy(&in, host_info->h_addr_list[0], host_info->h_length);
    
    Inet4Address inet4addr(&in);
    
    connect(inet4addr, port);
}

void NativeSocketPOSIX::close() throw (BusException) {
    int result;
    
    if(this->sock >= 0 && false == this->closed) {
        shutdown(this->sock, SHUT_RDWR);
        result = ::close(this->sock);
        this->sock = -1;
        this->bound = false;
        this->closed = true;
        
        if(result < 0) {
            string error("Got error when trying to close socket: ");
            error += strerror(errno);
            throw BusException(error);
        }
    }
}

bool NativeSocketPOSIX::isClosed() {
    return this->closed;
}

bool NativeSocketPOSIX::isBound() {
    return this->bound;
}

int NativeSocketPOSIX::getSOLinger() throw (SocketException) {
    struct linger so_linger;
    socklen_t length;
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
        error += strerror(errno);
        throw SocketException(error);
    }
    
    if(0 == so_linger.l_onoff) {
        return 0;
    }
    
    return so_linger.l_linger;
}

void NativeSocketPOSIX::setSOLinger(bool enable, int linger)
        throw (SocketException) {
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
        error += strerror(errno);
        throw SocketException(error);
    }
}

unsigned long NativeSocketPOSIX::getReadTimeoutMillis() throw (SocketException) {
    struct timeval timeout;
    int result;
    socklen_t length;
    
    if(this->sock < 0) {
        string error("Attempted to get socket options on invalid socket.");
        throw SocketException(error);
    }
    
    length = sizeof(timeout);
    result = getsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
            &length);
    
    if(result < 0 || length != sizeof(timeout)) {
        string error("Failed to get socket options: ");
        error += strerror(errno);
        throw SocketException(error);
    }
    
    return (timeout.tv_sec * 1000) + (timeout.tv_usec / 1000);
}

void NativeSocketPOSIX::setReadTimeoutMillis(unsigned long timeoutMillis)
        throw (SocketException) {
    struct timeval timeout;
    int result;
    
    if(this->sock < 0) {
        string error("Attempted to get socket options on invalid socket.");
        throw SocketException(error);
    }
    
    timeout.tv_sec = timeoutMillis / 1000;
    timeout.tv_usec = (timeoutMillis % 1000) * 1000; /* Isolate millis then convert to micros */
    
    result = setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
            sizeof(timeout));
    
    if(result < 0) {
        string error("Failed to set socket options: ");
        error += strerror(errno);
        throw SocketException(error);
    }
}

int NativeSocketPOSIX::read(unsigned char *buf, unsigned long count)
            throw (BusTransferException) {
    int result = ::read(this->sock, buf, count);
    
    if(result < 0) {
        if(EAGAIN == errno) {
            string error("No data available on non-blocking socket.");
            throw SocketTimeoutException(error);
        } else {
            string error("Socket error on read: ");
            error += strerror(errno);
            throw SocketException(error);
        }
    }
    
    return result;
}

int NativeSocketPOSIX::write(const unsigned char *buf, unsigned long count)
            throw (BusTransferException) {
    int result = ::write(this->sock, buf, count);
    
    if(result < 0) {
        string error("Socket error on write: ");
        error += strerror(errno);
        throw BusTransferException(error);
    }
    
    return result;
}
