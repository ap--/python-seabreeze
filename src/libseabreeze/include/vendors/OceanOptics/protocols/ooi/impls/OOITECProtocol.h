/***************************************************//**
 * @file    OOITECProtocol.h
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

#ifndef OOITECPROTOCOL_H
#define OOITECPROTOCOL_H

#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/interfaces/ThermoElectricProtocolInterface.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/TECEnableExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/WriteTECSetPointExchange.h"
#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadTECTemperatureExchange.h"
#include "common/exceptions/ProtocolException.h"

namespace seabreeze {
  namespace ooiProtocol {
    class OOITECProtocol : public ThermoElectricProtocolInterface {
    public:
        OOITECProtocol(TECEnableExchange *enableExchange,
                WriteTECSetPointExchange *writeSetPointExchange,
                ReadTECTemperatureExchange *readTECTempExchange);
        virtual ~OOITECProtocol();

        /* Inherited from ThermoElectricProtocolInterface */
        virtual double readThermoElectricTemperatureCelsius(const Bus &bus)
                throw (ProtocolException);
        virtual void writeThermoElectricEnable(const Bus &bus, bool enable)
                throw (ProtocolException);
        virtual void writeThermoElectricSetPointCelsius(const Bus &bus, double degreesC)
                throw (ProtocolException);

    private:
        TECEnableExchange *tecEnable;
        WriteTECSetPointExchange *tecWriteSetPoint;
        ReadTECTemperatureExchange *tecReadTemperature;
    };
  }
}

#endif /* OOITECPROTOCOL_H */
