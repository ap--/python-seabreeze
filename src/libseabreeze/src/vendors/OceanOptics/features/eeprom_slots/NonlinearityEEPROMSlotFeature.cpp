/***************************************************//**
 * @file    NonlinearityEEPROMSlotFeature.cpp
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
#include "vendors/OceanOptics/features/eeprom_slots/NonlinearityEEPROMSlotFeature.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/Log.h"

#define __NONLINEARITY_SLOT_ORDER_ZERO  6
#define __NONLINEARITY_ORDER_SLOT       14

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

NonlinearityEEPROMSlotFeature::NonlinearityEEPROMSlotFeature() {

}

NonlinearityEEPROMSlotFeature::~NonlinearityEEPROMSlotFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
vector<double> *NonlinearityEEPROMSlotFeature::readNonlinearityCoefficients(
        const Protocol &protocol, const Bus &bus) {
    LOG(__FUNCTION__)

    int i;
    int order;
    int numberCoeffs;
    vector<double> *retval;

    try {
        /* Order of the polynomial is stored in slot 14 */
        order = (int)readLong(protocol, bus, __NONLINEARITY_ORDER_SLOT);
    } catch (NumberFormatException &nfe) {
        logger.error("Could not parse NLC order");
        throw FeatureException("Could not get nonlinearity polynomial order.");
    }

    /* Must add one to the order to include the 0th order coefficient */
    numberCoeffs = order + 1;
    retval = new vector<double>(numberCoeffs);

    for(i = 0; i < numberCoeffs; i++) {
        /* Nonlinearity coefficients are stored starting with intercept at slot 6 */
        try {
            /* This may throw a FeatureException or NumberFormatException */
            (*retval)[i] = readDouble(protocol, bus, i + __NONLINEARITY_SLOT_ORDER_ZERO);
        } catch (NumberFormatException &nfe) {
            logger.error("Could not parse NLC coeff");
            for(i = 0; i < numberCoeffs; i++) {
                /* Set the polynomial such that the correction is negated.
                 */
                if(0 == i) {
                    (*retval)[i] = 1.0;
                } else {
                    (*retval)[i] = 0.0;
                }
            }
            break;
        } catch (const exception &ex) {
            logger.error("Caught unknown exception reading NLC coeff");
            (*retval)[i] = 0;
        }
    }

    return retval;
}

FeatureFamily NonlinearityEEPROMSlotFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.NONLINEARITY_COEFFS;
}
