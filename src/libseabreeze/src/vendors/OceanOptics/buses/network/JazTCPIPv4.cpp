/***************************************************//**
 * @file    JazTCPIPv4.cpp
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

#include <cstddef>
#include "vendors/OceanOptics/buses/network/JazTCPIPv4.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "vendors/OceanOptics/protocols/ooi/hints/SpectrumHint.h"
#include "common/buses/network/IPv4SocketDeviceLocator.h"
#include "common/buses/network/TCPIPv4SocketTransferHelper.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

JazTCPIPv4::JazTCPIPv4() {
    this->socket = Socket::create();
}

JazTCPIPv4::~JazTCPIPv4() {
    if(NULL != this->socket) {
        if(false == this->socket->isClosed()) {
            this->socket->close();
        }
        delete this->socket;
    }
}

bool JazTCPIPv4::open() {
    if(NULL == this->deviceLocator || NULL == this->socket) {
        return false;
    }

    IPv4SocketDeviceLocator *loc = dynamic_cast<IPv4SocketDeviceLocator *>(this->deviceLocator);
    if(NULL == loc) {
        /* Must have been passed an invalid location */
        return false;
    }

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
    try {
        this->socket->connect(loc->getIPv4Address(), loc->getPort());
        this->socket->setSOLinger(false, 1);
        this->socket->setReadTimeoutMillis(0);  /* Wait indefinitely */
    } catch (const UnknownHostException &uhe) {
        return false;
    } catch (const BusConnectException &bce) {
        return false;
    } catch (const SocketException &se) {
        return false;
    }

    addHelper(new SpectrumHint(), new TCPIPv4SocketTransferHelper(this->socket));
    addHelper(new ControlHint(), new TCPIPv4SocketTransferHelper(this->socket));

    return true;
}

void JazTCPIPv4::close() {
    if(NULL != this->socket) {
        this->socket->close();
    }
}
