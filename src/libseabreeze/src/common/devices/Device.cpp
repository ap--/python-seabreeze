/***************************************************//**
 * @file    Device.cpp
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

#include "common/globals.h"
#include "common/devices/Device.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

using namespace std;
using namespace seabreeze;

Device::Device() {
    this->openedBus = NULL;
    this->location = NULL;
}

Device::~Device() {
    vector<Bus *>::iterator bIter;
    vector<Feature *>::iterator fIter;
    vector<Protocol *>::iterator pIter;

    for(fIter = this->features.begin(); fIter != this->features.end(); fIter++) {
        delete *fIter;
    }

    for(pIter = this->protocols.begin(); pIter != this->protocols.end(); pIter++) {
        delete *pIter;
    }

    for(bIter = this->buses.begin(); bIter != this->buses.end(); bIter++) {
        delete *bIter;
    }

    if(NULL != this->location) {
        delete this->location;
    }
}

vector<Bus *> &Device::getBuses() {
    return (this->buses);
}

vector<Feature *> &Device::getFeatures() {
    return (this->features);
}

vector<Protocol *> &Device::getProtocols() {
    return (this->protocols);
}

string &Device::getName() {
    return (this->name);
}

unsigned char Device::getEndpoint(int *errorCode, usbEndpointType endpointType)
{
    unsigned char endpoint;
    
	SET_ERROR_CODE(ERROR_SUCCESS);
	
	switch (endpointType)
	{
		case kEndpointTypePrimaryOut:
			endpoint = this->usbEndpoint_primary_out;
			break;
		case kEndpointTypePrimaryIn:
			endpoint = this->usbEndpoint_primary_in;
			break;
		case kEndpointTypeSecondaryOut:
			endpoint = this->usbEndpoint_secondary_out;
			break;
		case kEndpointTypeSecondaryIn:
			endpoint = this->usbEndpoint_secondary_in;
			break;
		case kEndpointTypeSecondaryIn2:
			endpoint = this->usbEndpoint_secondary_in2;
			break;
		default:
			SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			break;
	}
	return endpoint;
}


DeviceLocatorInterface *Device::getLocation() {
    return this->location;
}

void Device::setLocation(const DeviceLocatorInterface &loc) {
    if(NULL != this->location) {
        delete this->location;
    }

    this->location = loc.clone();
}

bool Device::initialize(const Bus &bus) {

    /* This function can be overridden to probe the hardware and determine
     * what features are available before initializing them.
     */
    vector<Feature *>::iterator fIter;

    for(fIter = this->features.begin(); fIter != this->features.end(); fIter++) {
        try {
            ProtocolFamily protocolFamily = getSupportedProtocol(
                (*fIter)->getFeatureFamily(), bus.getBusFamily());
            vector<Protocol *> protocols = getProtocolsByFamily(protocolFamily);
            if(protocols.size() < 1) {
                /* No supported protocol for this feature on the given bus. */
                continue;
            }
            (*fIter)->initialize(*(protocols[0]), bus);
        } catch (FeatureException &fe) {
            /* This ought to remove the feature if it cannot be accessed */
        }
    }

    return true;
}

int Device::open() {
    if(NULL == this->location) {
        /* Cannot open without a valid location specified */
        return -1;
    }

    /* Look up the bus based on the location type, then push the locator
     * to the bus, open the bus, and store it in the openedBus pointer
     */
    BusFamily targetFamily = location->getBusFamily();
    vector<Bus *> buses = getBusesByFamily(targetFamily);
    if(buses.size() > 0) {
        Bus *bus = buses[0];
        try {
            bool success = false;
            bus->setLocation(*(this->location));
            success = bus->open();
            if(true == success) {
                this->openedBus = bus;
                return 0;
            }
            return -4;
        } catch (runtime_error &re) {
            /* Probably a bad location for this bus */
            return -3;
        }
    } else {
        /* Bus not found. */
        return -2;
    }
}

void Device::close() {
    if(NULL == this->openedBus) {
        return;
    }

    this->openedBus->close();

    this->openedBus = NULL;
}

Bus *Device::getOpenedBus() {
    return this->openedBus;
}

vector<Bus *> Device::getBusesByFamily(BusFamily &family) {
    vector<Bus *>::iterator busIter;
    vector<Bus *> retval;
    for(busIter = this->buses.begin(); busIter != this->buses.end(); busIter++) {
        BusFamily thatFamily = (*busIter)->getBusFamily();
        if(thatFamily.equals(family)) {
            retval.push_back(*busIter);
        }
    }
    return retval;
}

vector<Protocol *> Device::getProtocolsByFamily(ProtocolFamily &family) {
    vector<Protocol *>::iterator protoIter;
    vector<Protocol *> retval;
    for(protoIter = this->protocols.begin(); protoIter != this->protocols.end(); protoIter++) {
        ProtocolFamily thatFamily = (*protoIter)->getProtocolFamily();
        if(thatFamily.equals(family)) {
            retval.push_back(*protoIter);
        }
    }
    return retval;
}
