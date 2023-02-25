/***************************************************//**
 * @file    SaturationEEPROMSlotFeature_MayaPro.cpp
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

#include "vendors/OceanOptics/features/eeprom_slots/SaturationEEPROMSlotFeature_MayaPro.h"
#include <vector>

using namespace seabreeze;
using namespace std;

SaturationEEPROMSlotFeature_MayaPro::SaturationEEPROMSlotFeature_MayaPro(int slot) {
    this->saturationSlot = slot;
}

SaturationEEPROMSlotFeature_MayaPro::~SaturationEEPROMSlotFeature_MayaPro() {

}

unsigned int SaturationEEPROMSlotFeature_MayaPro::getSaturation(
        const Protocol &protocol, const Bus &bus) {

    unsigned int saturation;

    vector<unsigned char> *slot = readEEPROMSlot(protocol, bus, this->saturationSlot);

    if(NULL == slot || slot->size() < 8) {
        if(NULL != slot) {
            delete slot;
        }
        throw FeatureException("Unable to read EEPROM slot for saturation level");
    }

    saturation = ((*slot)[0] & 0x00FF)
                 | (((*slot)[1] & 0x00FF) << 8);

    delete slot;

    return saturation;
}
