/***************************************************//**
 * @file    AcquisitionDelayFeature_FPGA.cpp
 * @date    November 2015
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

#include "vendors/OceanOptics/features/acquisition_delay/AcquisitionDelayFeature_FPGA.h"
#include "vendors/OceanOptics/protocols/interfaces/FPGARegisterProtocolInterface.h"
#include "common/exceptions/FeatureControlException.h"
#include <string>

#define DEFAULT_COUNTS_PER_MICROSECOND          48
#define DEFAULT_MIN_ACQUISITION_DELAY_COUNTS    0
#define DEFAULT_MAX_ACQUISITION_DELAY_COUNTS    65535
#define DEFAULT_ACQUISITION_DELAY_INCREMENT_MICROSECONDS 1
#define DEFAULT_ACQUISITION_DELAY_REGISTER      0x28

using namespace seabreeze;
using namespace std;

AcquisitionDelayFeature_FPGA::AcquisitionDelayFeature_FPGA(
        std::vector<ProtocolHelper *> helpers)
            : AcquisitionDelayFeature(helpers) {

    this->countsPerMicrosecond = DEFAULT_COUNTS_PER_MICROSECOND;
    this->minimumDelayCounts = DEFAULT_MIN_ACQUISITION_DELAY_COUNTS;
    this->maximumDelayCounts = DEFAULT_MAX_ACQUISITION_DELAY_COUNTS;
    this->incrementMicroseconds = DEFAULT_ACQUISITION_DELAY_INCREMENT_MICROSECONDS;

    this->acquisitionDelayRegister = DEFAULT_ACQUISITION_DELAY_REGISTER;
}

AcquisitionDelayFeature_FPGA::~AcquisitionDelayFeature_FPGA() {

}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
void AcquisitionDelayFeature_FPGA::setAcquisitionDelayMicroseconds(
                    const Protocol &protocol, const Bus &bus,
                    const unsigned long delayMicros)
                    throw (FeatureException) {

    unsigned long counts = microsecondsToCounts(delayMicros);

    if((counts > this->maximumDelayCounts)
            || (counts < this->minimumDelayCounts)) {
        throw FeatureException("Supplied acquisition delay out of range.");
    }

    ProtocolHelper *proto = NULL;
    FPGARegisterProtocolInterface *fpga = NULL;

    try {
        proto = lookupProtocolImpl(protocol);
        fpga = static_cast<FPGARegisterProtocolInterface *>(proto);
    } catch (FeatureProtocolNotFoundException &fpnfe) {
        string error("Could not find matching protocol implementation to write FPGA register");
        throw FeatureProtocolNotFoundException(error);
    }

    if(NULL == fpga) {
        string error("Unable to delegate acquisition delay to FPGA protocol handler");
        throw FeatureProtocolNotFoundException(error);
    }

    try {
        fpga->writeRegister(bus, this->acquisitionDelayRegister, counts);

        /* It might be possible to read back the setting from the FPGA, but it is
         * not known whether all devices do this correctly.  For now, just cache
         * the value to be used with the next "get" request.  A subclass could always
         * override the getter to call down to the device if appropriate.
         */
        this->lastAcquisitionDelayMicroseconds = delayMicros;
        this->lastAcquisitionDelayValid = true;
    } catch (ProtocolException &pe) {
        string error("Caught protocol exception: ");
        error += pe.what();
        throw FeatureControlException(error);
    }
}

unsigned long AcquisitionDelayFeature_FPGA::getAcquisitionDelayIncrementMicroseconds(
            const Protocol &protocol, const Bus &bus)
            throw (FeatureException) {
    return this->incrementMicroseconds;
}

unsigned long AcquisitionDelayFeature_FPGA::getAcquisitionDelayMaximumMicroseconds(
            const Protocol &protocol, const Bus &bus)
            throw (FeatureException) {
    return countsToMicroseconds(this->maximumDelayCounts);
}

unsigned long AcquisitionDelayFeature_FPGA::getAcquisitionDelayMinimumMicroseconds(
            const Protocol &protocol, const Bus &bus)
            throw (FeatureException) {
    return countsToMicroseconds(this->minimumDelayCounts);
}

unsigned long AcquisitionDelayFeature_FPGA::countsToMicroseconds(unsigned long counts) {
    return counts/this->countsPerMicrosecond;
}

unsigned long AcquisitionDelayFeature_FPGA::microsecondsToCounts(unsigned long microseconds) {
    return (microseconds * this->countsPerMicrosecond);
}

