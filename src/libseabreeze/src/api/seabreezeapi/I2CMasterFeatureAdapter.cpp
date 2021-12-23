/***************************************************//**
 * @file    I2CMasterFeatureAdapter.cpp
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

#include "common/globals.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/seabreezeapi/I2CMasterFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;


I2CMasterFeatureAdapter::I2CMasterFeatureAdapter(
        i2cMasterFeatureInterface *intf, const FeatureFamily &f, Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<i2cMasterFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

I2CMasterFeatureAdapter::~I2CMasterFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char I2CMasterFeatureAdapter::i2cMasterGetNumberOfBuses(int *errorCode)
{
	unsigned char numberOfBuses = 0;
	try {
		numberOfBuses = this->feature->i2cMasterGetNumberOfBuses(*this->protocol, *this->bus);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return numberOfBuses;
}

unsigned short  I2CMasterFeatureAdapter::i2cMasterReadBus(int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
{

    vector<unsigned char> i2cReadData;
	unsigned short readDataLength = 0;

    try
	{
        i2cReadData = this->feature->i2cMasterReadBus(*this->protocol, *this->bus, busIndex,slaveAddress, numberOfBytes);
		readDataLength = (unsigned short)i2cReadData.size();

        memcpy(&i2cReadData[0], readData, readDataLength);

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
	return readDataLength;
}

unsigned short I2CMasterFeatureAdapter::i2cMasterWriteBus(int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
{
	unsigned short i2cWriteLength = 0;
    vector<unsigned char> *i2cWriteData = new vector<unsigned char>(numberOfBytes);
    memcpy(&((*i2cWriteData)[0]), writeData, numberOfBytes);

    try {
        i2cWriteLength = this->feature->i2cMasterWriteBus(*this->protocol, *this->bus, busIndex, slaveAddress, *i2cWriteData);
        delete i2cWriteData;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        delete i2cWriteData;
    }
	return i2cWriteLength;
}
