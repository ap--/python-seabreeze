/***************************************************//**
 * @file    GainAdjustedSpectrometerFeature.cpp
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014-2016, Ocean Optics Inc
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
#include "vendors/OceanOptics/features/spectrometer/GainAdjustedSpectrometerFeature.h"

using namespace seabreeze;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

GainAdjustedSpectrometerFeature::GainAdjustedSpectrometerFeature(
                ProgrammableSaturationFeature *saturationFeature) {
    this->saturation = saturationFeature;
}

GainAdjustedSpectrometerFeature::~GainAdjustedSpectrometerFeature() {
    delete this->saturation;
}

unsigned int GainAdjustedSpectrometerFeature::getSaturationLevel() {
    try {
        unsigned int result = this->saturation->getSaturation();
        if(0 == result || result > (unsigned)this->maxIntensity) {
            /* The saturation setting was retrieved but appears to be invalid.
             * Use the max intensity instead.
             */
            return this->maxIntensity;
        }
        return result;
    } catch (const FeatureException &fe) {
        /* No valid saturation setting, so default to the max intensity */
        return this->maxIntensity;
    }
}

bool GainAdjustedSpectrometerFeature::initialize(const Protocol &proto, const Bus &bus) {

    bool result = this->saturation->initialize(proto, bus);


    if(false == result) {
        return false;
    }

    return OOISpectrometerFeature::initialize(proto, bus);
}
