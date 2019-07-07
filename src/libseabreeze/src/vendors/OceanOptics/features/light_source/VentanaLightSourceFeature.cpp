/***************************************************//**
 * @file    VentanaLightSourceFeature.cpp
 * @date    April 2013
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
#include "vendors/OceanOptics/features/light_source/VentanaLightSourceFeature.h"

using namespace seabreeze;
using namespace std;

#define VENTANA_LASER_COUNT         1
#define VENTANA_LASER_MODULE_INDEX  0

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

VentanaLightSourceFeature::VentanaLightSourceFeature(vector<ProtocolHelper *> helpers)
        : LightSourceFeatureImpl(helpers, VENTANA_LASER_MODULE_INDEX, VENTANA_LASER_COUNT) {

}

VentanaLightSourceFeature::~VentanaLightSourceFeature() {
    /* Nothing to do here */
}

bool VentanaLightSourceFeature::initialize(const Protocol &protocol, const Bus &bus) {
    try {
        /* Attempt to query the state of the laser.  If this fails, then
         * there is no laser module.  The returned state of the laser is
         * discarded because it is really not important here -- the side
         * effect of the call succeeding or failing is what matters.
         */
        this->isLightSourceEnabled(protocol, bus, 0);
    } catch (FeatureException &fe) {
        return false;
    }
    return true;
}
