/***************************************************//**
 * @file    DeviceLocatorInterface.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * DeviceLocatorInterface provides a base interface for
 * classes that allow the location of a device to be
 * specified in a bus-specific way.  For instance, a
 * USB DeviceLocator might include a device path or
 * index, and a socket DeviceLocator might include an
 * IP address and port number.  This allows
 * devices that cannot be identified by probing to still
 * be found easily.
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

#ifndef DEVICELOCATORINTERFACE_H
#define DEVICELOCATORINTERFACE_H

#include <string>
#include "common/buses/BusFamily.h"

namespace seabreeze {

    class DeviceLocatorInterface {
    public:
        virtual ~DeviceLocatorInterface() = 0;

        /**
         * Get a unique identifier for this location.  This can be any value
         * as long as it is globally unique.
         */
        virtual unsigned long getUniqueLocation() const = 0;

        /**
         * Determine whether this DeviceLocator refers to the same device
         * as another.
         */
        virtual bool equals(DeviceLocatorInterface &that) = 0;

        /**
         * Get a human-readable string that describes the location
         */
        virtual std::string getDescription() = 0;

        /**
         * Get a description of the type of bus that the device is associated with
         */
        virtual BusFamily getBusFamily() const = 0;

        /* Get an exact copy of this instance */
        virtual DeviceLocatorInterface *clone() const = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline DeviceLocatorInterface::~DeviceLocatorInterface() {}

}

#endif /* DEVICELOCATORINTERFACE_H */
