/***************************************************//**
 * @file    IPv4FeatureAdapter.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze IPv4FeatureInterface instances.
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

#ifndef SEABREEZE_IPV4FEATUREADAPTER_H
#define SEABREEZE_IPV4FEATUREADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/ipv4/IPv4FeatureInterface.h"

namespace seabreeze
{
    namespace api
    {

        class IPv4FeatureAdapter
                : public FeatureAdapterTemplate<IPv4FeatureInterface>
		{
        	public:
            	IPv4FeatureAdapter(IPv4FeatureInterface *intf, const FeatureFamily &f, Protocol *p, Bus *b, unsigned short instanceIndex);
            	virtual ~IPv4FeatureAdapter();

            	unsigned char get_IPv4_DHCP_Enable_State(int *errorCode, unsigned char interfaceIndex);
				void   set_IPv4_DHCP_Enable_State(int *errorCode, unsigned char interfaceIndex, unsigned char isEnabled);
				unsigned char get_Number_Of_IPv4_Addresses(int *errorCode, unsigned char interfaceIndex);
				void   get_IPv4_Address(int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask);
				void   get_IPv4_Default_Gateway(int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4]);
				void   set_IPv4_Default_Gateway(int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4]);
				void   add_IPv4_Address(int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask);
				void   delete_IPv4_Address(int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex);
        };
    }
}

#endif //  SEABREEZE_IPV4FEATUREADAPTER_H
