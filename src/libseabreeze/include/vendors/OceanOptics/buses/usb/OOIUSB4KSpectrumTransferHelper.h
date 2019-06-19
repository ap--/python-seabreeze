/***************************************************//**
 * @file    OOIUSB4KSpectrumTransferHelper.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This class encapsulates the behavior of the USB4000 and HR4000
 * in the case where they are connected via a USB2.0 bus.  For the
 * case where the device is connected via USB 1.1, then the
 * OOIUSBSpectrumTransferHelper should be used instead.
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

#ifndef OOIUSB4KSPECTRUMTRANSFERHELPER_H
#define OOIUSB4KSPECTRUMTRANSFERHELPER_H

#include "common/buses/usb/USBTransferHelper.h"
#include "vendors/OceanOptics/buses/usb/OOIUSBEndpointMaps.h"

namespace seabreeze {

    class OOIUSB4KSpectrumTransferHelper : public USBTransferHelper {
    public:
        OOIUSB4KSpectrumTransferHelper(USB *usb,
            const OOIUSBCypressEndpointMap &map);
        virtual ~OOIUSB4KSpectrumTransferHelper();

        /* Inherited */
        virtual int receive(std::vector<byte> &buffer, unsigned int length)
            throw (BusTransferException);

    private:
        int secondaryHighSpeedEP;
        std::vector<byte> primaryReadBuffer;
        std::vector<byte> secondaryReadBuffer;
    };

}

#endif /* OOIUSB4KSPECTRUMTRANSFERHELPER_H */
