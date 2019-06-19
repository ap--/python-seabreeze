/***************************************************//**
 * @file    ThermoElectricQEFeature.cpp
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
#include "vendors/OceanOptics/features/thermoelectric/ThermoElectricQEFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/ThermoElectricProtocolInterface.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOITECProtocol.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/TECQEEnableExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/WriteTECQESetPointExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadTECQETemperatureExchange.h"
#include "common/exceptions/FeatureControlException.h"
#include "vendors/OceanOptics/features/eeprom_slots/EEPROMSlotFeature.h"
#include "vendors/OceanOptics/protocols/ooi/constants/QETECConstants.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace std;

ThermoElectricQEFeature::ThermoElectricQEFeature() {

    TECEnableExchange *enableExchange =
            new TECQEEnableExchange();
    WriteTECSetPointExchange *writeSetPointExchange =
            new WriteTECQESetPointExchange();
    ReadTECTemperatureExchange *readTECTempExchange =
            new ReadTECQETemperatureExchange();

    OOITECProtocol *ooiProtocol =
            new OOITECProtocol(enableExchange, writeSetPointExchange,
                readTECTempExchange);

    this->protocols.push_back(ooiProtocol);
}

ThermoElectricQEFeature::~ThermoElectricQEFeature() {

}

vector<byte> *ThermoElectricQEFeature::readTECDefaults(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {

    /* TEC defaults are stored in EEPROM slot 0x11 (17) on the QE65000
     * and the NIRQuest256 and NIRQuest512.  Note that the NIRQuest stores
     * other information in that same slot, so when it comes time to write
     * to that slot, the NIRQuest MUST have its own implementation.  At
     * that point, either this class needs to be extended or split up
     * into a common base class with QE and NIRQuest derived classes to
     * handle that difference.
     */
    EEPROMSlotFeature eeprom(18);

    /* Note: this may throw a FeatureException. */
    vector<byte> *data = eeprom.readEEPROMSlot(protocol, bus, 0x11);

    if((((*data)[0] & 0xFE) != 0) || (((*data)[1] & 0xFE) != 0)) {
        /* If programmed, only one bit for each of the first two bytes will
         * be used.  The rest should be cleared.  If the EEPROM is not programmed,
         * it will tend to return 0xFF for those bytes.
         */
        string error("No default TEC settings found.");
        throw FeatureException(error);
    }

    /* This data will have the following format:
     * Byte 0: TEC enable (0 or 1)
     * Byte 1: Fan enable (0 or 1)
     * Bytes 2, 3: temperature as signed short, LSB MSB
     */
    return data;
}

double ThermoElectricQEFeature::getDefaultSetPointCelsius(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {

    vector<byte> *defs = readTECDefaults(protocol, bus);

    signed short temp = (((*defs)[3] << 8) | (*defs)[2]);

    delete defs;

    double retval = temp / (1.0 * QE_TEC_COUNTS_PER_DEGREE_C);
    return retval;
}

bool ThermoElectricQEFeature::getDefaultThermoElectricEnable(
        const Protocol &protocol, const Bus &bus) throw (FeatureException) {

    vector<byte> *defs = readTECDefaults(protocol, bus);

    bool retval;
    retval = ((0 == (*defs)[0]) ? false : true);

    delete defs;

    return retval;
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
bool ThermoElectricQEFeature::initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException) {

    try {
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
