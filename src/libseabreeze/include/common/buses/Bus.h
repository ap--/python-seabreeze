/***************************************************//**
 * @file    Bus.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This provides a base class for all sorts of buses.  A bus
 * is a mechanism for transferring a stream of data from one
 * point to another.  The bus does not concern itself with the
 * contents of the data stream.  At most, it may use hints to
 * determine how a particular message will be moved if this
 * is necessary to complete the operation.
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

#ifndef SEABREEZE_BUS_H
#define SEABREEZE_BUS_H

#include "common/protocols/ProtocolHint.h"
#include "common/buses/TransferHelper.h"
#include "common/buses/BusFamily.h"
#include "common/buses/DeviceLocatorInterface.h"
#include "common/exceptions/IllegalArgumentException.h"

namespace seabreeze {

    class Bus {
    public:
        Bus();
        virtual ~Bus();
        virtual TransferHelper *getHelper(const std::vector<ProtocolHint *> &hints) const = 0;
        virtual BusFamily getBusFamily() const = 0;
        /* Associate this Bus instance with a particular device location.
         * This MUST be done before open or close can be used.
         */
        virtual void setLocation(const DeviceLocatorInterface &location)
                throw (IllegalArgumentException) = 0;
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual DeviceLocatorInterface *getLocation() = 0;
    };

}

#endif
