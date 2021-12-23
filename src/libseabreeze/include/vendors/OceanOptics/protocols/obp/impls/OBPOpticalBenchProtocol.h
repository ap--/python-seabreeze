/***************************************************//**
 * @file    OBPOpticalBenchProtocol.h
 * @date    January 2015
 * @author  Kirk Clendinning, Heliospectra
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2015, Ocean Optics Inc, Heliospectra AB
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

#ifndef SEABREEZE_OBP_OPTICAL_BENCHROTOCOL_H
#define SEABREEZE_OBP_OPTICAL_BENCHROTOCOL_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/interfaces/OpticalBenchProtocolInterface.h"
#include <vector>

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPOpticalBenchProtocol : public OpticalBenchProtocolInterface {
    public:
        OBPOpticalBenchProtocol();
        virtual ~OBPOpticalBenchProtocol();

        virtual unsigned short int readOpticalBenchSlitWidthMicrons(const Bus &bus);
        virtual unsigned short int readOpticalBenchFiberDiameterMicrons(const Bus &bus);
        virtual std::string *readOpticalBenchID(const Bus &bus);
        virtual std::string *readOpticalBenchSerialNumber(const Bus &bus);
        virtual std::string *readOpticalBenchCoating(const Bus &bus);
        virtual std::string *readOpticalBenchFilter(const Bus &bus);
        virtual std::string *readOpticalBenchGrating(const Bus &bus);
    };
  }
}

#endif
