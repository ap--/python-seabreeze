/***************************************************//**
 * @file    ThermoElectricCoolerFeatureAdapter.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows access to SeaBreeze
 * TECFeatureInterface instances.
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

#ifndef SEABREEZE_THERMO_ELECTRIC_COOLER_FEATURE_ADAPTER_H
#define SEABREEZE_THERMO_ELECTRIC_COOLER_FEATURE_ADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/thermoelectric/ThermoElectricFeatureInterface.h"

namespace seabreeze {
    namespace api {

        class ThermoElectricCoolerFeatureAdapter
                : public FeatureAdapterTemplate<ThermoElectricFeatureInterface> {
        public:
            ThermoElectricCoolerFeatureAdapter(ThermoElectricFeatureInterface *intf,
                    const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex);
            virtual ~ThermoElectricCoolerFeatureAdapter();

            /* Thermoelectric cooler functions */
            double readTECTemperature(int *errorCode);
            void setTECTemperature(int *errorCode,
                    double temperature_degrees_celsius);
            void setTECEnable(int *errorCode, bool tecEnable);
            void setTECFanEnable(int *errorCode, bool tecFanEnable);
        };

    }
}

#endif
