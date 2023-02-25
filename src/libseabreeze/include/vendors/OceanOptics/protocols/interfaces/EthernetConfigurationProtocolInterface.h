/***************************************************//**
 * @file    EthernetConfigurationProtocolInterface.h
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

#ifndef ETHERNETCONFIGURATIONPROTOCOLINTERFACE_H
#define ETHERNETCONFIGURATIONPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class EthernetConfigurationProtocolInterface : public ProtocolHelper {
    public:
        EthernetConfigurationProtocolInterface(Protocol *protocol);
        virtual ~EthernetConfigurationProtocolInterface();

        /**
         * Get the MAC address from the device.
         */
        virtual std::vector<unsigned char> get_MAC_Address(const Bus &bus, unsigned char interfaceIndex) = 0;
        /**
         *  Set the device MAC address.
         */
        virtual void set_MAC_Address(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> macAddress) = 0;

        /**
         * Get the GbE enable status from the device.
         */
        virtual unsigned char get_GbE_Enable_Status(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         * Set the GbE enable status on the device
         */
        virtual void set_GbE_Enable_Status(const Bus &bus, unsigned char interfaceIndex, unsigned char enableState) = 0;

    };

}

#endif /* ETHERNETCONFIGURATIONPROTOCOLINTERFACE_H */
