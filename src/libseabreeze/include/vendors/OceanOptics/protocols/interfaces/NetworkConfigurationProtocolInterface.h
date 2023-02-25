/***************************************************//**
 * @file    NetworkConfigurationProtocolInterface.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a generic interface into network configuration
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

#ifndef NETWORKCONFIGURATIONPROTOCOLINTERFACE_H
#define NETWORKCONFIGURATIONPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class NetworkConfigurationProtocolInterface : public ProtocolHelper {
    public:
        NetworkConfigurationProtocolInterface(Protocol *protocol);
        virtual ~NetworkConfigurationProtocolInterface();

        /**
         * Get the number of network interfaces available
         */
        virtual unsigned char getNumberOfNetworkInterfaces(const Bus &bus) = 0;

        /**
         *  Get the interface type. 0 = loopback, 1 = wired ethernet, 2= wifi, 3 = cdc ethernet (usb)
         */
        virtual unsigned char getNetworkInterfaceConnectionType(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         *  Find out if the referenced network interface is enabled
         */
        virtual unsigned char getNetworkInterfaceEnableState(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         *  Enable or disable a network interface
         */
        virtual void setNetworkInterfaceEnableState(const Bus &bus, unsigned char interfaceIndex, unsigned char enableState) = 0;

        /**
         *  Run a self test on the network interface of interest
         */
        virtual unsigned char runNetworkInterfaceSelfTest(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         *  Store all of the settings for the network interface of interest
         */
        virtual void saveNetworkInterfaceConnectionSettings(const Bus &bus, unsigned char interfaceIndex) = 0;

    };

}

#endif /* NETWORKCONFIGURATIONPROTOCOLINTERFACE_H */
