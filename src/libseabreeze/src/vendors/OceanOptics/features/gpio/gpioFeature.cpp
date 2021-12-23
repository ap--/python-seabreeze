/***************************************************//**
 * @file    gpioFeature.cpp
 * @date    April 2017
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
#include "vendors/OceanOptics/features/gpio/gpioFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/GPIOProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

GPIOFeature::GPIOFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++)
    {
        this->protocols.push_back(*iter);
    }
}

GPIOFeature::~GPIOFeature()
{

}

unsigned char GPIOFeature::getGPIO_NumberOfPins(const Protocol &protocol, const Bus &bus)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	unsigned char numberOfPins;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (const FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		numberOfPins = GPIO_PI->getGPIO_NumberOfPins(bus);
	}
	catch (const ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return numberOfPins;
}


unsigned int GPIOFeature::getGPIO_OutputEnableVector(const Protocol &protocol, const Bus &bus)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	unsigned int outputEnableVector;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (const FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		outputEnableVector = GPIO_PI->getGPIO_OutputEnableVector(bus);
	}
	catch (const ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return outputEnableVector;
}

void GPIOFeature::setGPIO_OutputEnableVector(const Protocol &protocol, const Bus &bus, unsigned int outputEnableVector, unsigned int bitMask)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		GPIO_PI->setGPIO_OutputEnableVector(bus, outputEnableVector, bitMask);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

}

unsigned int GPIOFeature::getGPIO_ValueVector(const Protocol &protocol, const Bus &bus)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	unsigned int valueVector;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		valueVector = GPIO_PI->getGPIO_ValueVector(bus);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return valueVector;
}

void GPIOFeature::setGPIO_ValueVector(const Protocol &protocol, const Bus &bus, unsigned int valueVector, unsigned int bitMask)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		GPIO_PI->setGPIO_ValueVector(bus, valueVector, bitMask);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

}

unsigned char GPIOFeature::getEGPIO_NumberOfPins(const Protocol &protocol, const Bus &bus)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	unsigned char numberOfPins;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		numberOfPins = GPIO_PI->getEGPIO_NumberOfPins(bus);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return numberOfPins;
}

vector<unsigned char> GPIOFeature::getEGPIO_AvailableModes(const Protocol &protocol, const Bus &bus, unsigned char pinNumber)
{

	vector<unsigned char> data;
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;

	try
	{
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
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
		data = GPIO_PI->getEGPIO_AvailableModes(bus, pinNumber);
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

unsigned char GPIOFeature::getEGPIO_CurrentMode(const Protocol &protocol, const Bus &bus, unsigned char pinNumber)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	unsigned char mode = 0;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		mode = GPIO_PI->getEGPIO_CurrentMode(bus, pinNumber);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return mode;
}

void GPIOFeature::setEGPIO_Mode(const Protocol &protocol, const Bus &bus, unsigned char pinNumber, unsigned char mode, float value)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		GPIO_PI->setEGPIO_Mode(bus, pinNumber, mode, value);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

}

unsigned int GPIOFeature::getEGPIO_OutputVector(const Protocol &protocol, const Bus &bus)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	unsigned int outputEnableVector;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		outputEnableVector = GPIO_PI->getEGPIO_OutputVector(bus);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return outputEnableVector;
}

void GPIOFeature::setEGPIO_OutputVector(const Protocol &protocol, const Bus &bus, unsigned int outputVector, unsigned int bitMask)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		GPIO_PI->setEGPIO_OutputVector(bus, outputVector, bitMask);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}


float GPIOFeature::getEGPIO_Value(const Protocol &protocol, const Bus &bus, unsigned char pinNumber)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;
	float value;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to get data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		value = GPIO_PI->getEGPIO_Value(bus, pinNumber);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}

	return value;
}

void GPIOFeature::setEGPIO_Value(const Protocol &protocol, const Bus &bus, unsigned char pinNumber, float value)
{
	GPIOProtocolInterface *GPIO_PI = NULL;
	ProtocolHelper *proto;

	try {
		proto = lookupProtocolImpl(protocol);
		GPIO_PI = static_cast<GPIOProtocolInterface *>(proto);
	}
	catch (FeatureProtocolNotFoundException &e) {
		string error(
			"Could not find matching protocol implementation to write data.");
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureProtocolNotFoundException(error);
	}

	try {
		GPIO_PI->setEGPIO_Value(bus, pinNumber, value);
	}
	catch (ProtocolException &pe) {
		string error("Caught protocol exception: ");
		error += pe.what();
		/* FIXME: previous exception should probably be bundled up into the new exception */
		throw FeatureControlException(error);
	}
}



FeatureFamily GPIOFeature::getFeatureFamily() {
    FeatureFamilies families;

	return families.GENERAL_PURPOSE_INPUT_OUTPUT;
}
