/***************************************************//**
 * @file    AcquisitionDelayFeature.cpp
 * @date    November 2015
 * @author  Ocean Optics, Inc.
 *
 * This feature provides an interface to the acquisition
 * delay feature.  This is also referred to as a
 * trigger delay.
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

#include "common/globals.h"
#include "vendors/OceanOptics/features/acquisition_delay/AcquisitionDelayFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/AcquisitionDelayProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPAcquisitionDelayProtocol.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/exceptions/FeatureControlException.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

AcquisitionDelayFeature::AcquisitionDelayFeature(vector<ProtocolHelper *> helpers) {
    /* Set to a safe default */
    this->lastAcquisitionDelayMicroseconds = 0;
    this->lastAcquisitionDelayValid = false;

    vector<ProtocolHelper *>::iterator iter;
    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }
}

AcquisitionDelayFeature::~AcquisitionDelayFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
void AcquisitionDelayFeature::setAcquisitionDelayMicroseconds(
            const Protocol &protocol, const Bus &bus,
            const unsigned long delayMicros)
            throw (FeatureException) {

    AcquisitionDelayProtocolInterface *delay = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        delay = static_cast<AcquisitionDelayProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to set acquisition delay.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        delay->setAcquisitionDelayMicroseconds(bus, delayMicros);
        this->lastAcquisitionDelayMicroseconds = delayMicros;
        this->lastAcquisitionDelayValid = true;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

unsigned long AcquisitionDelayFeature::getAcquisitionDelayMicroseconds(
                    const Protocol &protocol, const Bus &bus)
                    throw (FeatureException) {
    /* Many devices do not have a way to query this value.  The default
     * implementation of the method here will use a cached value for
     * the last delay that was configured.  If no value has been set
     * yet, this will throw a FeatureException so that the invalid value
     * is not actually used.
     *
     * If a device is added that can query this directly, then this function
     * should be overridden.
     */

    if(true == this->lastAcquisitionDelayValid) {
        return this->lastAcquisitionDelayMicroseconds;
    }

    throw FeatureException("Cannot read acquisition delay from device, and no previously set value is cached.");
}

FeatureFamily AcquisitionDelayFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.ACQUISITION_DELAY;
}

