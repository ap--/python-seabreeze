/***************************************************//**
 * @file    GPIOProtocolInterface.h
 * @date    April 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a generic interface into ethernet configuration
 * functionality at the protocol level, agnostic to
 * any particular protocol.  Each Protocol offering this
 * functionality should implement this interface.
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

#ifndef GPIOPROTOCOLINTERFACE_H
#define GPIOPROTOCOLINTERFACE_H

#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze
{

    class GPIOProtocolInterface : public ProtocolHelper
	{
    public:
		GPIOProtocolInterface(Protocol *protocol);
        virtual ~GPIOProtocolInterface();

        /**
         * Get the Number of GPIO pins
         */
        virtual unsigned char getGPIO_NumberOfPins(const Bus &bus) = 0;

        /**
         *  Get the output enable vector
         */
		virtual unsigned int getGPIO_OutputEnableVector(const Bus &bus) = 0;

        /**
         * Set the output enable vector
         */
        virtual void setGPIO_OutputEnableVector(const Bus &bus, unsigned int outputEnableVector, unsigned int bitMask) = 0;

		/**
		*  Get the value vector
		*/
		virtual unsigned int getGPIO_ValueVector(const Bus &bus) = 0;

		/**
		* Set the value vector
		*/
		virtual void setGPIO_ValueVector(const Bus &bus, unsigned int valueVector, unsigned int bitMask) = 0;



		/**
		* Get the Number of eGPIO pins
		*/
		virtual unsigned char getEGPIO_NumberOfPins(const Bus &bus) = 0;

		/**
		* Get the available eGPIO modes for the given pin
		*/
		virtual std::vector<unsigned char> getEGPIO_AvailableModes(const Bus &bus, unsigned char pinNumber) = 0;

		/**
		*  Get the current mode for a given pin
		*/
		virtual unsigned char getEGPIO_CurrentMode(const Bus &bus, unsigned char pinNumber) = 0;

		/**
		* Set the current mode for a given pin, value is 0.0 to 1.0
		*/
		virtual void setEGPIO_Mode(const Bus &bus, unsigned char pinNumber, unsigned char mode, float value) = 0;

		/**
		*  Get the egpio output vector
		*/
		virtual unsigned int getEGPIO_OutputVector(const Bus &bus) = 0;

		/**
		* Set the egpio output vector
		*/
		virtual void setEGPIO_OutputVector(const Bus &bus, unsigned int outputVector, unsigned int bitMask) = 0;


		/**
		*  Get the egpio Value, normalized 0.0 to 1.0
		*/
		virtual float getEGPIO_Value(const Bus &bus, unsigned char pinNumber) = 0;

		/**
		* Set the egpio value
		*/
		virtual void setEGPIO_Value(const Bus &bus, unsigned char pinNumber, float value) = 0;
    };

}

#endif /* GPIOPROTOCOLINTERFACE_H */
