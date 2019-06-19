/***************************************************//**
 * @file    USBDiscovery.cpp
 * @date    February 2012
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

/* Constants */
#define __MAX_USB_DEVICES   127

#include "common/globals.h"
#include "native/usb/USBDiscovery.h"
#include <stdlib.h>

using namespace seabreeze;
using namespace std;

USBDiscovery::USBDiscovery() {

}

USBDiscovery::~USBDiscovery() {

}

vector<unsigned long> *USBDiscovery::probeDevices(int vendorID, int productID) {
    int deviceCount;
    unsigned long *deviceList;
    vector<unsigned long> *retval;
    int i;

    deviceList = (unsigned long *)calloc(__MAX_USB_DEVICES, sizeof(unsigned long));

    deviceCount = USBProbeDevices(vendorID, productID, deviceList,
        __MAX_USB_DEVICES);

    if(deviceCount < 0) {
        /* This masks an error, but the effect is to return an empty vector */
        deviceCount = 0;
    }

    retval = new vector<unsigned long>(deviceCount);
    vector<unsigned long>::iterator iter;
    for(i = 0, iter = retval->begin(); i < deviceCount && iter != retval->end(); i++, iter++) {
        *iter = deviceList[i];
    }

    free(deviceList);

    return retval;
}

USB *USBDiscovery::createUSBInterface(unsigned long deviceID) {
    /* Create a USB instance with the given deviceID.  This constructor for
     * USB is protected, so this class uses a friend relationship to get
     * access.
     */
    USB *retval = new USB(deviceID);

    return retval;
}
