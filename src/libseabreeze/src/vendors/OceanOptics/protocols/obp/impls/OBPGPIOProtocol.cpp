/***************************************************//**
 * @file    OBPGPIOProtocol.cpp
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
#include <math.h>

#include "vendors/OceanOptics/protocols/obp/impls/OBPGPIOProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOExtensionAvailableModesExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOExtensionCurrentModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOExtensionNumberOfPinsExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOExtensionOutputVectorExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOExtensionValueExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIONumberOfPinsExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOOutputEnableVectorExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPGetGPIOValueVectorExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetGPIOExtensionModeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetGPIOExtensionOutputVectorExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetGPIOExtensionValueExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetGPIOOutputEnableVectorExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPSetGPIOValueVectorExchange.h"
#include "common/ByteVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPGPIOProtocol::OBPGPIOProtocol()
        : GPIOProtocolInterface(new OceanBinaryProtocol())
{

}

OBPGPIOProtocol::~OBPGPIOProtocol()
{

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char OBPGPIOProtocol::getGPIO_NumberOfPins(const Bus &bus)
{
    TransferHelper *helper;
	OBPGetGPIONumberOfPinsExchange request;

    helper = bus.getHelper(request.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    vector<unsigned char> *raw = request.queryDevice(helper);
    if (NULL == raw) {
        string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
        throw ProtocolException(error);
    }

    if(raw->size() < sizeof(unsigned char)) {
        string error("Failed to get back expected number of bytes that should"
            " have held collection area.");
        delete raw;
        throw ProtocolException(error);
    }

	unsigned char retval = (*raw)[0];

    delete raw;

    return retval;
}

unsigned int OBPGPIOProtocol::getGPIO_OutputEnableVector(const Bus &bus)
{
	TransferHelper *helper;
	OBPGetGPIOOutputEnableVectorExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned int retval = (*raw)[0];

	delete raw;

	return retval;
}

void OBPGPIOProtocol::setGPIO_OutputEnableVector(const Bus &bus, unsigned int outputEnableVector, unsigned int bitMask)
{
    TransferHelper *helper;
	OBPSetGPIOOutputEnableVectorExchange command;

    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

	command.setOutputEnableVector(outputEnableVector);
	command.setOutputEnableMask(bitMask);

    /* This may cause a ProtocolException to be thrown. */
    command.sendCommandToDevice(helper);

    /* FIXME: this could check the return value and react if it did not succeed */
}

unsigned int OBPGPIOProtocol::getGPIO_ValueVector(const Bus &bus)
{
	TransferHelper *helper;
	OBPGetGPIOValueVectorExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned int retval = (*raw)[0];

	delete raw;

	return retval;
}

void OBPGPIOProtocol::setGPIO_ValueVector(const Bus &bus, unsigned int valueVector, unsigned int bitMask)
{
	TransferHelper *helper;
	OBPSetGPIOValueVectorExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	command.setValueVector(valueVector);
	command.setBitMask(bitMask);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

	/* FIXME: this could check the return value and react if it did not succeed */
}

unsigned char OBPGPIOProtocol::getEGPIO_NumberOfPins(const Bus &bus)
{
	TransferHelper *helper;
	OBPGetGPIOExtensionNumberOfPinsExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned char retval = (*raw)[0];

	delete raw;

	return retval;
}

std::vector<unsigned char> OBPGPIOProtocol::getEGPIO_AvailableModes(const Bus &bus, unsigned char pinNumber)
{
	TransferHelper *helper;
	OBPGetGPIOExtensionAvailableModesExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	request.setPinNumber(pinNumber);

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result "
			"containing calibration data.  Without this data, it is not possible to "
			"generate a calibration array.");
		throw ProtocolException(error);
	}

	vector<unsigned char> result = *raw;

	delete raw;

	return result;
}

unsigned char OBPGPIOProtocol::getEGPIO_CurrentMode(const Bus &bus, unsigned char pinNumber )
{
	TransferHelper *helper;
	OBPGetGPIOExtensionCurrentModeExchange request;

	request.setPinNumber(pinNumber);

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned char retval = (*raw)[0];

	delete raw;

	return retval;
}

void OBPGPIOProtocol::setEGPIO_Mode(const Bus &bus, unsigned char pinNumber, unsigned char mode, float value)
{
	TransferHelper *helper;
	OBPSetGPIOExtensionModeExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}
	command.setPinNumber(pinNumber);
	command.setMode(mode);
	command.setValue(value);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

	/* FIXME: this could check the return value and react if it did not succeed */
}

unsigned int OBPGPIOProtocol::getEGPIO_OutputVector(const Bus &bus)
{
	TransferHelper *helper;
	OBPGetGPIOExtensionOutputVectorExchange request;

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	unsigned int retval = (*raw)[0];

	delete raw;

	return retval;
}

void OBPGPIOProtocol::setEGPIO_OutputVector(const Bus &bus, unsigned int outputVector, unsigned int bitMask)
{
	TransferHelper *helper;
	OBPSetGPIOExtensionOutputVectorExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	command.setOutputVector(outputVector);
	command.setBitMask(bitMask);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

	/* FIXME: this could check the return value and react if it did not succeed */
}


float OBPGPIOProtocol::getEGPIO_Value(const Bus &bus, unsigned char pinNumber)
{
	TransferHelper *helper;
	OBPGetGPIOExtensionValueExchange request;

	request.setPinNumber(pinNumber);

	helper = bus.getHelper(request.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	vector<unsigned char> *raw = request.queryDevice(helper);
	if (NULL == raw) {
		string error("Expected queryDevice to produce a non-null result.  Without this data, it is not possible to continue.");
		throw ProtocolException(error);
	}

	if (raw->size() < sizeof(unsigned char)) {
		string error("Failed to get back expected number of bytes that should"
			" have held collection area.");
		delete raw;
		throw ProtocolException(error);
	}

	float retval = (*raw)[0];

	delete raw;

	return retval;
}

void OBPGPIOProtocol::setEGPIO_Value(const Bus &bus, unsigned char pinNumber, float value)
{
	TransferHelper *helper;
	OBPSetGPIOExtensionValueExchange command;

	helper = bus.getHelper(command.getHints());
	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	command.setPinNumber(pinNumber);
	command.setValue(value);

	/* This may cause a ProtocolException to be thrown. */
	command.sendCommandToDevice(helper);

	/* FIXME: this could check the return value and react if it did not succeed */
}
