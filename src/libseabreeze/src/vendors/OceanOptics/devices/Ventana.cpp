/***************************************************//**
 * @file    Ventana.cpp
 * @date    January 2013
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
#include "api/seabreezeapi/FeatureFamilies.h"
#include "api/seabreezeapi/ProtocolFamilies.h"
#include "common/buses/BusFamilies.h"
#include "vendors/OceanOptics/devices/Ventana.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPSerialNumberProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPNonlinearityCoeffsProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPStrayLightCoeffsProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPLightSourceProtocol_Ventana.h"
#include "vendors/OceanOptics/buses/usb/VentanaUSB.h"
#include "vendors/OceanOptics/features/spectrometer/VentanaSpectrometerFeature.h"
#include "vendors/OceanOptics/features/serial_number/SerialNumberFeature.h"
#include "vendors/OceanOptics/features/nonlinearity/NonlinearityCoeffsFeature.h"
#include "vendors/OceanOptics/features/stray_light/StrayLightCoeffsFeature.h"
#include "vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeature.h"
#include "vendors/OceanOptics/features/thermoelectric/VentanaThermoElectricFeature.h"
#include "vendors/OceanOptics/features/light_source/VentanaLightSourceFeature.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

Ventana::Ventana() {

    this->name = "Ventana";
        // 0 is the control address, since it is not valid in this context, means not used
    this->usbEndpoint_primary_out = 0x01;
    this->usbEndpoint_primary_in = 0x82;
    this->usbEndpoint_secondary_out = 0;
    this->usbEndpoint_secondary_in = 0;
    this->usbEndpoint_secondary_in2 = 0;

    /* Set up the available buses on this device */
    this->buses.push_back(new VentanaUSB());

    /* Set up the available protocols understood by this device */
    this->protocols.push_back(new OceanBinaryProtocol());

    /* Set up the features that comprise this device */
    this->features.push_back(new VentanaSpectrometerFeature());

    /* Add serial number feature */
    vector<ProtocolHelper *> serialNumberHelpers;
    serialNumberHelpers.push_back(new OBPSerialNumberProtocol());
    this->features.push_back(new SerialNumberFeature(serialNumberHelpers));

    /* Add nonlinearity coefficients feature */
    vector<ProtocolHelper *> nonlinearityHelpers;
    nonlinearityHelpers.push_back(new OBPNonlinearityCoeffsProtocol());
    this->features.push_back(
        new NonlinearityCoeffsFeature(nonlinearityHelpers));

    /* Add stray light coefficients feature */
    vector<ProtocolHelper *> strayHelpers;
    strayHelpers.push_back(new OBPStrayLightCoeffsProtocol());
    this->features.push_back(new StrayLightCoeffsFeature(strayHelpers));

    this->features.push_back(new RawUSBBusAccessFeature());
}

Ventana::~Ventana() {
}

bool Ventana::initialize(const Bus &bus) {

    FeatureFamilies featureFamilies;
    ProtocolFamilies protocolFamilies;

    /* Some Ventana systems have a thermoelectric unit.  This can be detected
     * by attempting to communicate with it and checking whether an error
     * comes back.
     */
    VentanaThermoElectricFeature *thermoFeature = NULL;
    Protocol *thermoProtocol = NULL;

    ProtocolFamily family = getSupportedProtocol(
        featureFamilies.THERMOELECTRIC, bus.getBusFamily());

    if(family.equals(protocolFamilies.OCEAN_BINARY_PROTOCOL)) {
        thermoProtocol = new OceanBinaryProtocol();
    } /* Could check for other protocol matches here */

    if(NULL != thermoProtocol) {
        /* Found a usable protocol */
        thermoFeature = new VentanaThermoElectricFeature();
        try {
             /* May throw FeatureException */
            thermoFeature->getTemperatureCelsius(*thermoProtocol, bus);

            /* If an exception was not thrown, add this feature.  It will get
             * initialized with the other Feature instances once this is done.
             */
            this->features.push_back(thermoFeature);
        } catch (const FeatureException &fe) {
            /* The attempt to get the temperature failed, so this feature is
             * probably not present on the hardware.  Clean up.
             */
            delete thermoFeature;
        }

        /* Done probing with this protocol, so delete it */
        delete thermoProtocol;
    }

    /* Check for laser module */
    VentanaLightSourceFeature *laser = NULL;
    Protocol *laserProtocol = NULL;
    bool laserFound = false;
    vector<ProtocolHelper *> laserHelpers;

    family = getSupportedProtocol(
        featureFamilies.LIGHT_SOURCE, bus.getBusFamily());

    if(family.equals(protocolFamilies.OCEAN_BINARY_PROTOCOL)) {
        laserProtocol = new OceanBinaryProtocol();
        laserHelpers.push_back(new OBPLightSourceProtocol_Ventana());
    } /* Could check for other protocol matches here */

    if(NULL != laserProtocol) {
        /* Found a usable protocol */
        laser = new VentanaLightSourceFeature(laserHelpers);
        try {
            /* May throw FeatureException */
            laserFound = laser->initialize(*laserProtocol, bus);
            if(true == laserFound) {
                this->features.push_back(laser);
            } else {
                /* The instance is not needed so clean it up. */
                delete laser;
            }
        } catch (const FeatureException &fe) {
            /* Attempting to initialize the laser failed, so the hardware may
             * not be present.  Clean up.
             */
            delete laser;
        }

        /* Done probing with this protocol, so delete it */
        delete laserProtocol;
    }

    /* Be sure to allow the rest of the initialization (especially of the other
     * Feature instances) to take place regardless.
     */
    return Device::initialize(bus);
}

ProtocolFamily Ventana::getSupportedProtocol(FeatureFamily family, BusFamily bus) {
    ProtocolFamilies protocolFamilies;
    BusFamilies busFamilies;

    if(bus.equals(busFamilies.USB)) {
        /* This device only supports one protocol over USB. */
        return protocolFamilies.OCEAN_BINARY_PROTOCOL;
    }

    /* No other combinations of buses and protocols are supported. */
    return protocolFamilies.UNDEFINED_PROTOCOL;
}
