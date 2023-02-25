/***************************************************//**
 * @file    WifiConfigurationProtocolInterface.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a generic interface into ethernet configuration
 * functionality at the protocol level, agnostic to
 * any particular protocol.  Each Protocol offering this
 * functionality should implement this interface.
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

#ifndef WIFICONFIGURATIONPROTOCOLINTERFACE_H
#define WIFICONFIGURATIONPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class WifiConfigurationProtocolInterface : public ProtocolHelper {
    public:
        WifiConfigurationProtocolInterface(Protocol *protocol);
        virtual ~WifiConfigurationProtocolInterface();


	    /**
         * Get the Wifi mode (0: client, 1: access point, from the device.
         */
        virtual unsigned char getMode(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         * Set the Wifi mode (0: client, 1: access point, for the device.
         */
        virtual void setMode(const Bus &bus, unsigned char interfaceIndex, unsigned char mode) = 0;


          /**
         * Get the Wifi security type (0: open, 1: WPA2, from the device.
         */
        virtual unsigned char getSecurityType(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         * Get the Wifi security type (0: open, 1: WPA2, from the device.
         */
        virtual void setSecurityType(const Bus &bus, unsigned char interfaceIndex, unsigned char securityType) = 0;

        /**
         * Get the 32 byte SSID  from the device.
         */
        virtual std::vector<unsigned char> getSSID(const Bus &bus, unsigned char interfaceIndex) = 0;
        /**
         *  Set the SSID  .
         */
        virtual void setSSID(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> ssid_32_bytes) = 0;

        /**
         *  Set the pass phrase
         */
        virtual void setPassPhrase(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> passPhrase) = 0;


    };

}

#endif /* WIFICONFIGURATIONPROTOCOLINTERFACE_H */
