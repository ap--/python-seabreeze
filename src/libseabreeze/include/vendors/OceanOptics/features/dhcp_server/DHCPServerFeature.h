/***************************************************//**
 * @file    DHCPServerFeature.h
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

#ifndef DHCPSERVERFEATURE_H
#define DHCPSERVERFEATURE_H

#include "vendors/OceanOptics/features/dhcp_server/DHCPServerFeatureInterface.h"
#include "common/protocols/Protocol.h"
#include "common/features/FeatureImpl.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze
{

    class DHCPServerFeature : public FeatureImpl, public DHCPServerFeatureInterface
    {
    public:
        DHCPServerFeature(std::vector<ProtocolHelper *> helpers);
        virtual ~DHCPServerFeature();

		virtual unsigned char getServerEnableState(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex);
		virtual void setServerEnableState(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			unsigned char enableState);
		virtual void getServerAddress(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			std::vector<unsigned char> *serverAddress,
			unsigned char *netMask);
		virtual void setServerAddress(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			const std::vector<unsigned char> serverAddress,
			unsigned char netMask);

        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();

    };

}

#endif /* DHCPSERVERFEATURE_H */
