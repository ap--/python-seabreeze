/***************************************************//**
 * @file    OpticalBenchProtocolInterface.h
 * @date    Janaure 2015
 * @author  Ocean Optics, Inc., Kirk Clendinning, Heliospectra
 *
 * This is a simple interface for any protocol to implement
 * that provides a protocol-agnostic mechanism for accessing
 * Optical Bench Parameters on an Ocean Optics device.
 *
 * This does not extend Protocol or otherwise read involved
 * in that hierarchy because it might interfere with the
 * lookup process for readting a Protocol object to delegate
 * these methods to.  Worse, it could end up inheriting
 * twice from the same base class, which is just messy.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc
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

#ifndef OPTICALBENCHPROTOCOLINTERFACE_H
#define OPTICALBENCHPROTOCOLINTERFACE_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include <vector>
#include "common/exceptions/ProtocolException.h"
#include "common/protocols/ProtocolHelper.h"

namespace seabreeze {

    class OpticalBenchProtocolInterface : public ProtocolHelper {
    public:
        OpticalBenchProtocolInterface(Protocol *protocol);
        virtual ~OpticalBenchProtocolInterface();
        virtual std::string *readOpticalBenchID(const Bus &bus)
                throw (ProtocolException) = 0;
        virtual std::string *readOpticalBenchSerialNumber(const Bus &bus)
                throw (ProtocolException) = 0;
        virtual unsigned short int readOpticalBenchSlitWidthMicrons(const Bus &bus)
                throw (ProtocolException) = 0;
        virtual unsigned short int readOpticalBenchFiberDiameterMicrons(const Bus &bus)
                throw (ProtocolException) = 0;
    	virtual std::string *readOpticalBenchGrating(const Bus &bus)
                throw (ProtocolException) = 0;
        virtual std::string *readOpticalBenchFilter(const Bus &bus)
                throw (ProtocolException) = 0;
        virtual std::string *readOpticalBenchCoating(const Bus &bus)
                throw (ProtocolException) = 0;
    };

}

#endif /* OPTICALBENCHPROTOCOLINTERFACE_H */