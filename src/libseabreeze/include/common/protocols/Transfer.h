/***************************************************//**
 * @file    Transfer.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * The Transfer class captures a simplex
 * data transfer to or from a device.  At this
 * level, there is no notion of a particular bus,
 * just data, length, and direction.
 *
 * The bus aspects are encapsulated in a "helper"
 * object that must be provided when the Transfer
 * is executed.  The helper must provide send() and
 * receive() methods taking a buffer and length
 * only.  All of the details in getting the transfer to
 * or from the other end must be handled by the helper.
 * The helper is expected to be created by some aspect
 * of the driver for the device in question.  It contains
 * information about the connection and the particulars
 * about routing data in and out.
 *
 * Note that the Transfer class is completely orthagonal to any
 * particular protocol.  It is expected that a protocol may be
 * built up as a collection of related Transfer types.
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

#ifndef SEABREEZE_TRANSFER_H
#define SEABREEZE_TRANSFER_H

#include <vector>
#include "common/protocols/Exchange.h"
#include "common/protocols/ProtocolHint.h"
#include "common/Data.h"

typedef unsigned int direction_t;

namespace seabreeze {

    class Transfer : public Exchange {
    public:

        /* Note that the size of the provided buffer and the specified length
         * of the transfer itself do not need to agree.  If the transfer requires
         * more space than the buffer provides, the buffer will be resized.
         * If the buffer is created larger than is needed, only the given length
         * will be sent or received.  This allows for some freedom in buffer
         * management.
         */
        Transfer(std::vector<ProtocolHint *> *hints, std::vector<unsigned char> *buffer,
                direction_t direction, unsigned int length);

        virtual ~Transfer();
        virtual Data *transfer(TransferHelper *helper);

        static const direction_t TO_DEVICE;
        static const direction_t FROM_DEVICE;

		// workaround for the FlameX get buffered spectrum call, which unlike all of the rest of the get spectrum calls
		//  for all of the other spectrometers, has an argument. Since the request spectrum exchanges are stored as Transfer objects
		//  in the spectrometer protocol when it is first created, and the message content is already defined, then later, when the transfer method is
		//  called, there is no way to change the immediate data in the predefined message. This gives a pointer to the derived class and
		//  provides an interface to a static function, within the derived class, set the immediate data of the message.

		void * derivedClassPointer;
		void (*setParametersFunction)(void *, unsigned int);

    protected:
        Transfer();
        void checkBufferSize();

        unsigned int length;
        std::vector<unsigned char> *buffer;
        direction_t direction;

    };

}

#endif /* SEABREEZE_TRANSFER_H */
