/***************************************************//**
 * @file    ProtocolFamilies.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This provides a way to get references to different kinds
 * of features (e.g. spectrometer, TEC) generically.
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
#include "api/seabreezeapi/ProtocolFamilies.h"

/* Constants */
#define PROTOCOL_FAMILY_ID_UNDEFINED         0
#define PROTOCOL_FAMILY_ID_VIRTUAL           1
#define PROTOCOL_FAMILY_ID_OOI               2
#define PROTOCOL_FAMILY_ID_OCEAN_BINARY      3
#define PROTOCOL_FAMILY_ID_JAZ_MESSAGING     4

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

seabreeze::api::ProtocolFamilies::ProtocolFamilies() {

}

seabreeze::api::ProtocolFamilies::~ProtocolFamilies() {

}

seabreeze::api::UndefinedProtocolFamily::UndefinedProtocolFamily()
        : ProtocolFamily("Undefined", PROTOCOL_FAMILY_ID_UNDEFINED) {

}

seabreeze::api::UndefinedProtocolFamily::~UndefinedProtocolFamily() {

}

seabreeze::api::VirtualProtocolFamily::VirtualProtocolFamily()
        : ProtocolFamily("Virtual", PROTOCOL_FAMILY_ID_VIRTUAL) {

}

seabreeze::api::VirtualProtocolFamily::~VirtualProtocolFamily() {

}

seabreeze::api::OOIProtocolFamily::OOIProtocolFamily()
        : ProtocolFamily("OceanLegacyUSB", PROTOCOL_FAMILY_ID_OOI) {

}

seabreeze::api::OOIProtocolFamily::~OOIProtocolFamily() {

}

seabreeze::api::OceanBinaryProtocolFamily::OceanBinaryProtocolFamily()
        : ProtocolFamily("OceanBinary", PROTOCOL_FAMILY_ID_OCEAN_BINARY) {

}

seabreeze::api::OceanBinaryProtocolFamily::~OceanBinaryProtocolFamily() {

}

seabreeze::api::JazMessagingProtocolFamily::JazMessagingProtocolFamily()
        : ProtocolFamily("JazMessaging", PROTOCOL_FAMILY_ID_JAZ_MESSAGING) {

}

seabreeze::api::JazMessagingProtocolFamily::~JazMessagingProtocolFamily() {

}

vector<ProtocolFamily *> seabreeze::api::ProtocolFamilies::getAllProtocolFamilies() {
    vector<ProtocolFamily *> retval;
    /* This creates new instances of these so the class-wide fields do not risk
     * having their const flags ignored.
     */
    retval.push_back(new OOIProtocolFamily());
    retval.push_back(new OceanBinaryProtocolFamily());
    retval.push_back(new JazMessagingProtocolFamily());
    retval.push_back(new VirtualProtocolFamily());

    return retval;
}
