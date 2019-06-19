/***************************************************//**
 * @file    USBInterface.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is an abstract base class intended to be an interface
 * for USB control objects.  This allows USB devices to
 * be opened generically (just by providing the index of
 * the device on the bus) without any concern for
 * the vendor ID, product ID, or underlying USB implementation.
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

#ifndef USBINTERFACE_H
#define USBINTERFACE_H

#include "common/buses/Bus.h"
#include "native/usb/NativeUSB.h"
#include "native/usb/USB.h"
#include "common/exceptions/IllegalArgumentException.h"

namespace seabreeze {

    class USBInterface : public Bus {
    public:
        USBInterface();
        virtual ~USBInterface();
        virtual USB *getUSBDescriptor() const;
        virtual DeviceLocatorInterface *getLocation();
        virtual void setLocation(const DeviceLocatorInterface &location) throw (IllegalArgumentException);
        virtual BusFamily getBusFamily() const;
        virtual bool open() = 0;
        virtual void close() = 0;

    protected:
        USB *usb;
        DeviceLocatorInterface *deviceLocator;
    };

}

#endif /* USBINTERFACE_H */
