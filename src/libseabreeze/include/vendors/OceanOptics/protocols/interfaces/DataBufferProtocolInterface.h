/***************************************************//**
 * @file    DataBufferProtocolInterface.h
 * @date    October 2015
 * @author  Ocean Optics, Inc.
 *
 * This is a generic interface into buffer functionality
 * at the protocol level, agnostic to any particular protocol.
 * Each Protocol offering this functionality should implement
 * this interface.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc
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

#ifndef DATABUFFERPROTOCOLINTERFACE_H
#define DATABUFFERPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class DataBufferProtocolInterface : public ProtocolHelper {
    public:
        DataBufferProtocolInterface(Protocol *protocol);
        virtual ~DataBufferProtocolInterface();

        virtual void clearBuffer(const Bus &bus, unsigned char bufferIndex) = 0;

        virtual void removeOldestSpectraFromBuffer(const Bus &bus, unsigned char bufferIndex, unsigned int numberOfSpectra) = 0;

        virtual unsigned long getNumberOfElements(const Bus &bus,
                unsigned char bufferIndex) = 0;

        virtual unsigned long getBufferCapacity(const Bus &bus,
                unsigned char bufferIndex) = 0;

        virtual unsigned long getBufferCapacityMinimum(const Bus &bus,
                unsigned char bufferIndex) = 0;

        virtual unsigned long getBufferCapacityMaximum(const Bus &bus,
                unsigned char bufferIndex) = 0;

        virtual void setBufferCapacity(const Bus &bus,
                unsigned char bufferIndex,
                const unsigned long capacity) = 0;

    };

} /* end namespace */

#endif /*  DATABUFFERPROTOCOLINTERFACE_H */
