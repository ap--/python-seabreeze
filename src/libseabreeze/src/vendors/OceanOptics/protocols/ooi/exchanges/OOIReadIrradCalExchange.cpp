/***************************************************//**
 * @file    OOIReadIrradCalExchange.cpp
 * @date    March 2010
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

#include "vendors/OceanOptics/protocols/ooi/exchanges/OOIReadIrradCalExchange.h"
#include "vendors/OceanOptics/protocols/ooi/hints/ControlHint.h"
#include "common/UShortVector.h"
#include "common/exceptions/ProtocolFormatException.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"
#include "common/ByteVector.h"
#include "common/protocols/Transfer.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

#define BLOCK_TRANSFER_SIZE 60

OOIReadIrradCalExchange::OOIReadIrradCalExchange(int numberOfPixels)
        : OOIIrradCalExchange(numberOfPixels) {

    /* EEPROM Address from which we shall read, start at zero. */
    unsigned int addr;
    /* Number of calibration bytes that will be read */
    int bytesLeft = this->numberOfPixels * sizeof(float);

    for(addr = 0; bytesLeft > 0 && addr < 65536-BLOCK_TRANSFER_SIZE;
                addr += BLOCK_TRANSFER_SIZE, bytesLeft -= BLOCK_TRANSFER_SIZE) {
        /* create vector of ProtocolHints for requests */
        vector<ProtocolHint *> *requestHints = new vector<ProtocolHint *>;

        /* create vector of ProtocolHints for responses */
        vector<ProtocolHint *> *responseHints = new vector<ProtocolHint *>;

        /* create buffer for bytes of response */
        vector<unsigned char> *responseBuffer = new vector<unsigned char>;

        /* resize the response buffer to hold 60 bytes */
        responseBuffer->resize(BLOCK_TRANSFER_SIZE);

        /* create buffer for holding the bytes of the request */
        vector<unsigned char> *requestBuffer = new vector<unsigned char>;

        /* resize the request buffer to hold a request */
        requestBuffer->resize(3);

        /* Each Transfer needs own hints.  Transfer will destroy own hint
         * vector when Transfer destroys self
         */
        requestHints->push_back(new ControlHint());
        responseHints->push_back(new ControlHint());

        (*(requestBuffer))[0] = OpCodes::OP_READ_IRRAD_CAL;
        (*(requestBuffer))[1] = (unsigned char)( (addr) & 0x00FF);
        (*(requestBuffer))[2] = (unsigned char)((addr>>8) & 0x00FF);

        Transfer *request = new Transfer(requestHints,
            requestBuffer, Transfer::TO_DEVICE, 3);

        Transfer *response = new Transfer(responseHints,
            responseBuffer, Transfer::FROM_DEVICE, BLOCK_TRANSFER_SIZE);

        addTransfer(request);
        addTransfer(response);
    }
}

Data *OOIReadIrradCalExchange::transfer(TransferHelper *helper) {
    Data *xfer;
    ByteVector *output = new ByteVector();
    vector<Transfer *>::iterator iter = this->transfers.begin();
    /* Number of calibration bytes that will be read */
    int bytesLeft = this->numberOfPixels * sizeof(float);

    /* Iterate over all stored transfers and delegate to the helper to
     * move the data.
     */
    for(iter = this->transfers.begin(); iter != this->transfers.end(); iter++) {
        /* Note that this may throw a ProtocolException which will not be caught here. */
        /* Either send request for data, or obtain data.
         * xfer will be NULL if request being issued.
         * data is returned as ByteVector type object if data being returned.
         */
        xfer = (*iter)->transfer(helper);

        if(NULL != xfer) {
            /* transfer block of 60 bytes from xfer to output */
            for(    unsigned int i = 0;
                    i < ((ByteVector*)xfer)->getByteVector().size()
                        && bytesLeft > 0;
                    i++, bytesLeft--) {
                /* FIXME: can this be done more efficiently using a memcpy()? */
                output->getByteVector().push_back(
                    ((ByteVector*)xfer)->getByteVector()[i]);
            }
            delete xfer; /* clear for next iteration */
        }
    }

    /* return concatenated data as ByteVector */
    return output;
}

OOIReadIrradCalExchange::~OOIReadIrradCalExchange() {

}
