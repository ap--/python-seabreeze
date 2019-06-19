/***************************************************//**
 * @file    ThermoElectricFeatureBase.h
 * @date    March 2013
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the thermo-
 * electric unit for devices with a clean
 * protocol implementation.  This is an abstract base
 * class but it does much of the work needed for most
 * implementations that can delegate almost everything
 * to the protocol layer.
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

#ifndef THERMOELECTRICFEATUREBASE_H
#define THERMOELECTRICFEATUREBASE_H

#include "common/buses/Bus.h"
#include "common/protocols/Protocol.h"
#include "common/protocols/ProtocolHelper.h"
#include "common/features/FeatureImpl.h"
#include "common/exceptions/FeatureException.h"
#include "common/exceptions/IllegalArgumentException.h"
#include "vendors/OceanOptics/features/thermoelectric/ThermoElectricFeatureInterface.h"

namespace seabreeze {

    class ThermoElectricFeatureBase : public FeatureImpl,
            public ThermoElectricFeatureInterface {
    public:
        ThermoElectricFeatureBase();
        virtual ~ThermoElectricFeatureBase();
        virtual void setThermoElectricEnable(const Protocol &protocol,
                const Bus &bus, bool enable) throw (FeatureException);
        virtual double getTemperatureCelsius(const Protocol &protocol,
                const Bus &bus) throw (FeatureException);
        virtual void setTemperatureSetPointCelsius(const Protocol &protocol,
                const Bus &bus, double degreesC)
                throw (FeatureException, IllegalArgumentException);

        /* These remain abstract because the limits of any given device will
         * depend on its implementation.  They will either need to be hardcoded
         * into the implementation or be queried from the device.
         */
        virtual double getDefaultSetPointCelsius(const Protocol &protocol,
                const Bus &bus) throw (FeatureException) = 0;
        virtual bool getDefaultThermoElectricEnable(const Protocol &protocol,
                const Bus &bus) throw (FeatureException) = 0;

        /* Inherited from Feature */
        /* This is still abstract in case the device needs to do anything
         * special with regards to defaults when it is initialized.
         */
        virtual bool initialize(const Protocol &protocol, const Bus &bus)
            throw (FeatureException) = 0;

        virtual FeatureFamily getFeatureFamily();
    };

}

#endif /* THERMOELECTRICFEATUREBASE_H */
