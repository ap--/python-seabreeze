/***************************************************//**
 * @file    USBTransferHelper.cpp
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

#include "common/globals.h"
#include "common/buses/usb/USBTransferHelper.h"
#include <string>

using namespace seabreeze;
using namespace std;

USBTransferHelper::USBTransferHelper(USB *usbDescriptor, int sendEndpoint,
        int receiveEndpoint) : TransferHelper() {
    this->usb = usbDescriptor;
    this->sendEndpoint = sendEndpoint;
    this->receiveEndpoint = receiveEndpoint;
}

USBTransferHelper::USBTransferHelper(USB *usbDescriptor) : TransferHelper() {
    this->usb = usbDescriptor;
}

USBTransferHelper::~USBTransferHelper() {

}

int USBTransferHelper::receive(vector<unsigned char> &buffer, unsigned int length) {
    int retval = 0;

    retval = this->usb->read(this->receiveEndpoint, (void *)&(buffer[0]), length);

    if((0 == retval && length > 0) || (retval < 0)) {
        string error("Failed to read any data from USB.");
        throw BusTransferException(error);
    }

    return retval;
}

int USBTransferHelper::send(const vector<unsigned char> &buffer, unsigned int length) const {
    int retval = 0;

    retval = this->usb->write(this->sendEndpoint, (void *)&(buffer[0]), length);

    if((0 == retval && length > 0) || (retval < 0)) {
        string error("Failed to write any data to USB.");
        throw BusTransferException(error);
    }

    return retval;
}
