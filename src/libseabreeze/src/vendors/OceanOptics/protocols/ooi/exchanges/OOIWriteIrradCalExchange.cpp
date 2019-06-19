/***************************************************//**
 * @file    OOIWriteIrradCalExchange.cpp
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

#include "common/globals.h"
#include <vector>
#include "vendors/OceanOptics/protocols/ooi/exchanges/OOIWriteIrradCalExchange.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "common/UShortVector.h"
#include "common/exceptions/ProtocolFormatException.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"
#include "common/ByteVector.h"
#include "common/protocols/Transfer.h"
#include "native/system/System.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

#define BLOCK_TRANSFER_SIZE 60

OOIWriteIrradCalExchange::OOIWriteIrradCalExchange(int numberOfPixels)
        : OOIIrradCalExchange(numberOfPixels) {

}

int OOIWriteIrradCalExchange::setCalibration(const vector<float> &cal) {
    /* EEPROM Address from which we shall read, start at zero. */
    unsigned int addr;
    unsigned int i;
    unsigned int factorIndex;
    unsigned int *fptr;

    /* Number of calibration bytes that will be read.  Take the minimum of the
     * number of pixels and the length of the provided calibration.
     */
    unsigned int factors = (cal.size() < this->numberOfPixels) ? (unsigned) cal.size() : this->numberOfPixels;
    if(0 == factors) {
        /* Bail out before any side effects occur */
        return 0;
    }

    /* Wipe away any previous calibration information */
    this->transfers.resize(0);

    for(    addr = 0, factorIndex = 0;
            addr < 65536-BLOCK_TRANSFER_SIZE && factorIndex < factors;
            addr += BLOCK_TRANSFER_SIZE) {
        /* create vector of ProtocolHints for requests */
        vector<ProtocolHint *> *requestHints = new vector<ProtocolHint *>;

        /* create buffer for holding the bytes of the request */
        vector<byte> *requestBuffer = new vector<byte>;

        /* resize the request buffer to hold a request */
        requestBuffer->resize(BLOCK_TRANSFER_SIZE + 3);

        /* Each Transfer needs own hints.  Transfer will destroy own hint
         * vector when Transfer destroys self
         */
        requestHints->push_back(new ControlHint());

        (*(requestBuffer))[0] = OpCodes::OP_WRITE_IRRAD_CAL;
        (*(requestBuffer))[1] = (byte)( (addr) & 0x00FF);
        (*(requestBuffer))[2] = (byte)((addr >> 8) & 0x00FF);
        for(i = 0; i < BLOCK_TRANSFER_SIZE && factorIndex < factors; i += 4, factorIndex++) {
            fptr = (unsigned int *)&cal[factorIndex];
            /* Convert the floats to a byte array, MSB first */
            (*(requestBuffer))[i + 3] = ((*fptr) >> 24) & 0x00FF;
            (*(requestBuffer))[i + 4] = ((*fptr) >> 16) & 0x00FF;
            (*(requestBuffer))[i + 5] = ((*fptr) >> 8) & 0x00FF;
            (*(requestBuffer))[i + 6] = (*fptr) & 0x00FF;
        }

        Transfer *request = new Transfer(requestHints,
            requestBuffer, Transfer::TO_DEVICE, (unsigned) requestBuffer->size());

        addTransfer(request);
    }

    return factors;
}

Data *OOIWriteIrradCalExchange::transfer(TransferHelper *helper)
        throw (ProtocolException) {
    vector<Transfer *>::iterator iter = this->transfers.begin();

    /* Iterate over all stored transfers and delegate to the helper to
     * move the data.
     */
    for(iter = this->transfers.begin(); iter != this->transfers.end(); iter++) {
        /* Note that this may throw a ProtocolException which will not be caught here. */
        (*iter)->transfer(helper);

        /* Delay 200ms before continuing so that the EEPROM can finish writing */
        System::sleepMilliseconds(200);
    }

    /* return concatenated data as ByteVector */
    return NULL;
}

OOIWriteIrradCalExchange::~OOIWriteIrradCalExchange() {

}
