/***************************************************//**
 * @file    WavelengthEEPROMSlotFeature_QE65000.cpp
 * @date    March 2013
 * @author  Ocean Optics, Inc.
 *
 * The QE65000 wavelength calibration is based around pixel 10 on the detector,
 * where in most other spectrometers the calibration is based around pixel zero.
 * This is due to certain legacy issues: the first generation of QE65000 drivers
 * did not show the first ten pixels and this became the basis for all
 * wavelength calibrations.  This class simply alters how the wavelength
 * calibration is applied so that this mapping is accounted for.
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
#include "vendors/OceanOptics/features/eeprom_slots/WavelengthEEPROMSlotFeature_QE65000.h"
#include "vendors/OceanOptics/utils/Polynomial.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

WavelengthEEPROMSlotFeature_QE65000::WavelengthEEPROMSlotFeature_QE65000(
            unsigned int numPixels) : WavelengthEEPROMSlotFeature(numPixels) {
}

WavelengthEEPROMSlotFeature_QE65000::~WavelengthEEPROMSlotFeature_QE65000() {

}

/* This function has to be overridden to make the intercept of the
 * calibration polynomial correspond to detector pixel 10.  This is due to
 * some legacy calibration issues.
 */
vector<double> *WavelengthEEPROMSlotFeature_QE65000::computeWavelengths(
        double polynomial[], int length) {

    vector<double> *retval = new vector<double>(this->numberOfPixels);
    unsigned int i;

    Polynomial<double> *calibration = new Polynomial<double>(polynomial, length);

    /* Evaluate the given polynomial to generate the wavelength array. */
    for(i = 0; i < this->numberOfPixels; i++) {
        /* Shift the evaluation by ten pixels */
        (*retval)[i] = calibration->evaluate(i - 10.0);
    }
    delete calibration;
    return retval;
}
