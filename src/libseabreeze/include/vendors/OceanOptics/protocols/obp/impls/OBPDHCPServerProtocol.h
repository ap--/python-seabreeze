/***************************************************//**
 * @file    OBPDHCPServerProtocol.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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

#ifndef OBPDHCPSERVERPROTOCOL_H
#define OBPDHCPSERVERPROTOCOL_H

#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/interfaces/DHCPServerProtocolInterface.h"
#include "common/exceptions/ProtocolException.h"

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPDHCPServerProtocol : public DHCPServerProtocolInterface {
    public:
        OBPDHCPServerProtocol();

        virtual ~OBPDHCPServerProtocol();

        /* Inherited from OBPDHCPServerProtocolInterface */
        virtual void getServerAddress(const Bus &bus, unsigned char interfaceIndex, std::vector<unsigned char> *serverAddress, unsigned char *netMask);
        virtual void setServerAddress(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> macAddress, unsigned char netMask);
        virtual unsigned char getServerEnableStatus(const Bus &bus, unsigned char interfaceIndex);
        virtual void setServerEnableStatus(const Bus &bus, unsigned char interfaceIndex, unsigned char enableStatue);


    };
  }
}

#endif /* OBPDHCPSERVERPROTOCOL_H */
