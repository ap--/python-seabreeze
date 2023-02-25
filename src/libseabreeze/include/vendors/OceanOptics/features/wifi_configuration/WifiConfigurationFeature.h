/***************************************************//**
 * @file    WifiConfigurationFeature.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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

#ifndef WIFICONFIGURATIONFEATURE_H
#define WIFICONFIGURATIONFEATURE_H

#include "vendors/OceanOptics/features/wifi_configuration/WifiConfigurationFeatureInterface.h"
#include "common/protocols/Protocol.h"
#include "common/features/FeatureImpl.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze
{

    class WifiConfigurationFeature : public FeatureImpl, public WifiConfigurationFeatureInterface
    {
    public:
        WifiConfigurationFeature(std::vector<ProtocolHelper *> helpers);
        virtual ~WifiConfigurationFeature();

		virtual unsigned char getMode(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex);
		virtual void setMode(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			unsigned char mode);
		virtual unsigned char getSecurityType(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex);
		virtual void setSecurityType(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			unsigned char securityType);
		virtual std::vector<unsigned char> getSSID(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex);
		virtual void setSSID(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			const std::vector<unsigned char> ssid);
		virtual void setPassPhrase(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			const std::vector<unsigned char> passPhrase);


        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();

    };

}

#endif /* WIFICONFIGURATIONFEATURE_H */
