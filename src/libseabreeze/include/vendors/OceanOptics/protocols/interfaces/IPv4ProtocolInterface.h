/***************************************************//**
 * @file    IPv4ProtocolInterface.h
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

#ifndef IPV4PROTOCOLINTERFACE_H
#define IPV4PROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class IPv4ProtocolInterface : public ProtocolHelper {
    public:
        IPv4ProtocolInterface(Protocol *protocol);
        virtual ~IPv4ProtocolInterface();


	    /**
         * Is the DHCP client enabled for IPv4? 1: enabled, 0: disabled
         */
        virtual unsigned char get_IPv4_DHCP_Enable_State(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         * Set the enable state for the IPv4 client
         */
        virtual void set_IPv4_DHCP_Enable_State(const Bus &bus, unsigned char interfaceIndex, unsigned char enableState) = 0;

	    /**
         * Get the number of IPv4 addresses that can be indexed.
         */
        virtual unsigned char get_Number_Of_IPv4_Addresses(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         * Get an IPv4 address by its index
         */
        virtual void get_IPv4_Address(const Bus &bus, unsigned char interfaceIndex, unsigned char addressIndex, std::vector<unsigned char> *IPv4_Address, unsigned char *netMask) = 0;

        /**
         * Get the IPv4 gateway for the indexed interface
         */
        virtual std::vector<unsigned char> get_IPv4_Default_Gateway(const Bus &bus, unsigned char interfaceIndex) = 0;

        /**
         *  Set the gateway address for indexed interface
         */
        virtual void set_IPv4_Default_Gateway(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> IPv4_Gateway_Address) = 0;


        /**
         *  Add an IPv4 address to the indexed interface
         */
        virtual void add_IPv4_Address(const Bus &bus, unsigned char interfaceIndex, const std::vector<unsigned char> IPv4_Address, unsigned char netMask) = 0;

       /**
         *  Remove and IPv4 address from the indexed interface
         */
        virtual void delete_IPv4_Address(const Bus &bus, unsigned char interfaceIndex, unsigned char addressIndex) = 0;
    };

}

#endif /* IPV4PROTOCOLINTERFACE_H */
