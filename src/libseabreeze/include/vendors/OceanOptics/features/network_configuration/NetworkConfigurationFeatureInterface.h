/***************************************************//**
 * @file    NetworkConfigurationFeatureInterface.h
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

#ifndef NETWORKCONFIGURATIONFEATUREINTERFACE_H
#define NETWORKCONFIGURATIONFEATUREINTERFACE_H

#include "common/protocols/Protocol.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    class NetworkConfigurationFeatureInterface {
    public:
        virtual ~NetworkConfigurationFeatureInterface() = 0;

virtual unsigned char getNumberOfNetworkInterfaces(
			const Protocol &protocol,
			const Bus &bus) = 0;
		virtual unsigned char getNetworkInterfaceConnectionType(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex) = 0;
		virtual unsigned char getNetworkInterfaceEnableState(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex) = 0;
		virtual void setNetworkInterfaceEnableState(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex,
			unsigned char enableState) = 0;
		virtual unsigned char runNetworkInterfaceSelfTest(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex) = 0;
		virtual void saveNetworkInterfaceConnectionSettings(
			const Protocol &protocol,
			const Bus &bus,
			unsigned char interfaceIndex) = 0;

    };

    /* Default implementation for (otherwise) pure virtual destructor */
    inline NetworkConfigurationFeatureInterface::~NetworkConfigurationFeatureInterface() {}
}

#endif /* NETWORKCONFIGURATIONFEATUREINTERFACE_H */
