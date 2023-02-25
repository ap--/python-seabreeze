/***************************************************//**
 * @file    WifiFeatureAdapter.h
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze WifiFeatureInterface instances.
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

#ifndef SEABREEZE_WIFICONFIGURATIONFEATUREADAPTER_H
#define SEABREEZE_WIFICONFIGURATIONFEATUREADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "vendors/OceanOptics/features/wifi_configuration/WifiConfigurationFeatureInterface.h"

namespace seabreeze
{
    namespace api
    {

        class WifiConfigurationFeatureAdapter
                : public FeatureAdapterTemplate<WifiConfigurationFeatureInterface>
		{
        	public:
            	WifiConfigurationFeatureAdapter( WifiConfigurationFeatureInterface *intf, const FeatureFamily &f, Protocol *p, Bus *b, unsigned short instanceIndex);
            	virtual ~WifiConfigurationFeatureAdapter();

            	unsigned char getMode(int *errorCode, unsigned char interfaceIndex);
            	void setMode(int *errorCode, unsigned char interfaceIndex, unsigned char enableState);
            	unsigned char getSecurityType(int *errorCode, unsigned char interfaceIndex);
            	void setSecurityType(int *errorCode, unsigned char interfaceIndex, unsigned char securityType);

            	unsigned char getSSID(int *errorCode, unsigned char interfaceIndex, unsigned char (*ssid)[32]);
				void setSSID(int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length);
				void setPassPhrase(int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength);

        };

    }
}

#endif //  SEABREEZE_WIFICONFIGURATIONFEATUREADAPTER_H
