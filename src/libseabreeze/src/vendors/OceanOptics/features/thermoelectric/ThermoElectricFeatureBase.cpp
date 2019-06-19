/***************************************************//**
 * @file    ThermoElectricFeatureBase.cpp
 * @date    March 2013
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
#include "vendors/OceanOptics/features/thermoelectric/ThermoElectricFeatureBase.h"
#include "vendors/OceanOptics/protocols/interfaces/ThermoElectricProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

ThermoElectricFeatureBase::ThermoElectricFeatureBase() {

}

ThermoElectricFeatureBase::~ThermoElectricFeatureBase() {

}

void ThermoElectricFeatureBase::setThermoElectricEnable(const Protocol &protocol,
        const Bus &bus, bool enable) throw (FeatureException) {

    ThermoElectricProtocolInterface *te = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        te = static_cast<ThermoElectricProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException fpnfe) {
        string error(
                "Could not find matching protocol implementation to set thermoelectric state.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        te->writeThermoElectricEnable(bus, enable);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

double ThermoElectricFeatureBase::getTemperatureCelsius(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {

    ThermoElectricProtocolInterface *te = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        te = static_cast<ThermoElectricProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException fpnfe) {
        string error(
                "Could not find matching protocol implementation to read TE temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    double retval = 0.0;

    try {
        retval = te->readThermoElectricTemperatureCelsius(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

void ThermoElectricFeatureBase::setTemperatureSetPointCelsius(const Protocol &protocol,
        const Bus &bus, double degreesC) throw (FeatureException, IllegalArgumentException) {

    ThermoElectricProtocolInterface *te = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        te = static_cast<ThermoElectricProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException fpnfe) {
        string error(
                "Could not find matching protocol implementation to set TEC temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    /* FIXME: do bounds checking on the range in Celsius and throw an exception
     * if not in bounds.
     */
    try {
        te->writeThermoElectricSetPointCelsius(bus, degreesC);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

FeatureFamily ThermoElectricFeatureBase::getFeatureFamily() {
    FeatureFamilies families;

    return families.THERMOELECTRIC;
}
