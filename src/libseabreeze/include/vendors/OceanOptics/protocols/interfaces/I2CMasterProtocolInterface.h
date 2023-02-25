/***************************************************//**
 * @file    I2CMasterProtocolInterface.h
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a generic interface into the I2C Master
 * functionality at the protocol level, agnostic to
 * any particular protocol.  Each Protocol offering this
 * functionality should implement this interface.
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

#ifndef I2CMASTERPROTOCOLINTERFACE_H
#define I2CMASTERPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class I2CMasterProtocolInterface : public ProtocolHelper {
    public:
		I2CMasterProtocolInterface(Protocol *protocol);
        virtual ~I2CMasterProtocolInterface();


	    /**
         * Get the number of I2C busses.
         */
        virtual unsigned char i2cMasterGetNumberOfBuses(const Bus &bus) = 0;

        /**
         * Read data from the I2C bus
         */
        virtual std::vector<unsigned char> i2cMasterReadBus(const Bus &bus, unsigned char busIndex, unsigned char slaveAddress, unsigned short numberOfBytes) = 0;

        /**
         *  write data to the I2C bus
         */
        virtual unsigned short i2cMasterWriteBus(const Bus &bus, unsigned char busIndex, unsigned char slaveAddress, const std::vector<unsigned char> writeData) = 0;


    };

}

#endif /* I2CMASTERPROTOCOLINTERFACE_H */
