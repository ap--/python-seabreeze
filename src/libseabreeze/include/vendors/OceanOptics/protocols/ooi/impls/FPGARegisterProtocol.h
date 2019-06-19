/***************************************************//**
 * @file    FPGARegisterProtocol.h
 * @date    October 2012
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

#ifndef SEABREEZE_FPGA_REGISTER_PROTOCOL_H
#define SEABREEZE_FPGA_REGISTER_PROTOCOL_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "vendors/OceanOptics/protocols/interfaces/FPGARegisterProtocolInterface.h"

namespace seabreeze {
  namespace ooiProtocol {
    class FPGARegisterProtocol : public FPGARegisterProtocolInterface {
    public:
        FPGARegisterProtocol();
        virtual ~FPGARegisterProtocol();
        virtual unsigned int readRegister(const Bus &bus, byte address)
                throw (ProtocolException);
        virtual void writeRegister(const Bus &bus, byte address,
                unsigned int value) throw (ProtocolException);
    };
  }
}

#endif
