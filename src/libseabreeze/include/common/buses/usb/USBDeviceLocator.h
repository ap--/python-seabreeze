/***************************************************//**
 * @file    USBDeviceLocator.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This class encapsulates the information needed to open
 * a device on a USB bus.
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

#ifndef USBDEVICELOCATOR_H
#define USBDEVICELOCATOR_H

#include "common/buses/DeviceLocatorInterface.h"
#include <string>

namespace seabreeze {

    class USBDeviceLocator : public DeviceLocatorInterface {
    public:
        USBDeviceLocator(unsigned long ID);
        virtual ~USBDeviceLocator();

        /* Inherited from DeviceLocatorInterface */
        virtual unsigned long getUniqueLocation() const;
        virtual bool equals(DeviceLocatorInterface &that);
        virtual std::string getDescription();
        virtual BusFamily getBusFamily() const;
        virtual DeviceLocatorInterface *clone() const;

    private:
        unsigned long deviceID;
    };

}

#endif /* USBDEVICELOCATOR_H */
