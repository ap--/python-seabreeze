/***************************************************//**
 * @file    SaturationEEPROMSlotFeature_EEPROM.h
 * @date    March 2016
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2016, Ocean Optics Inc
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

#ifndef SATURATIONEEPROMSLOTFEATURE_EEPROM_H
#define SATURATIONEEPROMSLOTFEATURE_EEPROM_H

#include "vendors/OceanOptics/features/eeprom_slots/SaturationEEPROMSlotFeatureBase.h"

namespace seabreeze {

    /* This class is intended for most devices that store their saturation level
     * in EEPROM in the so-called "autonulling" configuration.  Note that the
     * NIRQuest, MayaPro, Apex and some others do not do things quite the same
     * way, so they should not use this class.
     */
    class SaturationEEPROMSlotFeature
            : public SaturationEEPROMSlotFeatureBase {
    public:
        SaturationEEPROMSlotFeature(int slot);
        virtual ~SaturationEEPROMSlotFeature();

    protected:
        /* Inherited from SaturationEEPROMSlotFeatureBase */
        virtual unsigned int getSaturation(const Protocol &protocol,
                const Bus &bus);

    private:
        int autonullingSlot;
    };

} /* end namespace seabreeze */

#endif /* SATURATIONEEPROMSLOTFEATURE_EEPROM_H */
