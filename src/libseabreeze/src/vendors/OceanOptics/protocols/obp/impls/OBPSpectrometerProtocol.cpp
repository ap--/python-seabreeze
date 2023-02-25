/***************************************************//**
 * @file    OBPSpectrometerProtocol.cpp
 * @date    February 2011
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPSpectrometerProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "common/ByteVector.h"
#include "common/Data.h"
#include "common/UShortVector.h"
#include "common/U32Vector.h"
#include "common/DoubleVector.h"
#include "common/exceptions/ProtocolBusMismatchException.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPSpectrometerProtocol::OBPSpectrometerProtocol(
        OBPIntegrationTimeExchange *integrationTime,
        Transfer *requestFormattedSpectrum,
		Transfer *readFormattedSpectrum,
        Transfer *requestUnformattedSpectrum,
        Transfer *readUnformattedSpectrum,
		Transfer *requestFastBufferSpectrum,
		Transfer *readFastBufferSpectrum,
        OBPTriggerModeExchange *triggerMode)
            : SpectrometerProtocolInterface(new OceanBinaryProtocol()) {
    this->integrationTimeExchange = integrationTime;
	this->requestFormattedSpectrumExchange = requestFormattedSpectrum;
	this->readFormattedSpectrumExchange = readFormattedSpectrum;
	this->requestUnformattedSpectrumExchange = requestUnformattedSpectrum;
    this->readUnformattedSpectrumExchange = readUnformattedSpectrum;
	this->requestFastBufferSpectrumExchange = requestFastBufferSpectrum;
	this->readFastBufferSpectrumExchange = readFastBufferSpectrum;
    this->triggerModeExchange = triggerMode;
}

OBPSpectrometerProtocol::~OBPSpectrometerProtocol() {
    delete this->integrationTimeExchange;
	delete this->requestFormattedSpectrumExchange;
	delete this->readFormattedSpectrumExchange;
	delete this->requestUnformattedSpectrumExchange;
    delete this->readUnformattedSpectrumExchange;
	delete this->requestFastBufferSpectrumExchange;
	delete this->readFastBufferSpectrumExchange;
    delete this->triggerModeExchange;
}

void OBPSpectrometerProtocol::Initialize(
	OBPIntegrationTimeExchange *integrationTime,
	Transfer *requestFormattedSpectrum,
	Transfer *readFormattedSpectrum,
	Transfer *requestUnformattedSpectrum,
	Transfer *readUnformattedSpectrum,
	Transfer *requestFastBufferSpectrum,
	Transfer *readFastBufferSpectrum,
	OBPTriggerModeExchange *triggerMode)
{

	if (this->integrationTimeExchange != NULL)
	{
		delete integrationTimeExchange;
	}
	this->integrationTimeExchange = integrationTime;


	if (this->requestFormattedSpectrumExchange != NULL)
	{
		delete requestFormattedSpectrumExchange;
	}
	this->requestFormattedSpectrumExchange = requestFormattedSpectrum;


	if (this->readFormattedSpectrumExchange != NULL)
	{
		delete readFormattedSpectrumExchange;
	}
	this->readFormattedSpectrumExchange = readFormattedSpectrum;


	if (this->requestUnformattedSpectrumExchange != NULL)
	{
		delete requestUnformattedSpectrumExchange;
	}
	this->requestUnformattedSpectrumExchange = requestUnformattedSpectrum;


	if (this->readUnformattedSpectrumExchange != NULL)
	{
		delete readUnformattedSpectrumExchange;
	}
	this->readUnformattedSpectrumExchange = readUnformattedSpectrum;


	if (this->requestFastBufferSpectrumExchange != NULL)
	{
		delete requestFastBufferSpectrumExchange;
	}
	this->requestFastBufferSpectrumExchange = requestFastBufferSpectrum;


	if (this->readFastBufferSpectrumExchange != NULL)
	{
		delete readFastBufferSpectrumExchange;
	}
	this->readFastBufferSpectrumExchange = readFastBufferSpectrum;


	if (this->triggerModeExchange != NULL)
	{
		delete triggerModeExchange;
	}
	this->triggerModeExchange = triggerMode;

}

vector<unsigned char> *OBPSpectrometerProtocol::readUnformattedSpectrum(const Bus &bus)
{
    Data *result;
    TransferHelper *helper;

    helper = bus.getHelper(this->readUnformattedSpectrumExchange->getHints());
    if (NULL == helper)
	{
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    result = this->readUnformattedSpectrumExchange->transfer(helper);

    if (NULL == result)
	{
        string error("Got NULL when expecting spectral data which was unexpected.");
        throw ProtocolException(error);
    }

    ByteVector *bv = static_cast<ByteVector *>(result);

    vector<unsigned char> *retval = new vector<unsigned char> (bv->getByteVector());

    delete result;

    /* FIXME: this method should probably return (Data *) so that
     * metadata is preserved.  In that case, this should just return
     * the above result without any additional work.  The current
     * implementation has an extra allocate/copy/destroy overhead.
     */
    return retval;
}

