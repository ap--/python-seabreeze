/***************************************************//**
 * @file    SaturationEEPROMSlotFeature.cpp
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

#include "vendors/OceanOptics/features/eeprom_slots/SaturationEEPROMSlotFeature.h"
#include <vector>

using namespace seabreeze;
using namespace std;

SaturationEEPROMSlotFeature::SaturationEEPROMSlotFeature(int slot) {
    this->autonullingSlot = slot;
}

SaturationEEPROMSlotFeature::~SaturationEEPROMSlotFeature() {
    
}

unsigned int SaturationEEPROMSlotFeature::getSaturation(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {
    
    unsigned int saturation;

    vector<byte> *slot = readEEPROMSlot(protocol, bus, this->autonullingSlot);
    
    if(NULL == slot || slot->size() < 6) {
        if(NULL != slot) {
            delete slot;
        }
        throw FeatureException("Unable to read EEPROM slot for saturation level");
    }

    saturation = ((*slot)[4] & 0x00FF) | (((*slot)[5] & 0x00FF) << 8);

    delete slot;
    
    return saturation;
}
        