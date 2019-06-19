/***************************************************//**
 * @file    RS232Interface.h
 * @date    April 2011
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

#ifndef RS232INTERFACE_H
#define RS232INTERFACE_H

#include "common/buses/Bus.h"
#include "native/rs232/NativeRS232.h"
#include "native/rs232/RS232.h"
#include "common/exceptions/IllegalArgumentException.h"

namespace seabreeze {

    class RS232Interface : public Bus {
    public:
        RS232Interface();
        virtual ~RS232Interface();
        virtual RS232 *getRS232Descriptor();
        virtual DeviceLocatorInterface *getLocation();
        virtual void setLocation(const DeviceLocatorInterface &location)
                throw (IllegalArgumentException);
        virtual BusFamily getBusFamily() const;
        
        /* Pure virtual methods */
        virtual TransferHelper *getHelper(
                const std::vector<ProtocolHint *> &hints) const = 0;
        virtual bool open() = 0;
        virtual void close() = 0;

    protected:
        RS232 *rs232;
        DeviceLocatorInterface *deviceLocator;
    };

}

#endif /* RS232INTERFACE_H */
