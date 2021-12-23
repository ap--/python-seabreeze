/***************************************************//**
 * @file    NetworkConfigurationFeatureAdapter.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze NetworkConfigurationFeatureInterface instances.
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

#ifndef SEABREEZE_NETWORKONFIGURATIONFEATUREADAPTER_H
#define SEABREEZE_NETWORKONFIGURATIONFEATUREADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/network_configuration/NetworkConfigurationFeatureInterface.h"

namespace seabreeze
{
    namespace api
    {

        class NetworkConfigurationFeatureAdapter
                : public FeatureAdapterTemplate<NetworkConfigurationFeatureInterface>
		{
        	public:
            	NetworkConfigurationFeatureAdapter(
					NetworkConfigurationFeatureInterface *intf, const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex);
            	virtual ~NetworkConfigurationFeatureAdapter();

				unsigned char getNumberOfNetworkInterfaces(int *errorCode);
				unsigned char getNetworkInterfaceConnectionType(int *errorCode, unsigned char interfaceIndex);
				unsigned char getNetworkInterfaceEnableState(int *errorCode, unsigned char interfaceIndex);
				void		  setNetworkInterfaceEnableState(int *errorCode, unsigned char interfaceIndex, unsigned char enableState);
				unsigned char runNetworkInterfaceSelfTest(int *errorCode, unsigned char interfaceIndex);
				void		  saveNetworkInterfaceConnectionSettings(int *errorCode, unsigned char interfaceIndex);
        };

    }
}

#endif //  SEABREEZE_NETWORKONFIGURATIONFEATUREADAPTER_H
