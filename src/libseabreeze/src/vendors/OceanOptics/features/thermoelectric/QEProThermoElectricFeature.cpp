/***************************************************//**
 * @file    QEProThermoElectricFeature.cpp
 * @date    September 2013
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the thermo-
 * electric unit (TEC) on the QE-PRO.
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

#include "vendors/OceanOptics/features/thermoelectric/QEProThermoElectricFeature.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPThermoElectricProtocol.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;

static const double defaultTESetpointCelsius = -10;
static const bool defaultTEEnable = true;

QEProThermoElectricFeature::QEProThermoElectricFeature() {
    this->protocols.push_back(new OBPThermoElectricProtocol());
}

QEProThermoElectricFeature::~QEProThermoElectricFeature() {

}

double QEProThermoElectricFeature::getDefaultSetPointCelsius(const Protocol &protocol,
                const Bus &bus) throw (FeatureException) {
    return defaultTESetpointCelsius;
}

bool QEProThermoElectricFeature::getDefaultThermoElectricEnable(const Protocol &protocol,
                const Bus &bus) throw (FeatureException) {
    return defaultTEEnable;
}

bool QEProThermoElectricFeature::initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException) {
    /* This doesn't need to do anything special at this point.  If desired, this
     * could be used to read out the current setting and cache it, but that is
     * not strictly required.
     */
    return true;
}
