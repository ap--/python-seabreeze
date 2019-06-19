/***************************************************//**
 * @file    RawUSBBusAccessFeature.cpp
 * @date    October 2012
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
#include "vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeature.h"
#include "common/buses/usb/USBTransferHelper.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using std::vector;
using std::string;

RawUSBBusAccessFeature::RawUSBBusAccessFeature() {

}

RawUSBBusAccessFeature::~RawUSBBusAccessFeature() {

}

vector<byte> RawUSBBusAccessFeature::readUSB(const USBInterface *bus, int endpoint,
        unsigned int length) throw (FeatureException) {

    vector<byte> retval(length);
    USB* descriptor = bus->getUSBDescriptor();

    try {
        USBTransferHelper helper(descriptor, 0x00, endpoint);
        helper.receive(retval, length);
    } catch (BusTransferException &e) {
        string error("Caught BusTransferException in readUSB: ");
        error += e.what();
        throw FeatureControlException(error);
    }

    return retval;
}

int RawUSBBusAccessFeature::writeUSB(const USBInterface *bus, int endpoint,
        const vector<byte> &data) throw (FeatureException) {

    int bytesWritten = 0;

    USB* descriptor = bus->getUSBDescriptor();
    try {
        USBTransferHelper helper(descriptor, endpoint, 0x00);
        helper.send(data, (unsigned int) data.size());
        bytesWritten = (int) data.size();
    } catch (BusTransferException &e) {
        string error("Caught BusTransferException in writeUSB: ");
        error += e.what();
        throw FeatureControlException(error);
    }

    return bytesWritten;
}

FeatureFamily RawUSBBusAccessFeature::getFeatureFamily() {
    FeatureFamilies families;
    return families.RAW_USB_BUS_ACCESS;
}
