/***************************************************//**
 * @file    OBPWifiConfigurationProtocol.h
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

#ifndef OBPWIFICONFIGURATIONPROTOCOL_H
#define OBPWIFICONFIGURATIONPROTOCOL_H

#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/interfaces/WifiConfigurationProtocolInterface.h"
#include "common/exceptions/ProtocolException.h"

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPWifiConfigurationProtocol : public WifiConfigurationProtocolInterface {
    public:
        OBPWifiConfigurationProtocol();

        virtual ~OBPWifiConfigurationProtocol();

        /* Inherited from OBPWifiConfigurationProtocolInterface */

        virtual unsigned char getMode(const Bus &bus, unsigned char interfaceIndex);
        virtual void setMode(const Bus &bus, unsigned char interfaceIndex, unsigned char mode);
        virtual unsigned char getSecurityType(const Bus &bus, unsigned char interfaceIndex);
        virtual void setSecurityType(const Bus &bus, unsigned char interfaceIndex, unsigned char securityType);
        virtual std::vector<unsigned char> getSSID(const Bus &bus, unsigned char interfaceIndex);
        virtual void setSSID(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> ssid);
        virtual void setPassPhrase(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> passPhrase);


    };
  }
}

#endif /* OBPWIFICONFIGURATIONPROTOCOL_H */