vector<unsigned char> *OBPSpectrometerProtocol::readFastBufferSpectrum(const Bus &bus, unsigned int numberOfSamplesToRetrieve)
{
	Data *result;
	TransferHelper *helper;

	helper = bus.getHelper(this->readFastBufferSpectrumExchange->getHints());
	if (NULL == helper)
	{
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	// workaround for setting the number of samples to be taken by the buffered get spectrum in the Flame X
	// See transfer.h for more details
	this->readFastBufferSpectrumExchange->setParametersFunction(this->readFastBufferSpectrumExchange, numberOfSamplesToRetrieve);

	/* This transfer() may cause a ProtocolException to be thrown. */
	result = this->readFastBufferSpectrumExchange->transfer(helper);

	if (NULL == result) {
		string error("Got NULL when expecting spectral data which was unexpected.");
		throw ProtocolException(error);
	}

	ByteVector *bv = static_cast<ByteVector *>(result);

	vector<unsigned char> *retval = new vector<unsigned char>(bv->getByteVector());

	delete result;

	/* FIXME: this method should probably return (Data *) so that
	* metadata is preserved.  In that case, this should just return
	* the above result without any additional work.  The current
	* implementation has an extra allocate/copy/destroy overhead.
	*/
	return retval;
}

vector<double> *OBPSpectrometerProtocol::readFormattedSpectrum(const Bus &bus) {
    TransferHelper *helper;
    Data *result;
    unsigned int i;

    helper = bus.getHelper(this->readFormattedSpectrumExchange->getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    result = this->readFormattedSpectrumExchange->transfer(helper);

    if (NULL == result) {
        string error("Got NULL when expecting spectral data which was unexpected.");
        throw ProtocolException(error);
    }

    /* FIXME: not knowing whether doubles or shorts will be returned
     * requires an RTTI lookup with dynamic_cast.  It might be better
     * to have the conversion to doubles done at a lower level (if that
     * can be done without preventing readout of raw spectra) so that
     * this is not blind as to the correct type.  Also, doing both
     * of the dynamic_casts even though only one of them will be
     * valid is adding some overhead.  Hopefully it isn't much.
     */
    vector<double> *retval = NULL;
    UShortVector *usv = dynamic_cast<UShortVector *>(result);
    DoubleVector *dv = dynamic_cast<DoubleVector *>(result);
    U32Vector *u32v = dynamic_cast<U32Vector *>(result);
    if(NULL != usv) {
        vector<unsigned short> shortVec = usv->getUShortVector();

        retval = new vector<double>(shortVec.size());

        for (i = 0; i < shortVec.size(); i++) {
            (*retval)[i] = shortVec[i];
        }
    } else if(NULL != u32v) {
        vector<unsigned int> u32Vec = u32v->getU32Vector();

        retval = new vector<double>(u32Vec.size());
        for (i = 0; i < u32Vec.size(); i++) {
            (*retval)[i] = u32Vec[i];
        }
    } else if(NULL != dv) {
        vector<double> doubleVec= dv->getDoubleVector();
        retval = new vector<double>(doubleVec.size());

        for (i = 0; i < doubleVec.size(); i++) {
            (*retval)[i] = doubleVec[i];
        }
    }
    delete result; /* a.k.a. usv or dv */
    return retval;
}

void OBPSpectrometerProtocol::requestFormattedSpectrum(const Bus &bus) {
    TransferHelper *helper;

    helper = bus.getHelper(this->requestFormattedSpectrumExchange->getHints());

    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    /* This transfer() may cause a ProtocolException to be thrown. */
    this->requestFormattedSpectrumExchange->transfer(helper);
}

void OBPSpectrometerProtocol::requestUnformattedSpectrum(const Bus &bus) {
	TransferHelper *helper;

	helper = bus.getHelper(this->requestUnformattedSpectrumExchange->getHints());

	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	/* This transfer() may cause a ProtocolException to be thrown. */
	this->requestUnformattedSpectrumExchange->transfer(helper);
}

void OBPSpectrometerProtocol::requestFastBufferSpectrum(const Bus &bus, unsigned int numberOfSamplesToRetrieve) {
	TransferHelper *helper;

	helper = bus.getHelper(this->requestFastBufferSpectrumExchange->getHints());

	if (NULL == helper) {
		string error("Failed to find a helper to bridge given protocol and bus.");
		throw ProtocolBusMismatchException(error);
	}

	// workaround for setting the number of samples to be taken by the buffered get spectrum in the Flame X
	// See transfer.h for more details
	this->requestFastBufferSpectrumExchange->setParametersFunction(this->requestFastBufferSpectrumExchange, numberOfSamplesToRetrieve);
	/* This transfer() may cause a ProtocolException to be thrown. */
	this->requestFastBufferSpectrumExchange->transfer(helper);
}


void OBPSpectrometerProtocol::setIntegrationTimeMicros(const Bus &bus,
        unsigned long integrationTime_usec) {
    TransferHelper *helper;

    helper = bus.getHelper(this->integrationTimeExchange->getHints());

    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    this->integrationTimeExchange->setIntegrationTimeMicros(integrationTime_usec);
    /* This may cause a ProtocolException to be thrown. */
    this->integrationTimeExchange->sendCommandToDevice(helper);
}

void OBPSpectrometerProtocol::setTriggerMode(const Bus &bus,
            SpectrometerTriggerMode &mode) {
    TransferHelper *helper;

    helper = bus.getHelper(this->triggerModeExchange->getHints());

    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    this->triggerModeExchange->setTriggerMode(mode);
    /* This may cause a ProtocolException to be thrown. */
    this->triggerModeExchange->sendCommandToDevice(helper);
}
