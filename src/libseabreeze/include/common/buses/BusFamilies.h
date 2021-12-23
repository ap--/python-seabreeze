/***************************************************//**
 * @file    BusFamilies.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This provides a way to get references to different kinds of buses
 * (e.g. USB, Ethernet, serial) generically.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
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

#ifndef BUSFAMILIES_H
#define BUSFAMILIES_H

#include "common/buses/BusFamily.h"
#include <vector>

namespace seabreeze {

    class USBBusFamily : public BusFamily {
    public:
        USBBusFamily();
        virtual ~USBBusFamily();
    };

    class EthernetBusFamily : public BusFamily {
    public:
        EthernetBusFamily();
        virtual ~EthernetBusFamily();
    };

    class RS232BusFamily : public BusFamily {
    public:
        RS232BusFamily();
        virtual ~RS232BusFamily();
    };

    class TCPIPv4BusFamily : public BusFamily {
    public:
        TCPIPv4BusFamily();
        virtual ~TCPIPv4BusFamily();
    };

    class UDPIPv4BusFamily : public BusFamily {
    public:
        UDPIPv4BusFamily();
        virtual ~UDPIPv4BusFamily();
    };

    class BusFamilies {
    public:
        const USBBusFamily USB;
        const EthernetBusFamily ETHERNET;
        const RS232BusFamily RS232;
        const TCPIPv4BusFamily TCPIPv4;
        const UDPIPv4BusFamily UDPIPv4;

        BusFamilies();
        ~BusFamilies();
        std::vector<BusFamily *> getAllBusFamilies();
    };

}

#endif /* BUSFAMILIES_H */
