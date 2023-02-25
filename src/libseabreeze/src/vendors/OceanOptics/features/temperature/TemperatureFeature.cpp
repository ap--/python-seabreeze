/***************************************************//**
 * @file    TemperatureFeature.cpp
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
#include "vendors/OceanOptics/features/temperature/TemperatureFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/TemperatureProtocolInterface.h"
#include "vendors/OceanOptics/protocols/obp/impls/OBPTemperatureProtocol.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace seabreeze::api;
using namespace std;

TemperatureFeature::TemperatureFeature(vector<ProtocolHelper *> helpers)
{
    vector<ProtocolHelper *>::iterator iter;

    for(iter = helpers.begin(); iter != helpers.end(); iter++) {
        this->protocols.push_back(*iter);
    }
}

TemperatureFeature::~TemperatureFeature() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

unsigned char TemperatureFeature::readTemperatureCount(const Protocol &protocol, const Bus &bus)
{

    TemperatureProtocolInterface *temperaturePI = NULL;
	unsigned char temperatureCount;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        temperaturePI = static_cast<TemperatureProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        temperatureCount = temperaturePI->readTemperatureCount(bus);
        return temperatureCount;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

double TemperatureFeature::readTemperature(const Protocol &protocol, const Bus &bus, int index) {

    TemperatureProtocolInterface *temperaturePI = NULL;
	double temperature;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        temperaturePI = static_cast<TemperatureProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        temperature = temperaturePI->readTemperature(bus, index);
        return temperature;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }
}

vector<double> *TemperatureFeature::readAllTemperatures(
        const Protocol &protocol, const Bus &bus) {

    TemperatureProtocolInterface *temperaturePI = NULL;
    vector<double> *temperatures = NULL;
    ProtocolHelper *proto = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        temperaturePI = static_cast<TemperatureProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &e) {
        string error(
                "Could not find matching protocol implementation to get temperature.");
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        temperatures = temperaturePI->readAllTemperatures(bus);
        return temperatures;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        /* FIXME: previous exception should probably be bundled up into the new exception */
        throw FeatureControlException(error);
    }

    /* Unreachable */
    return NULL;
}

FeatureFamily TemperatureFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.TEMPERATURE;
}
