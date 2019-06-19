/***************************************************//**
 * @file    LightSourceFeatureBase.cpp
 * @date    April 2013
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
#include "api/seabreezeapi/FeatureFamilies.h"
#include "common/exceptions/FeatureControlException.h"
#include "vendors/OceanOptics/features/light_source/LightSourceFeatureBase.h"
#include "vendors/OceanOptics/protocols/interfaces/LightSourceProtocolInterface.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

static double __mapIntensityFromProtocol(double intensity,
            double minimum, double maximum) throw (FeatureException);
static double __mapIntensityToProtocol(double intensity, double minimum,
            double maximum) throw (FeatureException);

LightSourceFeatureBase::LightSourceFeatureBase(vector<ProtocolHelper *> helpers,
            int lampModuleIndex) {

    this->moduleIndex = lampModuleIndex;

    vector<ProtocolHelper *>::iterator iter;
    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }
}

LightSourceFeatureBase::~LightSourceFeatureBase() {
    /* Nothing to do here */
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
bool LightSourceFeatureBase::hasLightSourceEnable(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex) throw (FeatureException) {

    if(lightSourceIndex < 0 || lightSourceIndex >= getLightSourceCount(protocol, bus)) {
        throw FeatureException("Invalid light source index");
    }

    LightSourceProtocolInterface *lightSource = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        lightSource = static_cast<LightSourceProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to control light source.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        return lightSource->hasLightSourceEnable(bus, this->moduleIndex, lightSourceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

bool LightSourceFeatureBase::isLightSourceEnabled(const Protocol &protocol,
        const Bus &bus, int lightSourceIndex) throw (FeatureException) {

    if(lightSourceIndex < 0 || lightSourceIndex >= getLightSourceCount(protocol, bus)) {
        throw FeatureException("Invalid light source index");
    }

    LightSourceProtocolInterface *lightSource = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        lightSource = static_cast<LightSourceProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to control light source.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        return lightSource->isLightSourceEnabled(bus, this->moduleIndex, lightSourceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

void LightSourceFeatureBase::setLightSourceEnable(const Protocol &protocol,
        const Bus &bus, int lightSourceIndex, bool enable) throw (FeatureException) {

    if(lightSourceIndex < 0 || lightSourceIndex >= getLightSourceCount(protocol, bus)) {
        throw FeatureException("Invalid light source index");
    }

    LightSourceProtocolInterface *lightSource = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        lightSource = static_cast<LightSourceProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to control light source.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        lightSource->setLightSourceEnable(bus, this->moduleIndex, lightSourceIndex, enable);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

bool LightSourceFeatureBase::hasVariableIntensity(const Protocol &protocol,
            const Bus &bus, int lightSourceIndex) throw (FeatureException) {

    if(lightSourceIndex < 0 || lightSourceIndex >= getLightSourceCount(protocol, bus)) {
        throw FeatureException("Invalid light source index");
    }

    LightSourceProtocolInterface *lightSource = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        lightSource = static_cast<LightSourceProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to control light source.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        return lightSource->hasVariableIntensity(bus, this->moduleIndex, lightSourceIndex);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

double LightSourceFeatureBase::getLightSourceIntensity(const Protocol &protocol,
        const Bus &bus, int lightSourceIndex) throw (FeatureException) {

    if(lightSourceIndex < 0 || lightSourceIndex >= getLightSourceCount(protocol, bus)) {
        throw FeatureException("Invalid light source index");
    }
    /* This expects the intensity to be normalized over the range [0, 1]
     * regardless of the underlying representation in the device.
     */
    LightSourceProtocolInterface *lightSourceProtocolInterface = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        lightSourceProtocolInterface = static_cast<LightSourceProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &ex) {
        throw FeatureProtocolNotFoundException("Could not find "
                "matching protocol implementation to control light source");
    }

    /* The underlying protocol might allow the intensity to be controlled using
     * either counts or a normalized intensity range.  This determines which
     * is true and computes the resulting intensity (over [0, 1]) accordingly.
     *
     * The biggest difference between the interfaces is the type -- the normalized
     * range needs a real-valued variable, and counts are an integer type.
     */
    LightSourceProtocolInterface_NormalizedIntensity *normalized = NULL;
    LightSourceProtocolInterface_Counts *counts = NULL;

    normalized = dynamic_cast<LightSourceProtocolInterface_NormalizedIntensity *>(lightSourceProtocolInterface);
    if(NULL == normalized) {
        /* That failed, so try the Counts approach */
        counts = dynamic_cast<LightSourceProtocolInterface_Counts *>(lightSourceProtocolInterface);
    }

    if(NULL != normalized) {
        try {
            double maximum = normalized->getIntensityMaximum(bus, this->moduleIndex, lightSourceIndex);
            double minimum = normalized->getIntensityMinimum(bus, this->moduleIndex, lightSourceIndex);
            double current = normalized->getIntensity(bus, this->moduleIndex, lightSourceIndex);

            return __mapIntensityFromProtocol(current, minimum, maximum);
        } catch (ProtocolException &ex) {
            string error("Caught protocol exception: ");
            error += ex.what();
            throw FeatureException(error);
        }
    } else if(NULL != counts) {
        try {
            double maximum = counts->getIntensityMaximumCounts(bus, this->moduleIndex, lightSourceIndex);
            double minimum = counts->getIntensityMinimumCounts(bus, this->moduleIndex, lightSourceIndex);
            double current = counts->getIntensityCounts(bus, this->moduleIndex, lightSourceIndex);

            return __mapIntensityFromProtocol(current, minimum, maximum);
        } catch (ProtocolException &ex) {
            string error("Caught protocol exception: ");
            error += ex.what();
            throw FeatureException(error);
        }
    } else {
        throw FeatureException("Cannot find interface allowing intensity to be determined.");
    }
}

void LightSourceFeatureBase::setLightSourceIntensity(const Protocol &protocol,
        const Bus &bus, int lightSourceIndex, double intensity) throw (FeatureException) {

    if(lightSourceIndex < 0 || lightSourceIndex >= getLightSourceCount(protocol, bus)) {
        throw FeatureException("Invalid lamp index");
    }
    /* This expects the intensity to be normalized over the range [0, 1].  This
     * needs to translate that to the value expected by the protocol layer.
     */
    LightSourceProtocolInterface *lightSourceProtocolInterface = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        lightSourceProtocolInterface = static_cast<LightSourceProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException ex) {
        throw FeatureProtocolNotFoundException("Could not find "
                "matching protocol implementation to control lamp");
    }

    /* Determine whether the underlying representation uses a real-valued or
     * integer counts intensity level
     */
    LightSourceProtocolInterface_NormalizedIntensity *normalized = NULL;
    LightSourceProtocolInterface_Counts *counts = NULL;

    normalized = dynamic_cast<LightSourceProtocolInterface_NormalizedIntensity *>(lightSourceProtocolInterface);
    if(NULL == normalized) {
        /* That failed, so try the Counts approach */
        counts = dynamic_cast<LightSourceProtocolInterface_Counts *>(lightSourceProtocolInterface);
    }

    if(NULL != normalized) {
        /* The intensity range coming into this function is [0, 1], but
            * the normalized intensity range of the device may be different.
            * This will remap as needed.
            */
        try {
            double minimum = normalized->getIntensityMinimum(bus, this->moduleIndex, lightSourceIndex);
            double maximum = normalized->getIntensityMaximum(bus, this->moduleIndex, lightSourceIndex);

            double current = __mapIntensityToProtocol(intensity, minimum, maximum);

            normalized->setIntensity(bus, this->moduleIndex, lightSourceIndex, current);
        } catch (ProtocolException &ex) {
            string error("Caught protocol exception: ");
            error += ex.what();
            throw FeatureException(error);
        }
    } else if(NULL != counts) {
        /* If the protocol does not support real-valued intensities,
            * compute the new target point in terms of counts.
            */
        try {
            double maximum = counts->getIntensityMaximumCounts(bus, this->moduleIndex, lightSourceIndex);
            double minimum = counts->getIntensityMinimumCounts(bus, this->moduleIndex, lightSourceIndex);
            double current = __mapIntensityToProtocol(intensity, minimum, maximum);

            counts->setIntensityCounts(bus, this->moduleIndex, lightSourceIndex, (int)current);
        } catch (ProtocolException &ex) {
            string error("Caught protocol exception: ");
            error += ex.what();
            throw FeatureException(error);
        }
    } else {
        throw FeatureException("Cannot find interface allowing intensity to be controlled.");
    }
}

FeatureFamily LightSourceFeatureBase::getFeatureFamily() {
    FeatureFamilies families;

    return families.LIGHT_SOURCE;
}

static double __mapIntensityFromProtocol(double intensity,
            double minimum, double maximum) throw (FeatureException) {

    if (maximum > minimum) {
        /* Maximum intensity corresponds to maximum counts */
        return (intensity - minimum) / (maximum - minimum);
    } else if (maximum < minimum) {
        /* Maximum intensity corresponds to minimum counts */
        return (intensity - maximum) / (minimum - maximum);
    }

    throw FeatureException("Minimum and maximum intensity values are equal");
}

static double __mapIntensityToProtocol(double intensity, double minimum, double maximum)
            throw (FeatureException) {
    double retval = 0.0;

    if (maximum > minimum) {
        /* Maximum intensity corresponds to maximum counts */
        retval = intensity * (maximum - minimum) + minimum;
    } else if (maximum < minimum) {
        /* Maximum intensity corresponds to minimum counts */
        retval = intensity * (minimum - maximum) + maximum;
    } else {
        throw FeatureException("Minimum and maximum intensity values are equal");
    }

    /* Bounds enforcement */
    retval = minimum > retval ? minimum : retval;
    retval = maximum < retval ? maximum : retval;

    return retval;
}
