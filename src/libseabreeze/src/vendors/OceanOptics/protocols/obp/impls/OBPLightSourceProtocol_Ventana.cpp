/***************************************************//**
 * @file    OBPLightSourceProtocol_Ventana.h
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
#include "vendors/OceanOptics/protocols/obp/impls/OBPLightSourceProtocol_Ventana.h"

#define LASER_INTENSITY_MINIMUM 0.0
#define LASER_INTENSITY_MAXIMUM 1.0

using namespace seabreeze;
using namespace seabreeze::oceanBinaryProtocol;

OBPLightSourceProtocol_Ventana::OBPLightSourceProtocol_Ventana() {

}

OBPLightSourceProtocol_Ventana::~OBPLightSourceProtocol_Ventana() {

}

int OBPLightSourceProtocol_Ventana::getModuleCount() {
    /* The Ventana can have at most one laser module */
    return 1;
}

int OBPLightSourceProtocol_Ventana::getLightSourceCount(int moduleIndex) {
    if(0 == moduleIndex) {
        /* The single laser module has only a single light source */
        return 1;
    }
    /* If any other module ID is specified, there are no controllable sources */
    return 0;
}


bool OBPLightSourceProtocol_Ventana::hasLightSourceEnable(const Bus &bus, int moduleIndex,
                int lightSourceIndex) throw (ProtocolException) {
    if(0 != moduleIndex) {
        /* Ventana has at most one laser module */
        throw ProtocolException("Invalid module index");
    }

    if(0 != lightSourceIndex) {
        /* Laser module has at most one laser */
        throw ProtocolException("Invalid light source index");
    }

    /* The Ventana allows the laser to be enabled and disabled. */
    return true;
}


bool OBPLightSourceProtocol_Ventana::hasVariableIntensity(const Bus &bus, int moduleIndex,
                int lightSourceIndex) throw (ProtocolException) {

    if(0 != moduleIndex) {
        /* Ventana has at most one laser module */
        throw ProtocolException("Invalid module index");
    }

    if(0 != lightSourceIndex) {
        /* Laser module has at most one laser */
        throw ProtocolException("Invalid light source index");
    }

    /* The Ventana allows the laser intensity to be controlled through pulsing. */
    return true;
}


double OBPLightSourceProtocol_Ventana::getIntensityMinimum(const Bus &bus, int moduleIndex,
                int lightSourceIndex) {

    if(0 != moduleIndex) {
        /* Ventana has at most one laser module */
        throw ProtocolException("Invalid module index");
    }

    if(0 != lightSourceIndex) {
        /* Laser module has at most one laser */
        throw ProtocolException("Invalid light source index");
    }

    return LASER_INTENSITY_MINIMUM;
}

double OBPLightSourceProtocol_Ventana::getIntensityMaximum(const Bus &bus, int moduleIndex,
                int lightSourceIndex) {

    if(0 != moduleIndex) {
        /* Ventana has at most one laser module */
        throw ProtocolException("Invalid module index");
    }

    if(0 != lightSourceIndex) {
        /* Laser module has at most one laser */
        throw ProtocolException("Invalid light source index");
    }

    return LASER_INTENSITY_MAXIMUM;
}
