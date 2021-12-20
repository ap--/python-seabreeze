/***************************************************//**
 * @file    RS232TransferHelper.h
 * @date    April 2011
 * @author  Ocean Optics, Inc.
 *
 * This provides an abstraction around the RS232 bus.
 * RS232 is pretty simple once the port is opened and
 * configured, so this mostly just takes care of ensuring
 * that all bytes are sent and received as required.
 *
 * This will effectively block on reads and writes until
 * they are complete.  A non-blocking transfer helper
 * could be created to complement this if there was
 * some need, but it is not clear who would be responsible
 * for delayed reads or retransmits.
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

#ifndef RS232TRANSFERHELPER_H
#define RS232TRANSFERHELPER_H

#include "common/buses/TransferHelper.h"
#include "native/rs232/RS232.h"

namespace seabreeze {

    class RS232TransferHelper : public TransferHelper {
    public:
        RS232TransferHelper(RS232 *rs232Descriptor);
        virtual ~RS232TransferHelper();

        virtual int receive(std::vector<unsigned char> &buffer, unsigned int length);
        virtual int send(const std::vector<unsigned char> &buffer, unsigned int length) const;

    protected:
        RS232 *rs232;
    };

}

#endif /* USBTRANSFERHELPER_H */
