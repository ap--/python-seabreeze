/***************************************************//**
 * @file    OBPSpectrometerProtocol.h
 * @date    February 2011
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

#ifndef OBPSPECTROMETERPROTOCOL_H
#define OBPSPECTROMETERPROTOCOL_H

#include "common/SeaBreeze.h"
#include "common/protocols/Transfer.h"
#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPIntegrationTimeExchange.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPTriggerModeExchange.h"
#include "vendors/OceanOptics/protocols/interfaces/SpectrometerProtocolInterface.h"
#include <vector>

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPSpectrometerProtocol : public SpectrometerProtocolInterface {
    public:
        OBPSpectrometerProtocol(
			OBPIntegrationTimeExchange *integrationTimeExchange,
			Transfer *requestFormattedSpectrumExchange,
			Transfer *readFormattedSpectrumExchange,
			Transfer *requestUnformattedSpectrumExchange,
			Transfer *readUnformattedSpectrumExchange,
			Transfer *requestFastBufferSpectrumExchange,
			Transfer *readFastBufferSpectrumExchange,
            OBPTriggerModeExchange *triggerMode);
        virtual ~OBPSpectrometerProtocol();

		void Initialize(
			OBPIntegrationTimeExchange *integrationTimeExchange,
			Transfer *requestFormattedSpectrumExchange,
			Transfer *readFormattedSpectrumExchange,
			Transfer *requestUnformattedSpectrumExchange,
			Transfer *readUnformattedSpectrumExchange,
			Transfer *requestFastBufferSpectrumExchange,
			Transfer *readFastBufferSpectrumExchange,
			OBPTriggerModeExchange *triggerMode);

        /* FIXME: instead of returning primitive vectors, should this return Data* so that
         * metadata (units, etc.) can also be attached?
         */
		virtual void requestFormattedSpectrum(const Bus &bus);
		virtual std::vector<double> *readFormattedSpectrum(const Bus &bus);
		virtual void requestUnformattedSpectrum(const Bus &bus);
        virtual std::vector<unsigned char> *readUnformattedSpectrum(const Bus &bus);
		virtual void requestFastBufferSpectrum(const Bus &bus, unsigned int numberOfSamplesToRetrieve);
		virtual std::vector<unsigned char> *readFastBufferSpectrum(const Bus &bus, unsigned int numberOfSamplesToRetrieve);
        virtual void setIntegrationTimeMicros(const Bus &bus, unsigned long time_usec);
        virtual void setTriggerMode(const Bus &bus, SpectrometerTriggerMode &mode);

    private:
        OBPIntegrationTimeExchange *integrationTimeExchange;

        /* These are Transfers instead of Exchanges so that we can call getHints() on them.
         * if getHints is promoted up to the level of Exchange, then these can revert back.
         */
        Transfer *requestFormattedSpectrumExchange;
        Transfer *readFormattedSpectrumExchange;
		Transfer *requestUnformattedSpectrumExchange;
		Transfer *readUnformattedSpectrumExchange;
		Transfer *requestFastBufferSpectrumExchange;
		Transfer *readFastBufferSpectrumExchange;
        OBPTriggerModeExchange *triggerModeExchange;
    };
  }
}

#endif /* OBPSPECTROMETERPROTOCOL_H */
