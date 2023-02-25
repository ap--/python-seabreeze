/***************************************************//**
 * @file    gpioFeatureAdapter.cpp
 * @date    April 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze gpioFeatureInterface instances.
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
#include "api/seabreezeapi/gpioFeatureAdapter.h"
#include <string.h> /* for memcpy pre c++11 */
#include <vector>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

gpioFeatureAdapter::gpioFeatureAdapter(
        gpioFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<gpioFeatureInterface>(intf, f, p, b, instanceIndex)
{

    /* Nothing else to do here, the initialization list takes care of it */
}

gpioFeatureAdapter::~gpioFeatureAdapter()
{
    /* This is just a wrapper around existing instances -- nothing to delete */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif


unsigned char gpioFeatureAdapter::getGPIO_NumberOfPins(int *errorCode)
{
	unsigned char numberOfPins = 0;
    try {
		numberOfPins = this->feature->getGPIO_NumberOfPins(*this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
    return numberOfPins;
}

unsigned int gpioFeatureAdapter::getGPIO_OutputEnableVector(int *errorCode)
{
	unsigned char outputEnableVector = 0;
	try {
		outputEnableVector = this->feature->getGPIO_OutputEnableVector(*this->protocol, *this->bus);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return outputEnableVector;
}

void gpioFeatureAdapter::setGPIO_OutputEnableVector(int *errorCode, unsigned int outputEnableVector, unsigned int bitMask)
{
    try
	{
        this->feature->setGPIO_OutputEnableVector(*this->protocol, *this->bus, outputEnableVector, bitMask);
        SET_ERROR_CODE(ERROR_SUCCESS);
    }
	catch (FeatureException &fe)
	{
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
    }
}

unsigned int gpioFeatureAdapter::getGPIO_ValueVector(int *errorCode)
{
	unsigned char valueVector = 0;
	try {
		valueVector = this->feature->getGPIO_ValueVector(*this->protocol, *this->bus);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return valueVector;
}

void gpioFeatureAdapter::setGPIO_ValueVector(int *errorCode, unsigned int valueVector, unsigned int bitMask)
{
	try
	{
		this->feature->setGPIO_ValueVector(*this->protocol, *this->bus, valueVector, bitMask);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}

unsigned char gpioFeatureAdapter::getEGPIO_NumberOfPins(int *errorCode)
{
	unsigned char numberOfPins = 0;
	try {
		numberOfPins = this->feature->getGPIO_NumberOfPins(*this->protocol, *this->bus);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return numberOfPins;
}

unsigned char gpioFeatureAdapter::getEGPIO_AvailableModes(int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maximumModeCount )
{

	vector<unsigned char> availableModesVector;

	try
	{
		availableModesVector = this->feature->getEGPIO_AvailableModes(*this->protocol, *this->bus, pinNumber);

		if (availableModesVector.size() < maximumModeCount)
		{
			memcpy(availableModes, &(availableModesVector[0]), availableModesVector.size());
		}
		else
		{
			SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		}

		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return availableModesVector.size() & 0xFF;
}

unsigned char gpioFeatureAdapter::getEGPIO_CurrentMode(int *errorCode, unsigned char pinNumber)
{
	unsigned char mode = 0;
	try {
		mode = this->feature->getEGPIO_CurrentMode(*this->protocol, *this->bus, pinNumber);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return mode;
}

void gpioFeatureAdapter::setEGPIO_Mode(int *errorCode, unsigned char pinNumber, unsigned char mode, float value)
{
	try
	{
		this->feature->setEGPIO_Mode(*this->protocol, *this->bus, pinNumber, mode, value);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}

unsigned int gpioFeatureAdapter::getEGPIO_OutputVector(int *errorCode)
{
	unsigned int outputVector = 0;
	try {
		outputVector = this->feature->getEGPIO_OutputVector(*this->protocol, *this->bus);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return outputVector;
}

void gpioFeatureAdapter::setEGPIO_OutputVector(int *errorCode, unsigned int outputVector, unsigned int bitMask)
{
	try
	{
		this->feature->setEGPIO_OutputVector(*this->protocol, *this->bus, outputVector, bitMask);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}

float gpioFeatureAdapter::getEGPIO_Value(int *errorCode, unsigned char pinNumber)
{
	float value = 0;
	try {
		value = this->feature->getEGPIO_Value(*this->protocol, *this->bus, pinNumber);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
	return value;
}

void gpioFeatureAdapter::setEGPIO_Value(int *errorCode, unsigned char pinNumber, float defaultValue)
{
	try
	{
		this->feature->setEGPIO_Value(*this->protocol, *this->bus, pinNumber, defaultValue);
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (FeatureException &fe)
	{
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
	}
}
