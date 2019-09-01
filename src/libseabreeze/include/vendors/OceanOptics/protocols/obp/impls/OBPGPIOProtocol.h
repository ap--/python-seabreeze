/***************************************************//**
 * @file    OBPGPIOProtocol.h
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

#ifndef OBPGPIOPROTOCOL_H
#define OBPGPIOPROTOCOL_H

#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/interfaces/GPIOProtocolInterface.h"
#include "common/exceptions/ProtocolException.h"

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPGPIOProtocol : public GPIOProtocolInterface {
    public:
		OBPGPIOProtocol();

        virtual ~OBPGPIOProtocol();

        /* Inherited from OBPGPIOProtocolInterface */
		virtual unsigned char getGPIO_NumberOfPins(const Bus &bus);
		virtual unsigned int getGPIO_OutputEnableVector(const Bus &bus);
		virtual void setGPIO_OutputEnableVector(const Bus &bus, unsigned int outputEnableVector, unsigned int bitMask);
		virtual unsigned int getGPIO_ValueVector(const Bus &bus);
		virtual void setGPIO_ValueVector(const Bus &bus, unsigned int valueVector, unsigned int bitMask);
		virtual unsigned char getEGPIO_NumberOfPins(const Bus &bus);
		virtual std::vector<unsigned char> getEGPIO_AvailableModes(const Bus &bus, unsigned char pinNumber);
		virtual unsigned char getEGPIO_CurrentMode(const Bus &bus, unsigned char pinNumber);
		virtual void setEGPIO_Mode(const Bus &bus, unsigned char pinNumber, unsigned char mode, float value);
		virtual unsigned int getEGPIO_OutputVector(const Bus &bus);
		virtual void setEGPIO_OutputVector(const Bus &bus, unsigned int outputVector, unsigned int bitMask);
		virtual float getEGPIO_Value(const Bus &bus, unsigned char pinNumber);
		virtual void setEGPIO_Value(const Bus &bus, unsigned char pinNumber, float value);
    };
  }
}

#endif /* OBPGPIOPROTOCOL_H */
