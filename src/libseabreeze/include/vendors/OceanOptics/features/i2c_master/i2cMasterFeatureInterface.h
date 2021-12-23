/***************************************************//**
 * @file    i2cMasterFeatureInterface.h
 * @date    May 2017
 * @author  Ocean Optics, Inc.
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

#ifndef I2CMASTERFEATUREINTERFACE_H
#define I2CMASTERFEATUREINTERFACE_H

#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    class i2cMasterFeatureInterface {
    public:
        virtual ~i2cMasterFeatureInterface() = 0;

		virtual unsigned char i2cMasterGetNumberOfBuses(
			const Protocol &protocol,
			const Bus &bus) = 0;

		virtual std::vector<unsigned char> i2cMasterReadBus(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char busIndex,
			unsigned char slaveAddress,
			unsigned short numberOfBytes) = 0;

		virtual unsigned short i2cMasterWriteBus(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char busIndex,
			unsigned char slaveAddress,
			const std::vector<unsigned char> writeData) = 0;
    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline i2cMasterFeatureInterface::~i2cMasterFeatureInterface() {}
}

#endif /* I2CMASTERFEATUREINTERFACE_H */
