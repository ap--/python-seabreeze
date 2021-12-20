/***************************************************//**
 * @file    FPGARegisterFeature.cpp
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
#include "vendors/OceanOptics/features/fpga_register/FPGARegisterFeature.h"
#include "vendors/OceanOptics/protocols/interfaces/FPGARegisterProtocolInterface.h"
#include "vendors/OceanOptics/protocols/ooi/impls/FPGARegisterProtocol.h"
#include "vendors/OceanOptics/protocols/ooi/constants/FPGARegisterCodes.h"
#include "common/exceptions/FeatureProtocolNotFoundException.h"
#include "common/exceptions/FeatureControlException.h"
#include "api/seabreezeapi/FeatureFamilies.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;
using namespace seabreeze::api;
using namespace std;

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

FPGARegisterFeature::FPGARegisterFeature() {
    this->protocols.push_back(new FPGARegisterProtocol());
}

FPGARegisterFeature::~FPGARegisterFeature() {

}

void FPGARegisterFeature::writeRegister(const Bus &bus, unsigned char address,
        unsigned int value) {

    FPGARegisterProtocol protocol;
    FPGARegisterProtocolInterface *fpga = &protocol;

    try {
        fpga->writeRegister(bus, address, value);
    } catch (ProtocolException &pe) {
        string error = string("Caught protocol exception: ") + pe.what();
        throw FeatureControlException(error);
    }
}

unsigned int FPGARegisterFeature::readRegister(const Bus &bus, unsigned char address) {

    unsigned int retval = 0;

    FPGARegisterProtocol protocol;
    FPGARegisterProtocolInterface *fpga = &protocol;

    try {
        retval = fpga->readRegister(bus, address);
    } catch (ProtocolException &pe) {
        string error = string("Caught protocol exception: ") + pe.what();
        throw FeatureControlException(error);
    }

    return retval;
}

unsigned char FPGARegisterFeature::getMajorVersion(const Bus &bus) {
    unsigned char retval = 0;

    FPGARegisterCodes::Base codes;
    FPGARegisterProtocol protocol;
    FPGARegisterProtocolInterface *fpga = &protocol;

    try {
        // firmware version appears to be packed into two bytes as follows:
        //
        //    MSB       LSB
        //  01234567 01234567
        //  \__/\_______/\__/
        // major  minor  patch

        unsigned int version = fpga->readRegister(bus, codes.FIRMWARE_VERSION);
        retval = (version >> 12) & 0x0f;

    } catch (ProtocolException &pe) {
        string error = string("Caught protocol exception: ") + pe.what();
        throw FeatureControlException(error);
    }

    return retval;
}

FeatureFamily FPGARegisterFeature::getFeatureFamily() {
    FeatureFamilies families;

    return families.UNDEFINED;  // this is an internal-only feature
}
