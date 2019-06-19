/***************************************************//**
 * @file    USB.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
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

#ifndef SEABREEZE_USB_H
#define SEABREEZE_USB_H

#include "native/usb/USBDiscovery.h"
#include "native/usb/NativeUSB.h"
#include <string>

namespace seabreeze {

    /* Empty declaration of USBDiscovery to deal with cross-includes */
    class USBDiscovery;

    class USB {
    public:
        virtual ~USB();

        bool open();
        bool close();
        int write(int endpoint, void *data, unsigned int length_bytes);
        int read(int endpoint, void *data, unsigned int length_bytes);
        void clearStall(int endpoint);

        static void setVerbose(bool v);

        int getDeviceDescriptor(struct USBDeviceDescriptor *desc);
        int getInterfaceDescriptor(struct USBInterfaceDescriptor *desc);
        /* Get the endpoint descriptor where index is the endpoint index. */
        int getEndpointDescriptor(int index, struct USBEndpointDescriptor *epDesc);
        std::string *getStringDescriptor(int index);
        int getMaxPacketSize();

        bool isOpened();

        /* This allows the USBDiscovery class to access the protected
         * constructor and act as a factory for USB instances.
         */
        friend class USBDiscovery;

    protected:

        /* These methods are primarily for debugging. */
        void usbHexDump(void *x, int length, int endpoint);
        void hexDump(void *x, int length);
        void describeTransfer(const char *label, int length, void* data, int endpoint, bool hexdump);
        USB(unsigned long deviceID);

        void *descriptor;
        bool opened;
        static bool verbose;
        unsigned long deviceID;
    };

}

#endif
