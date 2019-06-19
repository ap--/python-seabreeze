/***************************************************//**
 * @file    VentanaThermoElectricFeature.cpp
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
#include "vendors/OceanOptics/features/thermoelectric/VentanaThermoElectricFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/ThermoElectricProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPThermoElectricProtocol.h"
#include "common/exceptions/FeatureControlException.h"

#define VENTANA_DEFAULT_TE_SETPOINT_CELSIUS     15.0
#define VENTANA_DEFAULT_TE_ENABLE               false

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

VentanaThermoElectricFeature::VentanaThermoElectricFeature() {

    this->protocols.push_back(new OBPThermoElectricProtocol());
}

VentanaThermoElectricFeature::~VentanaThermoElectricFeature() {

}

double VentanaThermoElectricFeature::getDefaultSetPointCelsius(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {

    return VENTANA_DEFAULT_TE_SETPOINT_CELSIUS;
}

bool VentanaThermoElectricFeature::getDefaultThermoElectricEnable(
        const Protocol &protocol, const Bus &bus) throw (FeatureException) {

    return VENTANA_DEFAULT_TE_ENABLE;
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
bool VentanaThermoElectricFeature::initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException) {

    try {
        /* Try to detect whether this kind of Ventana has a thermoelectric
         * unit by attempting to read from it.  If this throws an exception
         * then the device does not have this feature.
         */
        getTemperatureCelsius(protocol, bus);

        /* Apply the power-on defaults */
        bool enable = getDefaultThermoElectricEnable(protocol, bus);
        double temp = getDefaultSetPointCelsius(protocol, bus);
        setThermoElectricEnable(protocol, bus, enable);
        setTemperatureSetPointCelsius(protocol, bus, temp);
    } catch (FeatureException &fe) {
        return false;
    } catch (IllegalArgumentException &iae) {
        return false;
    }

    return true;
}
