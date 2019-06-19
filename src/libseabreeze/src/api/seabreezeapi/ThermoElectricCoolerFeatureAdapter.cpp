/***************************************************//**
 * @file    ThermoElectricCoolerFeatureAdapter.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze TECFeatureInterface instances.
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
#include "api/seabreezeapi/ThermoElectricCoolerFeatureAdapter.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

using namespace seabreeze;
using namespace seabreeze::api;

ThermoElectricCoolerFeatureAdapter::ThermoElectricCoolerFeatureAdapter(
        ThermoElectricFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex)
        : FeatureAdapterTemplate<ThermoElectricFeatureInterface>(intf, f, p, b, instanceIndex) {

}

ThermoElectricCoolerFeatureAdapter::~ThermoElectricCoolerFeatureAdapter() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
double ThermoElectricCoolerFeatureAdapter::readTECTemperature(int *errorCode) {
    double retval;

    try {
        retval = this->feature->getTemperatureCelsius(
            *this->protocol, *this->bus);
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    return 0;
}

void ThermoElectricCoolerFeatureAdapter::setTECTemperature(int *errorCode,
        double temperature_degrees_celsius) {

    try {
        this->feature->setTemperatureSetPointCelsius(*this->protocol, *this->bus,
                temperature_degrees_celsius);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    } catch (IllegalArgumentException &iae) {
        SET_ERROR_CODE(ERROR_INVALID_ERROR);
        return;
    }

    return;
}

void ThermoElectricCoolerFeatureAdapter::setTECEnable(int *errorCode,
        bool tec_enable) {
    try {
        this->feature->setThermoElectricEnable(*this->protocol, *this->bus, tec_enable);
        SET_ERROR_CODE(ERROR_SUCCESS);
    } catch (FeatureException &fe) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return;
    }

    return;
}

void ThermoElectricCoolerFeatureAdapter::setTECFanEnable(int *errorCode,
        bool tec_fan_enable) {

    SET_ERROR_CODE(ERROR_NOT_IMPLEMENTED);
    /* FIXME: MISSING_IMPL */
    return;
}
