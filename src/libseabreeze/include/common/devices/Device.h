/***************************************************//**
 * @file    Device.h
 * @date    February 2009
 * @author  Ocean Optics, Inc.
 *
 * This is a base class for all sorts of devices.  A
 * device is really just an aggregation of features
 * with the protocols and buses required to access them.
 * A Device is intended to represent a single discrete
 * piece of equipment that may have several capabilities
 * (features) inside.  The device may communicate to the
 * outside world via seqences of bytes (a protocol) that
 * are transferred across a physical medium (the bus).
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

#ifndef DEVICE_H
#define DEVICE_H

#include <vector>
#include <string>
#include "common/buses/Bus.h"
#include "common/buses/BusFamily.h"
#include "common/buses/DeviceLocatorInterface.h"
#include "common/features/Feature.h"
#include "common/features/FeatureFamily.h"
#include "common/protocols/Protocol.h"
#include "api/USBEndpointTypes.h"

namespace seabreeze {

    class Device {
    public:
        Device();
        virtual ~Device();
        std::vector<Bus *> &getBuses();
        std::vector<Feature *> &getFeatures();
        std::vector<Protocol *> &getProtocols();
        std::string &getName();

        // get the usb endpoints, according to the endpointType enumerator,
        //  if the endpoint type is not used, a 0 is returned
        // TODO: this should be delegated down into a Bus instance
        // (e.g. found by getBusesByFamily()) for USB.  It is inappropriate here.
        unsigned char getEndpoint(int *errorCode, usbEndpointType endpointType);

        /* This will allow the driver to probe the device and initialize itself
         * based on what it finds there.  This should be called shortly after
         * open().  The Device instance should use the indicated Bus instance to
         * communicate with the hardware and get everything set up.  It can
         * use any appropriate protocol or protocols that are valid for the Bus.
         */
        virtual bool initialize(const Bus &bus);

        /* Each instance of a device is assumed to be associated with a unique
         * location on a bus.  If the device is connected via multiple buses, then
         * a special DeviceLocator and TransferHelper will have to hide those
         * details.  Otherwise, each connection to the device will be considered
         * independent of all others.
         */
        virtual DeviceLocatorInterface *getLocation();
        virtual void setLocation(const DeviceLocatorInterface &loc);

        virtual int open();

        virtual void close();

        virtual std::vector<Bus *> getBusesByFamily(BusFamily &family);

        virtual seabreeze::ProtocolFamily getSupportedProtocol(
                seabreeze::FeatureFamily family, BusFamily bus) = 0;

        virtual std::vector<Protocol *> getProtocolsByFamily(
                seabreeze::ProtocolFamily &family);

        virtual Bus *getOpenedBus();

    protected:
        std::vector<Bus *> buses;
        std::vector<Feature *> features;
        std::vector<Protocol *> protocols;

        std::string name;
        unsigned char usbEndpoint_primary_out;
    	unsigned char usbEndpoint_primary_in;
    	unsigned char usbEndpoint_secondary_out;
    	unsigned char usbEndpoint_secondary_in;
    	unsigned char usbEndpoint_secondary_in2;


        DeviceLocatorInterface *location;
        Bus *openedBus;
    };

}

#endif /* DEVICE_H */
