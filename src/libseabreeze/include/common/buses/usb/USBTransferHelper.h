/***************************************************//**
 * @file    USBTransferHelper.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is a TransferHelper intended for USB communications.
 * Each USBTransferHelper must specify a sending and
 * receiving endpoint, which will tend to vary according to
 * the type of data transfer being conducted.  This adapts
 * the send() and receive() methods required of a TransferHelper
 * according to a particular type of transfer, which may be
 * inferred from a ProtocolHint.
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

#ifndef USBTRANSFERHELPER_H
#define USBTRANSFERHELPER_H

#include "common/buses/TransferHelper.h"
#include "native/usb/USB.h"

namespace seabreeze {

    class USBTransferHelper : public TransferHelper {
    public:
        USBTransferHelper(USB *usbDescriptor, int sendEndpoint,
                int receiveEndpoint);
        USBTransferHelper(USB *usbDescriptor);
        virtual ~USBTransferHelper();

        virtual int receive(std::vector<byte> &buffer, unsigned int length)
            throw (BusTransferException);
        virtual int send(const std::vector<byte> &buffer, unsigned int length) const
            throw (BusTransferException);

    protected:
        USB *usb;
        int sendEndpoint;
        int receiveEndpoint;
    };

}

#endif /* USBTRANSFERHELPER_H */
