/***************************************************//**
 * @file    IrradCalFeature.cpp
 * @date    March 2010
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
#include "vendors/OceanOptics/features/irradcal/IrradCalFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/IrradCalProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/FloatVector.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

IrradCalFeature::IrradCalFeature(vector<ProtocolHelper *> helpers, int numPixels) {
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }

    this->numberOfPixels = numPixels;
}

IrradCalFeature::~IrradCalFeature() {

}

int IrradCalFeature::getNumberOfPixels() {
    return this->numberOfPixels;
}

vector<float> *IrradCalFeature::readIrradCalibration(const Protocol &protocol,
        const Bus &bus) throw (FeatureException) {

    vector<float> *data = NULL;
    IrradCalProtocolInterface *cpi = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        cpi = static_cast<IrradCalProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        data = cpi->readIrradCal(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return data;
}

int IrradCalFeature::writeIrradCalibration(const Protocol &protocol,
        const Bus &bus, const vector<float> &calibration) throw (FeatureException) {

    IrradCalProtocolInterface *cpi = NULL;
    ProtocolHelper *proto;
    int written = 0;

    try {
        proto = lookupProtocolImpl(protocol);
        cpi = static_cast<IrradCalProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to get calibration.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        written = cpi->writeIrradCal(bus, calibration);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return written;
}


int IrradCalFeature::hasCollectionArea(const Protocol &protocol,
            const Bus &bus) {
    IrradCalProtocolInterface *cpi = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        cpi = static_cast<IrradCalProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        /* Cannot access any such thing from here, so return accordingly */
        return 0;
    }

    return cpi->hasCollectionArea(bus);
}

double IrradCalFeature::readCollectionArea(const Protocol &protocol,
            const Bus &bus) throw (FeatureException) {
    IrradCalProtocolInterface *cpi = NULL;
    ProtocolHelper *proto;
    float area;

    try {
        proto = lookupProtocolImpl(protocol);
        cpi = static_cast<IrradCalProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to read collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        area = cpi->readCollectionArea(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    return area;
}

void IrradCalFeature::writeCollectionArea(const Protocol &protocol,
            const Bus &bus, double area) throw (FeatureException) {
    IrradCalProtocolInterface *cpi = NULL;
    ProtocolHelper *proto;

    try {
        proto = lookupProtocolImpl(protocol);
        cpi = static_cast<IrradCalProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
        "Could not find matching protocol implementation to write collection area.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        cpi->writeCollectionArea(bus, (float) area);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

}


FeatureFamily IrradCalFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.IRRAD_CAL;
}
