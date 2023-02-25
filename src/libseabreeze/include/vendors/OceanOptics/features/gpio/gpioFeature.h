/***************************************************//**
 * @file    GPIOFeature.h
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

#ifndef GPIOFEATURE_H
#define GPIOFEATURE_H

#include "vendors/OceanOptics/features/gpio/gpioFeatureInterface.h"
#include "common/protocols/Protocol.h"
#include "common/features/FeatureImpl.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze
{

    class GPIOFeature : public FeatureImpl, public gpioFeatureInterface
    {
    public:
		GPIOFeature(std::vector<ProtocolHelper *> helpers);
        virtual ~GPIOFeature();

		virtual unsigned char getGPIO_NumberOfPins(
			const Protocol &protocol,
			const Bus &bus);
		virtual unsigned int getGPIO_OutputEnableVector(
			const Protocol &protocol,
			const Bus &bus);
		virtual void setGPIO_OutputEnableVector(
			const Protocol &protocol,
			const Bus &bus,
			unsigned int outputEnableVector,
			unsigned int bitMask);
		virtual unsigned int getGPIO_ValueVector(
			const Protocol &protocol,
			const Bus &bus);
		virtual void setGPIO_ValueVector(
			const Protocol &protocol,
			const Bus &bus,
			unsigned int valueVector,
			unsigned int bitMask);
		virtual unsigned char getEGPIO_NumberOfPins(
			const Protocol &protocol,
			const Bus &bus);
		virtual std::vector<unsigned char> getEGPIO_AvailableModes(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char pinNumber);
		virtual unsigned char getEGPIO_CurrentMode(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char pinNumber);
		virtual void setEGPIO_Mode(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char pinNumber,
			unsigned char mode,
			float value);
		virtual unsigned int getEGPIO_OutputVector(
			const Protocol &protocol,
			const Bus &bus);
		virtual void setEGPIO_OutputVector(
			const Protocol &protocol,
			const Bus &bus,
			unsigned int valueVector,
			unsigned int bitMask);
		virtual float getEGPIO_Value(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char pinNumber);
		virtual void setEGPIO_Value(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char pinNumber,
			float value);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();

    };

}

#endif /* GPIOFEATURE_H */
