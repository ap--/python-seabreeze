/***************************************************//**
 * @file    SparkUSB.cpp
 * @date    January 2015
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
#include "vendors/OceanOptics/buses/usb/SparkUSB.h"
#include "vendors/OceanOptics/buses/usb/OOIUSBProductID.h"
#include "vendors/OceanOptics/buses/usb/OOIUSBEndpointMaps.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPControlHint.h"
#include "vendors/OceanOptics/protocols/obp/hints/OBPSpectrumHint.h"
#include "vendors/OceanOptics/buses/usb/OOIUSBTrivialTransferHelper.h"

using namespace seabreeze;
using namespace oceanBinaryProtocol;

SparkUSB::SparkUSB() {
    this->productID = SPARK_USB_PID;
}

SparkUSB::~SparkUSB() {

}

bool SparkUSB::open() {
    bool retval = false;

    retval = OOIUSBInterface::open();

    if(true == retval) {
        OBPControlHint *controlHint = new OBPControlHint();
        OBPSpectrumHint *spectrumHint = new OBPSpectrumHint();
        OOIUSBSimpleDualEndpointMap epMap;

        clearHelpers();

        /* On the Spark, endpoints 1 and 2 are both bidirectional
         * and equivalent.  This sets all typical transfer types
         * to the same endpoint.  If some special commands need to
         * be passed through, such as a reset or software-simulated
         * trigger, then a different helper could be used for those.
         */
        addHelper(spectrumHint, new OOIUSBTrivialTransferHelper(
            (this->usb), epMap));
        addHelper(controlHint, new OOIUSBTrivialTransferHelper(
            (this->usb), epMap));
    }

    return retval;
}
