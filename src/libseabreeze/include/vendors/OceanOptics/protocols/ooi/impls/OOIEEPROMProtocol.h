/***************************************************//**
 * @file    OOIEEPROMProtocol.h
 * @date    February 2009
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

#ifndef OOIEEPROMPROTOCOL_H
#define OOIEEPROMPROTOCOL_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "vendors/OceanOptics/protocols/interfaces/EEPROMProtocolInterface.h"
#include <vector>

namespace seabreeze {
  namespace ooiProtocol {
    class OOIEEPROMProtocol : public EEPROMProtocolInterface {
    public:
        OOIEEPROMProtocol();
        virtual ~OOIEEPROMProtocol();
        virtual std::vector<byte> *readEEPROMSlot(const Bus &bus, int slot)
                throw (ProtocolException);
        virtual int writeEEPROMSlot(const Bus &bus, int slot,
                const std::vector<byte> &data) throw (ProtocolException);
    };
  }
}

#endif /* OOIEEPROMPROTOCOL_H */
