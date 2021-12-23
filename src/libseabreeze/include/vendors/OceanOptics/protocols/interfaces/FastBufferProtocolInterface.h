/***************************************************//**
 * @file    FastBufferProtocolInterface.h
 * @date    February 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a generic interface into fast buffer functionality
 * at the protocol level, agnostic to any particular protocol.
 * Each Protocol offering this functionality should implement
 * this interface.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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

#ifndef FASTBUFFERPROTOCOLINTERFACE_H
#define FASTBUFFERPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class FastBufferProtocolInterface : public ProtocolHelper {
    public:
        FastBufferProtocolInterface(Protocol *protocol);
        virtual ~FastBufferProtocolInterface();

		virtual unsigned char getBufferingEnable(const Bus &bus,
                unsigned char bufferIndex) = 0;

        virtual void setBufferingEnable(const Bus &bus,
                unsigned char bufferIndex,
                const unsigned char isEnabled) = 0;

		virtual unsigned int getConsecutiveSampleCount(const Bus &bus,
			unsigned char bufferIndex) = 0;

		virtual void setConsecutiveSampleCount(const Bus &bus,
			unsigned char bufferIndex,
			const unsigned int consecutiveSampleCount) = 0;
    };

} /* end namespace */

#endif /*  DATABUFFERPROTOCOLINTERFACE_H */
