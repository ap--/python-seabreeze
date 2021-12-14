/***************************************************//**
 * @file    EEPROMSlotFeatureBase.h
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

#ifndef EEPROMSLOTFEATUREBASE_H
#define EEPROMSLOTFEATUREBASE_H

#include "common/features/FeatureImpl.h"
#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/SeaBreeze.h"
#include "common/exceptions/FeatureException.h"
#include "common/exceptions/NumberFormatException.h"
#include "common/exceptions/IllegalArgumentException.h"
#include <vector>

namespace seabreeze {

    class EEPROMSlotFeatureBase : public FeatureImpl {
        /* Keeping most of this class protected to force use of the derived
         * classes, e.g. EEPROMSlotFeature, which can provide better control
         * over which slots are accessed.
         */

    protected:

        EEPROMSlotFeatureBase();
        virtual ~EEPROMSlotFeatureBase();
        virtual std::vector<unsigned char> *readEEPROMSlot(const Protocol &protocol,
                const Bus &bus, unsigned int slot);
        virtual int writeEEPROMSlot(const Protocol &protocol,
                const Bus &bus, unsigned int slot, const std::vector<unsigned char> &data);

        /* This is a utility function that reads out the given EEPROM slot and
         * parses it into a double value.  If for some reason the parse fails,
         * this will throw a NumberFormatException.
         */
        double readDouble(const Protocol &protocol, const Bus &bus,
                unsigned int slot);

        /* As with readDouble(), this will read a slot and parse into an integer */
        long readLong(const Protocol &protocol, const Bus &bus,
                unsigned int slot);

    };

}

#endif /* EEPROMSLOTFEATUREBASE_H */
