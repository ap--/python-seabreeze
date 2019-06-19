/***************************************************//**
 * @file    OOIUSBInterface.cpp
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
#include "vendors/OceanOptics/buses/usb/OOIUSBInterface.h"
#include "native/usb/USBDiscovery.h"
#include "common/buses/usb/USBDeviceLocator.h"
#include "common/exceptions/IllegalArgumentException.h"

using namespace seabreeze;
using namespace std;

OOIUSBInterface::OOIUSBInterface() {
    /* Ocean Optics vendor ID is 0x2457. */
    this->vendorID = OCEAN_OPTICS_USB_VID;

    /* Subclass will set the PID.  This is perhaps an inefficient use of
     * subclasses, so if that offends, this can be restructured to just
     * take the PID as an argument.
     */

    this->usb = NULL;
}

OOIUSBInterface::~OOIUSBInterface() {
    if(NULL != this->usb) {
        if(true == this->usb->isOpened()) {
            this->usb->close();
        }
        delete this->usb;
    }

    vector<ProtocolHint *>::iterator keyIter;
    vector<TransferHelper *>::iterator valueIter;

    for(keyIter = this->helperKeys.begin(); keyIter != this->helperKeys.end(); keyIter++) {
        delete *keyIter;
    }

    for(valueIter = this->helperValues.begin(); valueIter != this->helperValues.end(); valueIter++) {
        delete *valueIter;
    }
}

int OOIUSBInterface::getProductID() {
    return this->productID;
}

int OOIUSBInterface::getVendorID() {
    return this->vendorID;
}

vector<DeviceLocatorInterface *> *OOIUSBInterface::probeDevices() {
    vector<DeviceLocatorInterface *> *retval;
    USBDiscovery discovery;
    vector<unsigned long> *ids;
    vector<unsigned long>::iterator longIter;

    retval = new vector<DeviceLocatorInterface *>;

    ids = discovery.probeDevices(this->getVendorID(), this->getProductID());
    if(NULL != ids) {
        /* Iterate over the IDs that were just probed and create a new
         * USB device locator for each.  Note that these are specific to
         * the particular VID and PID associated with this device interface.
         */
        for(longIter = ids->begin(); longIter != ids->end(); longIter++) {
            retval->push_back(new USBDeviceLocator(*longIter));
        }

        delete ids;
    }

    return retval;
}

void OOIUSBInterface::setLocation(const DeviceLocatorInterface &location)
        throw (IllegalArgumentException) {
    USBDiscovery discovery;

    BusFamily family = location.getBusFamily();
    if(false == family.equals(this->getBusFamily())) {
        string cause("Invalid bus on device locator.");
        throw IllegalArgumentException(cause);
    }

    if(NULL != this->usb) {
        delete this->usb;
    }

    this->usb = discovery.createUSBInterface(location.getUniqueLocation());

    /* Delegate to the base class to copy the location instance */
    USBInterface::setLocation(location);
}

bool OOIUSBInterface::open() {
    bool flag = false;

    if(NULL == this->usb) {
        /* This instance does not point to anything that can be opened. */
        return false;
    }

    /* It looks like this is a valid location, so try to open it */
    flag = this->usb->open();
    return flag;
}

void OOIUSBInterface::close() {
    this->usb->close();
}

void OOIUSBInterface::addHelper(ProtocolHint *hint, TransferHelper *helper) {
    this->helperKeys.push_back(hint);
    this->helperValues.push_back(helper);
}

void OOIUSBInterface::clearHelpers() {
    for(unsigned int i = 0; i < this->helperKeys.size(); i++) {
        delete this->helperKeys[i];
        delete this->helperValues[i];
    }
    this->helperKeys.resize(0);
    this->helperValues.resize(0);
}

TransferHelper *OOIUSBInterface::getHelper(const vector<ProtocolHint *> &hints) const {
    /* Just grab the first hint and use that to look up a helper.
     * The helpers for Ocean Optics USB devices are 1:1 with respect to hints.
     */

    /* Note: this should really be done with a map or hashmap, but I am just not able
     * to get that to work (it was returning bad values).  Feel free to reimplement
     * this in a cleaner fashion.
     */
    unsigned int i;
    for(i = 0; i < this->helperKeys.size(); i++) {
        if((*(this->helperKeys[i])) == (*hints[0])) {
            return this->helperValues[i];
        }
    }
    return NULL;
}
