/***************************************************//**
 * @file    OpticalBenchFeature.cpp
 * @date    January 2015
 * @author  Kirk Clendinning, Heliospectra
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Heliospetra AB
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
#include "vendors/OceanOptics/features/optical_bench/OpticalBenchFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/OpticalBenchProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPOpticalBenchProtocol.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace seabreeze::api;
using namespace std;

OpticalBenchFeature::OpticalBenchFeature(vector<ProtocolHelper *> helpers) {
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }
}

OpticalBenchFeature::~OpticalBenchFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif


unsigned short int OpticalBenchFeature::readOpticalBenchFiberDiameterMicrons(const Protocol &protocol, const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI = NULL;
	unsigned short int fiberDiameter;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        fiberDiameter = opticalBenchPI->readOpticalBenchFiberDiameterMicrons(bus);
        return fiberDiameter;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

unsigned short int OpticalBenchFeature::readOpticalBenchSlitWidthMicrons(const Protocol &protocol, const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI = NULL;
	unsigned short int slitWidth;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        slitWidth = opticalBenchPI->readOpticalBenchSlitWidthMicrons(bus);
        return slitWidth;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

string *OpticalBenchFeature::readOpticalBenchID(const Protocol &protocol,
                const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI= NULL;
    ProtocolHelper *proto = NULL;
    string *retval = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get serial number.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        retval = opticalBenchPI->readOpticalBenchID(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return retval;
}

string *OpticalBenchFeature::readOpticalBenchSerialNumber(const Protocol &protocol,
                const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI= NULL;
    ProtocolHelper *proto = NULL;
    string *retval = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get serial number.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        retval = opticalBenchPI->readOpticalBenchSerialNumber(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return retval;
}

string *OpticalBenchFeature::readOpticalBenchCoating(const Protocol &protocol,
                const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI= NULL;
    ProtocolHelper *proto = NULL;
    string *retval = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get serial number.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        retval = opticalBenchPI->readOpticalBenchCoating(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return retval;
}

string *OpticalBenchFeature::readOpticalBenchFilter(const Protocol &protocol,
                const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI= NULL;
    ProtocolHelper *proto = NULL;
    string *retval = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get serial number.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        retval = opticalBenchPI->readOpticalBenchFilter(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return retval;
}


string *OpticalBenchFeature::readOpticalBenchGrating(const Protocol &protocol,
                const Bus &bus) {

    OpticalBenchProtocolInterface *opticalBenchPI= NULL;
    ProtocolHelper *proto = NULL;
    string *retval = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        opticalBenchPI = static_cast<OpticalBenchProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get serial number.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        retval = opticalBenchPI->readOpticalBenchGrating(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return retval;
}


FeatureFamily OpticalBenchFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.OPTICAL_BENCH;
}
