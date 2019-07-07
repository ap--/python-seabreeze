/***************************************************//**
 * @file    ContinuousStrobeFeature_FPGA.cpp
 * @date    February 2013
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
#include "vendors/OceanOptics/features/continuous_strobe/ContinuousStrobeFeature_FPGA.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOIProtocol.h"
#include "vendors/OceanOptics/features/fpga_register/FPGARegisterFeature.h"
#include "vendors/OceanOptics/features/light_source/StrobeLampFeature.h"
#include "vendors/OceanOptics/protocols/ooi/constants/FPGARegisterCodes.h"
#include "vendors/OceanOptics/protocols/ooi/impls/OOIStrobeLampProtocol.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"
#include <math.h>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace seabreeze::ooiProtocol;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

ContinuousStrobeFeature_FPGA::ContinuousStrobeFeature_FPGA() {

}

ContinuousStrobeFeature_FPGA::~ContinuousStrobeFeature_FPGA() {

}

void ContinuousStrobeFeature_FPGA::setContinuousStrobePeriodMicroseconds(const Protocol &protocol,
        const Bus &bus, unsigned short strobe_id, unsigned long period_usec) {

    if (0 != strobe_id) {
        throw FeatureException("feature only supports one continuous strobe generator");
    }

    // delegate I/O to internal FPGA Register Feature
    FPGARegisterFeature fpga;
    unsigned char fpgaMajorVersion = 0;

    fpgaMajorVersion = fpga.getMajorVersion(bus);

    // we could subclass this, but I'm not sure it's worth it at this point
    if (fpgaMajorVersion == 1) {
        // The base clock value is 48Mhz, so divide out the 48 leaving a 1 usec
        // resolution
        unsigned baseClock = 0;
        unsigned timerInterval = 48;  // use 5.6 bits (leaving about 10)

        // Compute how many bits are needed to represent the entire amount. The
        // first 10 will be absorbed by timerValue, and up to 16 more by
        // baseClockValue. If more than 26 bits (64 seconds) are given, it is
        // too large.
        int bits = (int) floor(log((double)period_usec)/log(2.0)) + 1;

        if (bits < 17) { // 0-1023 usec
            // Absorb the entire delay into delayMicros
            baseClock = period_usec - 1;
        } else if (bits <= 26) { // up to about 64 seconds
            timerInterval = 48000; // = 2^(15.55), about 1ms (new minimum step size)
            baseClock = (period_usec / 1000) - 1;
        } else {
            throw ProtocolException("Input strobe period too large");
        }

        FPGARegisterCodes::V1 codes;
        fpga.writeRegister(bus, codes.CONTINUOUS_STROBE_TIMER_INTERVAL_DIVISOR, timerInterval);
        fpga.writeRegister(bus, codes.CONTINUOUS_STROBE_BASE_CLOCK_DIVISOR, baseClock);

    } else if (fpgaMajorVersion == 3) {
        unsigned long counts = period_usec * 48;
        FPGARegisterCodes::V3 codes;
        fpga.writeRegister(bus, codes.CONTINUOUS_STROBE_TIMER_MSB, (counts >> 16) & 0xffff);
        fpga.writeRegister(bus, codes.CONTINUOUS_STROBE_TIMER_LSB,  counts        & 0xffff);

    } else {
        throw FeatureException("unsupported FPGA major version");
    }
}

void ContinuousStrobeFeature_FPGA::setContinuousStrobeEnable(const Protocol &protocol,
        const Bus &bus, unsigned short strobe_id, bool enable) {
    /* This delegates to the OOIStrobeLampProtocol.  Note that (like the rest
     * of this implementation) some devices like the STS will not work if this
     * approach is taken.  A separate generic continous strobe feature is
     * recommended for such devices.
     */
    vector<ProtocolHelper *> helpers;
    OOIStrobeLampProtocol *lampProtocol = new OOIStrobeLampProtocol();
    helpers.push_back(lampProtocol);

    StrobeLampFeature feature(helpers);
    OOIProtocol proto;
    feature.setStrobeLampEnable(proto, bus, enable);
}

FeatureFamily ContinuousStrobeFeature_FPGA::getFeatureFamily() {
    FeatureFamilies families;

    return families.CONTINUOUS_STROBE;
}
