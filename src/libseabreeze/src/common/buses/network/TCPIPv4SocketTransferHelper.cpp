/***************************************************//**
 * @file    TCPIPv4SocketTransferHelper.cpp
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

#include "common/buses/network/TCPIPv4SocketTransferHelper.h"

using namespace seabreeze;
using namespace std;

TCPIPv4SocketTransferHelper::TCPIPv4SocketTransferHelper(Socket *sock) {
    this->socket = sock;
}

TCPIPv4SocketTransferHelper::~TCPIPv4SocketTransferHelper() {
    /* Note that disposal of the socket is the responsibility of the Bus
     * (i.e. TCPIPv4SocketBus).
     */
}

int TCPIPv4SocketTransferHelper::receive(vector<unsigned char> &buffer,
        unsigned int length) {

    unsigned char *rawBuffer = (unsigned char *)&buffer[0];
    unsigned int bytesRead = 0;

    /* TODO: There should be a couple alternatives for this.  One should
     * poll more or less indefinitely.  The other should do a single read
     * attempt and return, which allows the Protocol layer to decide what
     * to do next.
     */
    try {
        while(bytesRead < length) {
            int result = this->socket->read(&rawBuffer[bytesRead],
                    length - bytesRead);
            if(result > 0) {
                bytesRead += result;
            } else {
                /* This should only be possible if a timeout was set for the
                 * socket.
                 */
                break;
            }
        }
    } catch (BusTransferException &bte) {
        if(0 == bytesRead) {
            throw bte;
        }
    }
    return bytesRead;
}

int TCPIPv4SocketTransferHelper::send(const vector<unsigned char> &buffer,
        unsigned int length) const {

    unsigned char *rawBuffer = (unsigned char *)&buffer[0];
    unsigned int written = 0;

    while(written < length) {
        /* This may throw a BusTransferException.  This needs to be dealt with
         * by the caller.
         */
        int result = this->socket->write(&rawBuffer[written], length - written);
        if(result > 0) {
            written += result;
        } else {
            break;
        }
    }
    return written;
}
