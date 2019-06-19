/***************************************************//**
 * @file    RS232TransferHelper.cpp
 * @date    April 2011
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
#include "common/buses/rs232/RS232TransferHelper.h"
#include "native/system/System.h"
#include <string>

using namespace seabreeze;
using namespace std;

RS232TransferHelper::RS232TransferHelper(RS232 *rs232Descriptor) : TransferHelper() {
    this->rs232 = rs232Descriptor;
}

RS232TransferHelper::~RS232TransferHelper() {

}

int RS232TransferHelper::receive(vector<byte> &buffer, unsigned int length)
        throw (BusTransferException) {
    int retval = 0;
    unsigned int bytesRead = 0;

    while(bytesRead < length) {
        retval = this->rs232->read((void *)&(buffer[bytesRead]), length - bytesRead);
        if(retval < 0) {
            string error("Failed to read any data from RS232.");
            throw BusTransferException(error);
        } else if(retval != 0) {
            bytesRead += retval;
        } else {
            /* Not enough data available to satisfy the request.  Wait for more
             * data to arrive.
             */
            System::sleepMilliseconds(10);
        }
    }

    return bytesRead;
}

int RS232TransferHelper::send(const vector<byte> &buffer, unsigned int length) const
        throw (BusTransferException) {
    int retval = 0;
    unsigned int bytesWritten = 0;

    while(bytesWritten < length) {
        retval = this->rs232->write((void *)&(buffer[bytesWritten]), length - bytesWritten);
        if(retval < 0) {
            string error("Failed to write any data to RS232.");
            throw BusTransferException(error);
        } else if(retval != 0) {
            bytesWritten += retval;
        } else {
            /* Output buffer is probably full.  Wait for some of the bytes to
             * be transferred before trying again.
             */
            System::sleepMilliseconds(10);
        }
    }

    return bytesWritten;
}
