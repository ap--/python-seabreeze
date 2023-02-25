/***************************************************//**
 * @file    gpioFeatureAdapter.h
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

#ifndef SEABREEZE_GPIOFEATUREADAPTER_H
#define SEABREEZE_GPIOFEATUREADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/gpio/gpioFeatureInterface.h"

namespace seabreeze
{
    namespace api
    {

        class gpioFeatureAdapter
                : public FeatureAdapterTemplate<gpioFeatureInterface>
		{
        	public:
            	gpioFeatureAdapter( gpioFeatureInterface *intf, const FeatureFamily &f, Protocol *p, Bus *b, unsigned short instanceIndex);
            	virtual ~gpioFeatureAdapter();

				unsigned char getGPIO_NumberOfPins(int *errorCode);
				unsigned int getGPIO_OutputEnableVector(int *errorCode);
				void setGPIO_OutputEnableVector(int *errorCode, unsigned int outputEnableVector, unsigned int bitMask);
				unsigned int getGPIO_ValueVector(int *errorCode);
				void setGPIO_ValueVector(int *errorCode, unsigned int valueVector, unsigned int bitMask);
				unsigned char getEGPIO_NumberOfPins(int *errorCode);
				unsigned char getEGPIO_AvailableModes(int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maxModeCount);
				unsigned char getEGPIO_CurrentMode(int *errorCode, unsigned char pinNumber);
				void setEGPIO_Mode(int *errorCode, unsigned char pinNumber, unsigned char mode, float value);
				unsigned int getEGPIO_OutputVector(int *errorCode);
				void setEGPIO_OutputVector(int *errorCode, unsigned int outputVector, unsigned int bitMask);
				float getEGPIO_Value(int *errorCode, unsigned char pinNumber);
				void setEGPIO_Value(int *errorCode, unsigned char pinNumber, float value);
        };

    }
}

#endif //  SEABREEZE_GPIOFEATUREADAPTER_H
