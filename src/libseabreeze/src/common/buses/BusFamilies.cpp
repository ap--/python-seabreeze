/***************************************************//**
 * @file    BusFamilies.cpp
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

#include "common/globals.h"
#include "common/buses/BusFamilies.h"

/* Constants */
#define BUS_FAMILY_ID_USB       0
#define BUS_FAMILY_ID_ETHERNET  1
#define BUS_FAMILY_ID_RS232     2
#define BUS_FAMILY_ID_TCPIPV4   3
#define BUS_FAMILY_ID_UDPIPV4   4

using namespace seabreeze;
using namespace std;

BusFamilies::BusFamilies() {

}

BusFamilies::~BusFamilies() {

}

USBBusFamily::USBBusFamily() : BusFamily("USB", BUS_FAMILY_ID_USB) {

}

USBBusFamily::~USBBusFamily() {

}

EthernetBusFamily::EthernetBusFamily()
        : BusFamily("Ethernet", BUS_FAMILY_ID_ETHERNET) {

}

EthernetBusFamily::~EthernetBusFamily() {

}

RS232BusFamily::RS232BusFamily() : BusFamily("RS232", BUS_FAMILY_ID_RS232) {

}

RS232BusFamily::~RS232BusFamily() {

}

TCPIPv4BusFamily::TCPIPv4BusFamily() : BusFamily("TCP/IPv4", BUS_FAMILY_ID_TCPIPV4) {

}

TCPIPv4BusFamily::~TCPIPv4BusFamily() {

}

UDPIPv4BusFamily::UDPIPv4BusFamily() : BusFamily("UDP/IPv4", BUS_FAMILY_ID_UDPIPV4) {

}

UDPIPv4BusFamily::~UDPIPv4BusFamily() {

}

vector<BusFamily *> BusFamilies::getAllBusFamilies() {
    vector<BusFamily *> retval;

    /* This creates new instances of these so the class-wide fields do not risk
     * having their const flags ignored.
     */
    USBBusFamily *usb = new USBBusFamily();
    EthernetBusFamily *ethernet = new EthernetBusFamily();
    RS232BusFamily *rs232 = new RS232BusFamily();
    TCPIPv4BusFamily *tcpipv4 = new TCPIPv4BusFamily();
    UDPIPv4BusFamily *udpipv4 = new UDPIPv4BusFamily();

    retval.push_back(usb);
    retval.push_back(ethernet);
    retval.push_back(rs232);
    retval.push_back(tcpipv4);
    retval.push_back(udpipv4);

    return retval;
}
