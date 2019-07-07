/***************************************************//**
 * @file    OBPLightSourceProtocol.cpp
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPLightSourceProtocol.h"
#include "vendors/OceanOptics/protocols/obp/impls/OceanBinaryProtocol.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPLightSourceEnabledQuery.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPLightSourceEnableCommand.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPLightSourceIntensityQuery.h"
#include "vendors/OceanOptics/protocols/obp/exchanges/OBPLightSourceIntensityCommand.h"
#include "common/exceptions/ProtocolBusMismatchException.h"
#include <string>

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;
using namespace std;

OBPLightSourceProtocol::OBPLightSourceProtocol()
        : LightSourceProtocolInterface(new OceanBinaryProtocol()) {

}

OBPLightSourceProtocol::~OBPLightSourceProtocol() {
    /* Nothing to do here */
}

bool OBPLightSourceProtocol::isLightSourceEnabled(const Bus &bus,
        int moduleIndex, int lightSourceIndex) {

    if(moduleIndex < 0 || moduleIndex >= getModuleCount()) {
        throw ProtocolException("Invalid module index");
    }

    if(lightSourceIndex < 0
            || lightSourceIndex >= getLightSourceCount(moduleIndex)) {
        throw ProtocolException("Invalid light source index");
    }

    OBPLightSourceEnabledQuery query(moduleIndex, lightSourceIndex);
    TransferHelper *helper = NULL;
    helper = bus.getHelper(query.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    bool result = query.queryEnable(helper);
    return result;
}

void OBPLightSourceProtocol::setLightSourceEnable(const Bus &bus,
        int moduleIndex, int lightSourceIndex, bool enable) {

    if(moduleIndex < 0 || moduleIndex >= getModuleCount()) {
        throw ProtocolException("Invalid module index");
    }

    if(lightSourceIndex < 0
            || lightSourceIndex >= getLightSourceCount(moduleIndex)) {
        throw ProtocolException("Invalid light source index");
    }

    OBPLightSourceEnableCommand command;
    command.setLightSourceEnable(moduleIndex, lightSourceIndex, enable);

    TransferHelper *helper = NULL;
    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    command.sendCommandToDevice(helper);
}


OBPLightSourceProtocol_NormalizedIntensity::OBPLightSourceProtocol_NormalizedIntensity() {

}

OBPLightSourceProtocol_NormalizedIntensity::~OBPLightSourceProtocol_NormalizedIntensity() {
    /* Nothing to do here */
}

double OBPLightSourceProtocol_NormalizedIntensity::getIntensity(const Bus &bus,
            int moduleIndex, int lightSourceIndex) {

    if(moduleIndex < 0 || moduleIndex >= getModuleCount()) {
        throw ProtocolException("Invalid module index");
    }

    if(lightSourceIndex < 0
            || lightSourceIndex >= getLightSourceCount(moduleIndex)) {
        throw ProtocolException("Invalid light source index");
    }

    OBPLightSourceIntensityQuery query(moduleIndex, lightSourceIndex);

    TransferHelper *helper = NULL;
    helper = bus.getHelper(query.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    float intensity = query.queryIntensity(helper);
    return intensity;
}

void OBPLightSourceProtocol_NormalizedIntensity::setIntensity(const Bus &bus,
            int moduleIndex, int lightSourceIndex, double intensity) {

    if(moduleIndex < 0 || moduleIndex >= getModuleCount()) {
        throw ProtocolException("Invalid module index");
    }

    if(lightSourceIndex < 0
            || lightSourceIndex >= getLightSourceCount(moduleIndex)) {
        throw ProtocolException("Invalid light source index");
    }

    OBPLightSourceIntensityCommand command;
    command.setLightSourceIntensity(moduleIndex, lightSourceIndex, (float) intensity);

    TransferHelper *helper = NULL;
    helper = bus.getHelper(command.getHints());
    if (NULL == helper) {
        string error("Failed to find a helper to bridge given protocol and bus.");
        throw ProtocolBusMismatchException(error);
    }

    command.sendCommandToDevice(helper);
}
