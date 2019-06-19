/***************************************************//**
 * @file    WavelengthEEPROMSlotFeature.cpp
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
#include "vendors/OceanOptics/features/eeprom_slots/WavelengthEEPROMSlotFeature.h"
#include "vendors/OceanOptics/utils/Polynomial.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

WavelengthEEPROMSlotFeature::WavelengthEEPROMSlotFeature(unsigned int numPixels) {
    this->numberOfPixels = numPixels;
}

WavelengthEEPROMSlotFeature::~WavelengthEEPROMSlotFeature() {

}

vector<double> *WavelengthEEPROMSlotFeature::readWavelengths(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {

    double polynomial[4]; /* order of term equals index of term */
    int i;

    for(i = 0; i < 4; i++) {
        /* Wavelength coefficients are stored starting with intercept at slot 1 */
        try {
            /* This may throw a FeatureException or NumberFormatException */
            polynomial[i] = readDouble(protocol, bus, i + 1);
        } catch (NumberFormatException &nfe) {
            /* FIXME: If there were some sort of logging mechanism, this would
             * be a good thing to warn about.
             */
            for(i = 0; i < 4; i++) {
                /* Set the polynomial such that the reported wavelength equals
                 * the pixel number.
                 */
                if(1 == i) {
                    polynomial[i] = 1.0;
                } else {
                    polynomial[i] = 0.0;
                }
            }
            break;
        }
    }

    return computeWavelengths(polynomial, 4);
}

/* Note: this should probably be broken out or delegated to a more generic
 * polynomial evaluation function that can be reused.
 */
vector<double> *WavelengthEEPROMSlotFeature::computeWavelengths(
        double polynomial[], int length) {

    vector<double> *retval = new vector<double>(this->numberOfPixels);
    unsigned int i;

    Polynomial<double> *calibration = new Polynomial<double>(polynomial, length);

    /* Evaluate the given polynomial to generate the wavelength array. */
    for(i = 0; i < this->numberOfPixels; i++) {
        (*retval)[i] = calibration->evaluate(i);
    }
    delete calibration;
    return retval;
}

FeatureFamily WavelengthEEPROMSlotFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.WAVELENGTH_CAL;
}
