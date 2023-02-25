/***************************************************//**
 * @file    EEPROMSlotFeature.cpp
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

#include "common/globals.h"
#include "vendors/OceanOptics/features/eeprom_slots/EEPROMSlotFeature.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

EEPROMSlotFeature::EEPROMSlotFeature(unsigned int numSlots) {
    this->numberOfSlots = numSlots;
}

EEPROMSlotFeature::~EEPROMSlotFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
vector< vector<unsigned char> * > *EEPROMSlotFeature::readAllEEPROMSlots(
        const Protocol &protocol, const Bus &bus) {

    unsigned int i;
    vector< vector<unsigned char> * > *retval = new vector< vector<unsigned char> * >();

    for(i = 0; i < this->numberOfSlots; i++) {
        try {
            /* This may throw a FeatureException */
            retval[i].push_back(readEEPROMSlot(protocol, bus, i));
        } catch (IllegalArgumentException &iae) {
            /* This shouldn't be possible since the loop is enumerating
             * the known range of slots, but recover anyway
             */
            continue;
        }
    }

    return retval;
}

vector<unsigned char> *EEPROMSlotFeature::readEEPROMSlot(const Protocol &protocol,
        const Bus &bus, unsigned int slot) {

    if(slot >= this->numberOfSlots) {
        string error("EEPROM slot out of bounds.");
        throw IllegalArgumentException(error);
    }

    /* This may throw a FeatureException. */
    return EEPROMSlotFeatureBase::readEEPROMSlot(protocol, bus, slot);
}

int EEPROMSlotFeature::writeEEPROMSlot(const Protocol &protocol,
        const Bus &bus, unsigned int slot, const vector<unsigned char> &data) {

    if(slot >= this->numberOfSlots) {
        throw IllegalArgumentException(string("EEPROM slot out of bounds."));
    }

    return EEPROMSlotFeatureBase::writeEEPROMSlot(protocol, bus, slot, data);
}

FeatureFamily EEPROMSlotFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.EEPROM;
}
