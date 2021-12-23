/***************************************************//**
 * @file    SpectrometerFeatureAdapter.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows access to SeaBreeze
 * OOISpectrometerFeature instances.
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
#include <string>
#include <string.h>     /* for memcpy() */
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/seabreezeapi/SpectrometerFeatureAdapter.h"
#include "common/exceptions/IllegalArgumentException.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

SpectrometerFeatureAdapter::SpectrometerFeatureAdapter(
        OOISpectrometerFeatureInterface *spec, const FeatureFamily &f,
        seabreeze::Protocol *p, seabreeze::Bus *b, unsigned short instanceID)
            : FeatureAdapterTemplate<OOISpectrometerFeatureInterface>(spec,
                f, p, b, instanceID) {

}

SpectrometerFeatureAdapter::~SpectrometerFeatureAdapter() {
    /* This is just a wrapper around pointers to instances.  This is not
     * responsible for destroying anything.
     */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
int SpectrometerFeatureAdapter::getUnformattedSpectrum(int *errorCode,
                    unsigned char *buffer, int bufferLength) {
    vector<unsigned char> *spectrum;
    int bytesCopied = 0;

    if(NULL == buffer) {
        SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
        return 0;
    }

    try {
        spectrum = this->feature->getUnformattedSpectrum(*this->protocol,
            *this->bus);
        int bytes = (int) spectrum->size();
        bytesCopied = (bytes < bufferLength) ? bytes : bufferLength;
        memcpy(buffer, &((*spectrum)[0]), bytesCopied * sizeof (unsigned char));
        delete spectrum;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    return bytesCopied;
}

int SpectrometerFeatureAdapter::getFastBufferSpectrum(int *errorCode,
	unsigned char *buffer, int bufferLength, unsigned int numberOfSamplesToRetrieve) {
	vector<unsigned char> *spectrum;
	int bytesCopied = 0;

	if (NULL == buffer) {
		SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		return 0;
	}

	try {

		spectrum = this->feature->getFastBufferSpectrum(*this->protocol, *this->bus, numberOfSamplesToRetrieve);
		int bytes = (int)spectrum->size();
		bytesCopied = (bytes < bufferLength) ? bytes : bufferLength;
		if (spectrum->size() > 0)
		{
			memcpy(buffer, &((*spectrum)[0]), bytesCopied * sizeof(unsigned char));
		}
		delete spectrum;
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (const FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		return 0;
	}

	return bytesCopied;
}


int SpectrometerFeatureAdapter::getFormattedSpectrum(int *errorCode,
                    double* buffer, int bufferLength) {
    vector<double> *spectrum;
    int doublesCopied = 0;

    if(NULL == buffer) {
        SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
        return 0;
    }

    try {
        spectrum = this->feature->getFormattedSpectrum(*this->protocol, *this->bus);
        int pixels = (int) spectrum->size();
        doublesCopied = (pixels < bufferLength) ? pixels : bufferLength;
        memcpy(buffer, &((*spectrum)[0]), doublesCopied * sizeof (double));
        delete spectrum;
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {

		// the get spectrum calls should have an argument for the error string so that fe.what can be used
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
    return doublesCopied;
}

int SpectrometerFeatureAdapter::getUnformattedSpectrumLength(int *errorCode) {
    /* This is, unfortunately, very hard to implement directly.
     * The readout length from the device is buried inside a particular
     * protocol message that would require a lot of digging to reach.
     * Since every protocol could have a different readout length,
     * this value cannot simply be stored in the spectrometer feature
     * like the number of pixels.
     *
     * This leaves two (not particularly nice) options.  The first is
     * to simply get an unformatted spectrum, check the length, and
     * throw it away.  The other is to create a lookup table of all
     * possible lengths based on the device type.  I am opting for
     * the former, even though it could have unpleasant side-effects.
     */
    vector<unsigned char> *spectrum;

    try {
        spectrum = this->feature->getUnformattedSpectrum(
            *this->protocol, *this->bus);
        int length = (int) spectrum->size();
        delete spectrum;
        SET_ERROR_CODE(ERROR_SUCCESS);
        return length;
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
    return 0;
}


int SpectrometerFeatureAdapter::getFormattedSpectrumLength(int *errorCode) {
    int numberOfPixels = 0;

    try {
        numberOfPixels = this->feature->getNumberOfPixels();
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
    return numberOfPixels;
}

void SpectrometerFeatureAdapter::setTriggerMode(int *errorCode, int mode) {
    SpectrometerTriggerMode triggerMode(mode);

    try {
        this->feature->setTriggerMode(*this->protocol, *this->bus, triggerMode);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_INVALID_TRIGGER_MODE);
        return;
    }
}


int SpectrometerFeatureAdapter::getWavelengths(int *errorCode,
        double *wavelengths, int length) {
    int valuesCopied = 0;
    int i;
    vector<double> *wlVector;
    vector<double>::iterator iter;

    try {
        wlVector = this->feature->getWavelengths(*this->protocol, *this->bus);
        /* It might be possible to do a memcpy() of the underlying vector into
         * the array, but that isn't the safest thing to do.  As long as this is
         * called once and the result cached, the inefficiency won't hurt.
         */
        for(iter = wlVector->begin(), i = 0;
                iter != wlVector->end() && i < length; iter++, i++) {
            wavelengths[i] = *iter;
            valuesCopied++;
        }

        delete wlVector;

        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }
    return valuesCopied;
}

unsigned short SpectrometerFeatureAdapter::getNumberOfPixels(int *errorCode)
{
// this, of course, is the same as getFormattedSpectrumLength(), but has a more descriptive name.

	unsigned short numberOfPixels = 0;

	try {
		numberOfPixels = this->feature->getNumberOfPixels();
		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	catch (const FeatureException &fe) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return numberOfPixels;
}

int SpectrometerFeatureAdapter::getElectricDarkPixelCount(int *errorCode) {
    vector<unsigned int> pixelVector;

    pixelVector = this->feature->getElectricDarkPixelIndices();

    SET_ERROR_CODE(ERROR_SUCCESS);
    return (int) pixelVector.size();
}

int SpectrometerFeatureAdapter::getElectricDarkPixelIndices(int *errorCode,
                    int *indices, int length) {
    int valuesCopied = 0;
    int i;
    vector<unsigned int> pixelVector;
    vector<unsigned int>::iterator iter;

    pixelVector = this->feature->getElectricDarkPixelIndices();

    /* It might be possible to do a memcpy() of the underlying vector into
     * the array, but that isn't the safest thing to do.  As long as this is
     * called once and the result cached, the inefficiency won't hurt.
     */
    for(iter = pixelVector.begin(), i = 0;
            iter != pixelVector.end() && i < length; iter++, i++) {
        indices[i] = *iter;
        valuesCopied++;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return valuesCopied;
}

int SpectrometerFeatureAdapter::getOpticalDarkPixelCount(int *errorCode) {
	vector<unsigned int> pixelVector;

	pixelVector = this->feature->getOpticalDarkPixelIndices();

	SET_ERROR_CODE(ERROR_SUCCESS);
	return (int)pixelVector.size();
}

int SpectrometerFeatureAdapter::getOpticalDarkPixelIndices(int *errorCode,
	int *indices, int length) {
	int valuesCopied = 0;
	int i;
	vector<unsigned int> pixelVector;
	vector<unsigned int>::iterator iter;

	pixelVector = this->feature->getOpticalDarkPixelIndices();

	/* It might be possible to do a memcpy() of the underlying vector into
	* the array, but that isn't the safest thing to do.  As long as this is
	* called once and the result cached, the inefficiency won't hurt.
	*/
	for (iter = pixelVector.begin(), i = 0;
		iter != pixelVector.end() && i < length; iter++, i++) {
		indices[i] = *iter;
		valuesCopied++;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return valuesCopied;
}

int SpectrometerFeatureAdapter::getActivePixelCount(int *errorCode) {
	vector<unsigned int> pixelVector;

	pixelVector = this->feature->getActivePixelIndices();

	SET_ERROR_CODE(ERROR_SUCCESS);
	return (int)pixelVector.size();
}

int SpectrometerFeatureAdapter::getActivePixelIndices(int *errorCode,
	int *indices, int length) {
	int valuesCopied = 0;
	int i;
	vector<unsigned int> pixelVector;
	vector<unsigned int>::iterator iter;

	pixelVector = this->feature->getActivePixelIndices();

	/* It might be possible to do a memcpy() of the underlying vector into
	* the array, but that isn't the safest thing to do.  As long as this is
	* called once and the result cached, the inefficiency won't hurt.
	*/
	for (iter = pixelVector.begin(), i = 0;
		iter != pixelVector.end() && i < length; iter++, i++) {
		indices[i] = *iter;
		valuesCopied++;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return valuesCopied;
}

void SpectrometerFeatureAdapter::setIntegrationTimeMicros(int *errorCode,
                    unsigned long integrationTimeMicros) {
    try {
        this->feature->setIntegrationTimeMicros(*this->protocol, *this->bus,
                    integrationTimeMicros);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    } catch (const IllegalArgumentException &iae) {
        SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
        return;
    }
}

long SpectrometerFeatureAdapter::getMinimumIntegrationTimeMicros(int *errorCode) {
    long retval = -1;

    try {
        retval = this->feature->getIntegrationTimeMinimum();
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (const FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return -1;
    }
    return retval;
}

long SpectrometerFeatureAdapter::getMaximumIntegrationTimeMicros(int *errorCode) {
    long retval = -1;

    try {
        retval = this->feature->getIntegrationTimeMaximum();
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return -1;
    }
    return retval;
}



double SpectrometerFeatureAdapter::getMaximumIntensity(int *errorCode) {
    long retval = -1;

    try {
        retval = this->feature->getMaximumIntensity();
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return -1;
    }
    return retval;
}
