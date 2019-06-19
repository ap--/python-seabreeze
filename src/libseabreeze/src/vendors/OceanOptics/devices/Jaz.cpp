/***************************************************//**
 * @file    Jaz.cpp
 * @date    November 2011
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

#include <vector>

#include "common/globals.h"
#include "api/seabreezeapi/ProtocolFamilies.h"
#include "common/buses/BusFamilies.h"
#include "vendors/OceanOptics/devices/Jaz.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOIProtocol.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOIStrobeLampProtocol.h"
#include "vendors/OceanOptics/buses/network/JazTCPIPv4.h"
#include "vendors/OceanOptics/buses/usb/JazUSB.h"
#include "vendors/OceanOptics/features/eeprom_slots/EEPROMSlotFeature.h"
#include "vendors/OceanOptics/features/eeprom_slots/WavelengthEEPROMSlotFeature.h"
#include "vendors/OceanOptics/features/eeprom_slots/SaturationEEPROMSlotFeature.h"
#include "vendors/OceanOptics/features/eeprom_slots/SerialNumberEEPROMSlotFeature.h"
#include "vendors/OceanOptics/features/eeprom_slots/NonlinearityEEPROMSlotFeature.h"
#include "vendors/OceanOptics/features/eeprom_slots/StrayLightEEPROMSlotFeature.h"
#include "vendors/OceanOptics/features/light_source/StrobeLampFeature.h"
#include "vendors/OceanOptics/features/spectrometer/JazSpectrometerFeature.h"
#include "vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeature.h"

using namespace std;
using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace seabreeze::api;

Jaz::Jaz() {

    this->name = "Jaz";
        
    // 0 is the control address, since it is not valid in this context, means not used
    this->usbEndpoint_primary_out = 0x01;
    this->usbEndpoint_primary_in = 0x81;
    this->usbEndpoint_secondary_out = 0;
    this->usbEndpoint_secondary_in = 0x82;
    this->usbEndpoint_secondary_in2 = 0;

    /* Set up the available buses on this device */
    this->buses.push_back(new JazUSB());
    this->buses.push_back(new JazTCPIPv4());

    /* Set up the available protocols understood by this device */
    this->protocols.push_back(new OOIProtocol());

    /* Set up the features that comprise this device */
    
    ProgrammableSaturationFeature *saturation =
            new SaturationEEPROMSlotFeature(0x0011);
    
    this->features.push_back(new JazSpectrometerFeature(saturation));
    this->features.push_back(new SerialNumberEEPROMSlotFeature());
    this->features.push_back(new EEPROMSlotFeature(17));

    vector<ProtocolHelper *> strobeLampHelpers;
    strobeLampHelpers.push_back(new OOIStrobeLampProtocol());
    this->features.push_back(new StrobeLampFeature(strobeLampHelpers));

    this->features.push_back(new NonlinearityEEPROMSlotFeature());
    this->features.push_back(new StrayLightEEPROMSlotFeature());
    this->features.push_back(new RawUSBBusAccessFeature());
}

Jaz::~Jaz() {
}

ProtocolFamily Jaz::getSupportedProtocol(FeatureFamily family, BusFamily bus) {
    ProtocolFamilies protocols;
    BusFamilies busFamilies;

    if(bus.equals(busFamilies.USB) || bus.equals(busFamilies.TCPIPv4)) {
        return protocols.OOI_PROTOCOL;
    }

    /* No other combinations of buses and protocols are supported. */
    /* TODO: add support for Jaz Messaging protocol.  This can be done over
     * both Ethernet and USB.  This is only required for certain features.
     */
    return protocols.UNDEFINED_PROTOCOL;
}
