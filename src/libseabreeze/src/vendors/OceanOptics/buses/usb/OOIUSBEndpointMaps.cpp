/***************************************************//**
 * @file    OOIUSBSpectrumTransferHelper.h
 * @date    February 2009
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
#include "vendors/OceanOptics/buses/usb/OOIUSBEndpointMaps.h"

using namespace seabreeze;

int OOIUSBCypressEndpointMap::getLowSpeedInEP() const {
    return this->lowSpeedIn;
}

int OOIUSBCypressEndpointMap::getLowSpeedOutEP() const {
    return this->lowSpeedOut;
}

int OOIUSBCypressEndpointMap::getHighSpeedInEP() const {
    return highSpeedIn;
}

int OOIUSBCypressEndpointMap::getHighSpeedIn2EP() const {
    return highSpeedIn2;
}

OOIUSBLegacy2KEndpointMap::OOIUSBLegacy2KEndpointMap() {
    this->lowSpeedOut = 0x02;
    this->highSpeedIn = 0x82;
    /* Endpoint 0x07 is unused */
    this->lowSpeedIn = 0x87;
}

OOIUSBLegacy2KEndpointMap::~OOIUSBLegacy2KEndpointMap() {

}

OOIUSBFPGAEndpointMap::OOIUSBFPGAEndpointMap() {
    this->lowSpeedOut = 0x01;
    this->highSpeedIn = 0x82;
    this->highSpeedIn2 = 0x86;
    this->lowSpeedIn = 0x81;
}

OOIUSBFPGAEndpointMap::~OOIUSBFPGAEndpointMap() {

}

OOIUSBJazEndpointMap::OOIUSBJazEndpointMap() {
    this->lowSpeedOut = 0x01;
    this->highSpeedIn = 0x82;
    this->lowSpeedIn = 0x81;
    /* The Jaz does not have a second "high speed in" endpoint. */
}

OOIUSBJazEndpointMap::~OOIUSBJazEndpointMap() {

}

OOIUSBBidrectionalEndpointMap::OOIUSBBidrectionalEndpointMap() {

}

OOIUSBBidrectionalEndpointMap::~OOIUSBBidrectionalEndpointMap() {

}

int OOIUSBBidrectionalEndpointMap::getPrimaryInEndpoint() const {
    return this->primaryInEndpoint;
}

int OOIUSBBidrectionalEndpointMap::getPrimaryOutEndpoint() const {
    return this->primaryOutEndpoint;
}

OOIUSBVentanaEndpointMap::OOIUSBVentanaEndpointMap() {
    this->primaryInEndpoint = 0x82;
    this->primaryOutEndpoint = 0x01;
}

OOIUSBVentanaEndpointMap::~OOIUSBVentanaEndpointMap() {

}

OOIUSBDualBidirectionalEndpointMap::OOIUSBDualBidirectionalEndpointMap() {

}

OOIUSBDualBidirectionalEndpointMap::~OOIUSBDualBidirectionalEndpointMap() {

}

int OOIUSBDualBidirectionalEndpointMap::getSecondaryInEndpoint() const {
    return this->secondaryInEndpoint;
}

int OOIUSBDualBidirectionalEndpointMap::getSecondaryOutEndpoint() const {
    return this->secondaryOutEndpoint;
}

OOIUSBSimpleDualEndpointMap::OOIUSBSimpleDualEndpointMap() {
    /* This endpoint map represents a pair of bidirectional
     * USB endpoints: 1/81 and 2/82.
     */
    this->primaryInEndpoint = 0x81;
    this->primaryOutEndpoint = 0x01;
    this->secondaryInEndpoint = 0x82;
    this->secondaryOutEndpoint = 0x02;
}

OOIUSBSimpleDualEndpointMap::~OOIUSBSimpleDualEndpointMap() {

}
