/***************************************************//**
 * @file    i2cMasterFeature.cpp
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

#include "common/globals.h"
#include "vendors/OceanOptics/features/i2c_master/i2cMasterFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/I2CMasterProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

i2cMasterFeature::i2cMasterFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

i2cMasterFeature::~i2cMasterFeature()
{

}

unsigned char i2cMasterFeature::i2cMasterGetNumberOfBuses(const Protocol &protocol, const Bus &bus)
{

	I2CMasterProtocolInterface *i2cMasterPI = NULL;
	ProtocolHelper *proto;
	unsigned char numberOfBuses;

	try {
		proto = lookupProtocolImpl(protocol);
		i2cMasterPI = static_cast<I2CMasterProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		numberOfBuses = i2cMasterPI->i2cMasterGetNumberOfBuses(bus);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return numberOfBuses;
}

std::vector<unsigned char> i2cMasterFeature::i2cMasterReadBus(const Protocol &protocol, const Bus &bus, unsigned char busIndex, unsigned char slaveAddress, unsigned short numberOfBytes)
{

	vector<unsigned char> data;
	I2CMasterProtocolInterface *i2cMasterPI = NULL;
	ProtocolHelper *proto;

	try
	{
		proto = lookupProtocolImpl(protocol);
		i2cMasterPI = static_cast<I2CMasterProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e)
	{
		string error(
			"Could not find matching protocol implementation to data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try
	{
		data = i2cMasterPI->i2cMasterReadBus(bus, busIndex, slaveAddress, numberOfBytes);
	}
	catch (ProtocolException &pe)
	{
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return data;
}

unsigned short i2cMasterFeature::i2cMasterWriteBus(const Protocol &protocol, const Bus &bus, unsigned char busIndex, unsigned char slaveAddress, const std::vector<unsigned char> writeData)
{

	unsigned short data;
	I2CMasterProtocolInterface *i2cMasterPI = NULL;
	ProtocolHelper *proto;

	try
	{
		proto = lookupProtocolImpl(protocol);
		i2cMasterPI = static_cast<I2CMasterProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e)
	{
		string error(
			"Could not find matching protocol implementation to data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try
	{
		data = i2cMasterPI->i2cMasterWriteBus(bus, busIndex, slaveAddress, writeData);
	}
	catch (ProtocolException &pe)
	{
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return data;
}



FeatureFamily i2cMasterFeature::getFeatureFamily() {
    FeatureFamilies families;

	return families.GENERAL_PURPOSE_INPUT_OUTPUT;
}
