/***************************************************//**
 * @file    STSPixelBinningFeatureBase.cpp
 * @date    October 2015
 * @author  Ocean Optics, Inc.
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
#include "vendors/OceanOptics/features/pixel_binning/STSPixelBinningFeature.h"
#include "vendors/OceanOptics/features/spectrometer/STSSpectrometerFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/PixelBinningProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

STSPixelBinningFeature::STSPixelBinningFeature(std::vector<ProtocolHelper *> helpers, STSSpectrometerFeature *spectroFeature)
    : spectrometerFeature(spectroFeature) {

    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }
}

STSPixelBinningFeature::~STSPixelBinningFeature() {
}

void STSPixelBinningFeature::setPixelBinningFactor(const Protocol &protocol,
        const Bus &bus, const unsigned char binningFactor) {

    PixelBinningProtocolInterface *pb = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        pb = static_cast<PixelBinningProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to set pixel binning factor.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        const unsigned char maxBinning = pb->readMaxPixelBinningFactor(bus);

        // The binning factor must be in the range 0-maxBinning
        if (binningFactor > maxBinning) {
            string error("Specified binning factor is not supported.");
            throw FeatureException(error);
        }

        pb->writePixelBinningFactor(bus, binningFactor);

        if (spectrometerFeature != NULL) {
            spectrometerFeature->setPixelBinningFactor(binningFactor);
        }
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

unsigned char STSPixelBinningFeature::getPixelBinningFactor(const Protocol &protocol,
        const Bus &bus) {

    PixelBinningProtocolInterface *pb = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        pb = static_cast<PixelBinningProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to read pixel binning factor.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    unsigned char retval = 0;

    try {
        retval = pb->readPixelBinningFactor(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

void STSPixelBinningFeature::setDefaultPixelBinningFactor(const Protocol &protocol,
        const Bus &bus, const unsigned char binningFactor) {

    PixelBinningProtocolInterface *pb = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        pb = static_cast<PixelBinningProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to set pixel binning factor.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        const unsigned char maxBinning = pb->readMaxPixelBinningFactor(bus);

        // The binning factor must be in the range 0-maxBinning
        if (binningFactor > maxBinning) {
            string error("Specified binning factor is not supported.");
            throw FeatureException(error);
        }

        pb->writeDefaultPixelBinningFactor(bus, binningFactor);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

void STSPixelBinningFeature::setDefaultPixelBinningFactor(const Protocol &protocol,
        const Bus &bus) {

    PixelBinningProtocolInterface *pb = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        pb = static_cast<PixelBinningProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to set pixel binning factor.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        pb->writeDefaultPixelBinningFactor(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

unsigned char STSPixelBinningFeature::getDefaultPixelBinningFactor(const Protocol &protocol,
        const Bus &bus) {

    PixelBinningProtocolInterface *pb = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        pb = static_cast<PixelBinningProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to read pixel binning factor.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    unsigned char retval = 0;

    try {
        retval = pb->readDefaultPixelBinningFactor(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

unsigned char STSPixelBinningFeature::getMaxPixelBinningFactor(const Protocol &protocol,
        const Bus &bus) {

    PixelBinningProtocolInterface *pb = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        pb = static_cast<PixelBinningProtocolInterface *>(proto);
    } catch (const FeatureProtocolNotFoundException &fpnfe) {
        string error(
                "Could not find matching protocol implementation to read pixel binning factor.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    unsigned char retval = 0;

    try {
        retval = pb->readMaxPixelBinningFactor(bus);
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
    return retval;
}

//void STSPixelBinningFeature::setSpectrometerFeature(STSSpectrometerFeature *s) {
//    spectrometerFeature = s;
//}
//
FeatureFamily STSPixelBinningFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.PIXEL_BINNING;
}

bool STSPixelBinningFeature::initialize(const Protocol &protocol, const Bus &bus) {
    /* This doesn't need to do anything special at this point.  If desired, this
     * could be used to read out the current setting and cache it, but that is
     * not strictly required.
     */
    return true;
}
