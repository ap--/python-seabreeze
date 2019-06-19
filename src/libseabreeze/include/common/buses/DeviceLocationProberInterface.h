/***************************************************//**
 * @file    DeviceLocationProberInterface.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * DeviceLocatorInterface provides a base interface for
 * classes that allow the location of a device to be
 * probed in a bus-specific way.
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

#ifndef DEVICELOCATIONPROBERINTERFACE_H
#define DEVICELOCATIONPROBERINTERFACE_H

#include <vector>
#include "common/buses/DeviceLocatorInterface.h"

namespace seabreeze {

    class DeviceLocationProberInterface {
    public:
        virtual ~DeviceLocationProberInterface() = 0;

        /* Report how many devices of this type are available */
        virtual std::vector<DeviceLocatorInterface *> *probeDevices() = 0;

    protected:
        DeviceLocationProberInterface();
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline DeviceLocationProberInterface::~DeviceLocationProberInterface() {}

}

#endif /* DEVICELOCATIONPROBERINTERFACE_H */
