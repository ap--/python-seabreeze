/***************************************************//**
 * @file    USBDiscovery.h
 * @date    February 2012
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
#ifndef USBDISCOVERY_H
#define USBDISCOVERY_H

#include "native/usb/USB.h"
#include <vector>

namespace seabreeze {

    /* Empty declaration of USB to deal with cross-includes */
    class USB;

    class USBDiscovery {
    public:
        USBDiscovery();
        ~USBDiscovery();

        /**
         * Probes the bus for devices of the given VID and PID and returns
         * a vector of identifiers.  Note that these IDs are implementation-
         * specific and are not necessarily portable between platforms.  They
         * are not guaranteed to be constant from one program execution to the
         * next, though they should remain constant from one invocation of this
         * method to the next within one execution if no devices change status.
         */
        std::vector<unsigned long> *probeDevices(int vendorID, int productID);

        /**
         * Given an identifier from probeDevices(), create a USB interface to
         * the device that can be used to open/write/read/close the device.
         */
        USB *createUSBInterface(unsigned long deviceID);
    };

}

#endif /* USBDISCOVERY_H */
