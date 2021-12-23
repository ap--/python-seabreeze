/***************************************************//**
 * @file    I2CMasterFeatureAdapter.h
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze I2CMasterFeatureInterface instances.
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

#ifndef SEABREEZE_I2CMASTERFEATUREADAPTER_H
#define SEABREEZE_I2CMASTERFEATUREADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/i2c_master/i2cMasterFeatureInterface.h"

namespace seabreeze
{
    namespace api
    {

        class I2CMasterFeatureAdapter
                : public FeatureAdapterTemplate<i2cMasterFeatureInterface>
		{
        	public:
				I2CMasterFeatureAdapter(i2cMasterFeatureInterface *intf, const FeatureFamily &f, Protocol *p, Bus *b, unsigned short instanceIndex);
            	virtual ~I2CMasterFeatureAdapter();

				unsigned char i2cMasterGetNumberOfBuses(int *errorCode);
				unsigned short i2cMasterReadBus(int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes);
				unsigned short i2cMasterWriteBus(int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes);
        };

    }
}

#endif //  SEABREEZE_I2CMASTERFEATUREADAPTER_H
