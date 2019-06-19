/***************************************************//**
 * @file    EEPROMProtocolInterface.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is a simple interface for any protocol to implement
 * that provides a protocol-agnostic mechanism for accessing
 * an EEPROM slot on an Ocean Optics device.
 *
 * This does not extend Protocol or otherwise get involved
 * in that hierarchy because it might interfere with the
 * lookup process for getting a Protocol object to delegate
 * these methods to.  Worse, it could end up inheriting
 * twice from the same base class, which is just messy.
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

#ifndef EEPROMPROTOCOLINTERFACE_H
#define EEPROMPROTOCOLINTERFACE_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"
#include <vector>

namespace seabreeze {

    class EEPROMProtocolInterface : public ProtocolHelper {
    public:
        EEPROMProtocolInterface(Protocol *protocol);
        virtual ~EEPROMProtocolInterface();
        virtual std::vector<byte> *readEEPROMSlot(const Bus &bus, int slot)
                throw (ProtocolException) = 0;
        virtual int writeEEPROMSlot(const Bus &bus, int slot,
                const std::vector<byte> &data) throw (ProtocolException) = 0;
    };

}

#endif /* EEPROMPROTOCOLINTERFACE_H */
