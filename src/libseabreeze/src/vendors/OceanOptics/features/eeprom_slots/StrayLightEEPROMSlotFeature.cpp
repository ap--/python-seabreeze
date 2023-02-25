/***************************************************//**
 * @file    StrayLightEEPROMSlotFeature.cpp
 * @date    February 2012
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
#include <stdlib.h>
#include <errno.h>
#include "common/exceptions/FeatureControlException.h"
#include "vendors/OceanOptics/features/eeprom_slots/StrayLightEEPROMSlotFeature.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/Log.h"
#include <sstream>

#define __STRAY_LIGHT_EEPROM_SLOT 5

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

StrayLightEEPROMSlotFeature::StrayLightEEPROMSlotFeature() {

}

StrayLightEEPROMSlotFeature::~StrayLightEEPROMSlotFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
vector<double> *StrayLightEEPROMSlotFeature::readStrayLightCoefficients(
        const Protocol &protocol, const Bus &bus) {
    LOG(__FUNCTION__);

    unsigned int i, j, k;
    int numberCoeffs;
    vector<double> *retval;
    vector<unsigned char> *rawSlot = NULL;
    char buffer[20] = { 0 };
    double temp;

    try {
        /* This may throw an exception -- if it does, don't catch it here. */
        rawSlot = readEEPROMSlot(protocol, bus, __STRAY_LIGHT_EEPROM_SLOT);
    } catch (IllegalArgumentException &iae) {
        throw FeatureException("Internal error: stray light slot invalid.");
    }

    if(NULL == rawSlot) {
        string error("Could not read EEPROM slot for stray light.");
        throw FeatureControlException(error);
    }

    numberCoeffs = 1;

    /* It is possible to store two stray light coefficients into a single
     * slot by separating them with a NULL terminator.  This will try to
     * identify when a second term is available.
     */
    for(i = 0; i < rawSlot->size() - 1; i++) {
        if('\0' == (*rawSlot)[i] &&
                ('\0' != (*rawSlot)[i+1] && 0xFF != (*rawSlot)[i+1])) {
            /* There appears to be a second constant after the null terminator
             * at the end of the first coefficient.
             */
            numberCoeffs++;
            for(j = i + 1, k = 0; j < rawSlot->size() && k < 20; j++, k++) {
                /* Copy the second coefficient into an auxiliary buffer */
                buffer[k] = (*rawSlot)[j];
            }
            /* Make sure this is null-terminated */
            buffer[19] = '\0';
            break;
        }
    }

    delete rawSlot;

    retval = new vector<double>(numberCoeffs);
    try {
        (*retval)[0] = readDouble(protocol, bus, __STRAY_LIGHT_EEPROM_SLOT);
    } catch (NumberFormatException &nfe) {
        (*retval)[0] = 0;
    }

    if(numberCoeffs > 1) {

        /* buffer should be set up with the other term.  Read it out in a way
         * that allows for error checking.
         */
        try
        {
            std::istringstream istr(buffer);
            istr >> temp;
        }
        catch (const exception &ex)
        {
            logger.error("Error converting NLC coefficient [%s] to double", buffer);
            temp = 0;
        }
        (*retval)[1] = temp;
    }

    return retval;
}

FeatureFamily StrayLightEEPROMSlotFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.STRAY_LIGHT_COEFFS;
}
