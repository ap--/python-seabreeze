/***************************************************//**
 * @file    OOIUSBInterface.h
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

#ifndef OOIUSBINTERFACE_H
#define OOIUSBINTERFACE_H

#include "common/buses/usb/USBInterface.h"
#include "common/protocols/ProtocolHint.h"
#include "common/buses/TransferHelper.h"
#include "common/buses/DeviceLocationProberInterface.h"

#define OCEAN_OPTICS_USB_VID 0x2457

namespace seabreeze {

    class OOIUSBInterface : public USBInterface, public DeviceLocationProberInterface {
    public:
        OOIUSBInterface();
        virtual ~OOIUSBInterface();

        int getProductID();
        int getVendorID();

        /* Inherited from Bus */
        virtual TransferHelper *getHelper(const std::vector<ProtocolHint *> &hints) const;
        virtual void setLocation(const DeviceLocatorInterface &location) throw (IllegalArgumentException);
        virtual bool open();
        virtual void close();

        /* Inherited from DeviceLocationProberInterface */
        virtual std::vector<DeviceLocatorInterface *> *probeDevices();

    protected:
        void addHelper(ProtocolHint *hint, TransferHelper *helper);
        void clearHelpers();

        int vendorID;
        int productID;

        /* These vectors should really be in a map, but that didn't want to
         * work easily.  Since there will likely be about 2 entries in here,
         * storing in a pair of vectors for now won't hurt anything.
         */
        std::vector<ProtocolHint *> helperKeys;
        std::vector<TransferHelper *> helperValues;
    };

}

#endif /* OOIUSBINTERFACE_H */
