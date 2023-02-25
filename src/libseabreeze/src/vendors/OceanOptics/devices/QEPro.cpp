/***************************************************//**
 * @file    QEPro.cpp
 * @date    May 2017
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
#include "api/seabreezeapi/ProtocolFamilies.h"
#include "common/buses/BusFamilies.h"
#include "vendors/OceanOptics/devices/QEPro.h"
#include "vendors/OceanOptics/buses/rs232/OOIRS232Interface.h"
#include "vendors/OceanOptics/buses/usb/QEProUSB.h"

#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPIrradCalProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPStrobeLampProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPSerialNumberProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPNonlinearityCoeffsProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPStrayLightCoeffsProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPShutterProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPContinuousStrobeProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPI2CMasterProtocol.h"

#include "vendors/OceanOptics/features/spectrometer/QEProSpectrometerFeature.h"
#include "vendors/OceanOptics/features/light_source/StrobeLampFeature.h"
#include "vendors/OceanOptics/features/data_buffer/QEProDataBufferFeature.h"
#include "vendors/OceanOptics/features/serial_number/SerialNumberFeature.h"
#include "vendors/OceanOptics/features/nonlinearity/NonlinearityCoeffsFeature.h"
#include "vendors/OceanOptics/features/stray_light/StrayLightCoeffsFeature.h"
#include "vendors/OceanOptics/features/shutter/ShutterFeature.h"
#include "vendors/OceanOptics/features/continuous_strobe/ContinuousStrobeFeature.h"
#include "vendors/OceanOptics/features/irradcal/IrradCalFeature.h"
#include "vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeature.h"
#include "vendors/OceanOptics/features/thermoelectric/QEProThermoElectricFeature.h"
#include "vendors/OceanOptics/features/i2c_master/i2cMasterFeature.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace seabreeze::api;
using namespace std;

QEPro::QEPro() {

    this->name = "QE-PRO";

    // 0 is the control address, since it is not valid in this context, means not used
    this->usbEndpoint_primary_out = 0x01;
    this->usbEndpoint_primary_in = 0x81;
    this->usbEndpoint_secondary_out = 0x02;
    this->usbEndpoint_secondary_in = 0x82;
    this->usbEndpoint_secondary_in2 = 0;

    /* Set up the available buses on this device */
    this->buses.push_back(new QEProUSB());
    this->buses.push_back(new OOIRS232Interface());

    /* Set up the available protocols understood by this device */
    this->protocols.push_back(new OceanBinaryProtocol());

    /* Set up the features that comprise this device */
    this->features.push_back(new QEProSpectrometerFeature());
    this->features.push_back(new QEProDataBufferFeature());

    /* Add serial number feature */
    vector<ProtocolHelper *> serialNumberHelpers;
    serialNumberHelpers.push_back(new OBPSerialNumberProtocol());
    this->features.push_back(new SerialNumberFeature(serialNumberHelpers));

    /* This creates a specific ProtocolHelper that this device can use to
     * handle irradiance calibration.  This makes for better code reuse
     * and allows devices to support a given feature through multiple protocols.
     */
    OBPIrradCalProtocol *obpIrrad = new OBPIrradCalProtocol(1044);
    vector<ProtocolHelper *> irradHelpers;
    irradHelpers.push_back(obpIrrad);
    this->features.push_back(new IrradCalFeature(irradHelpers, 1044));

    /* Add nonlinearity coefficients feature */
    vector<ProtocolHelper *> nonlinearityHelpers;
    nonlinearityHelpers.push_back(new OBPNonlinearityCoeffsProtocol());
    this->features.push_back(
        new NonlinearityCoeffsFeature(nonlinearityHelpers));

    /* Add stray light coefficients feature */
    vector<ProtocolHelper *> strayHelpers;
    strayHelpers.push_back(new OBPStrayLightCoeffsProtocol());
    this->features.push_back(new StrayLightCoeffsFeature(strayHelpers));

    /* Add thermoelectric feature */
    this->features.push_back(new QEProThermoElectricFeature());

    /* Add continuous strobe feature */
    vector<ProtocolHelper *> contStrobeHelpers;
    contStrobeHelpers.push_back(new OBPContinuousStrobeProtocol());
    this->features.push_back(new ContinuousStrobeFeature(contStrobeHelpers));

    /* Add lamp enable feature */
    vector<ProtocolHelper *> lampHelpers;
    lampHelpers.push_back(new OBPStrobeLampProtocol());
    this->features.push_back(new StrobeLampFeature(lampHelpers));

	/* Add i2c master feature */
	vector<ProtocolHelper *> i2cMasterHelpers;
	i2cMasterHelpers.push_back(new OBPI2CMasterProtocol());
	this->features.push_back(new i2cMasterFeature(i2cMasterHelpers));

    this->features.push_back(new RawUSBBusAccessFeature());
}

QEPro::~QEPro() {
}

ProtocolFamily QEPro::getSupportedProtocol(FeatureFamily family, BusFamily bus) {
    ProtocolFamilies protocols;

    /* The QEPro uses one protocol for all buses. */
    return protocols.OCEAN_BINARY_PROTOCOL;
}
