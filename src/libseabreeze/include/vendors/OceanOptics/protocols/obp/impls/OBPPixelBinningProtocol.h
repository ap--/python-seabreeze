/***************************************************//**
 * @file    OBPPixelBinningProtocol.h
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

#ifndef OBPPIXELBINNINGPROTOCOL_H
#define OBPPIXELBINNINGPROTOCOL_H

#include "common/SeaBreeze.h"
#include "common/buses/Bus.h"
#include "vendors/OceanOptics/protocols/interfaces/PixelBinningProtocolInterface.h"
#include <string>

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPPixelBinningProtocol : public PixelBinningProtocolInterface {
    public:
        OBPPixelBinningProtocol();
        virtual ~OBPPixelBinningProtocol();

        /**
         * Get the pixel binning factor of the device.
         */
        virtual unsigned char readPixelBinningFactor(const Bus &bus);

        /**
         * Set the pixel binning factor on the device.
         */
        virtual void writePixelBinningFactor(const Bus &bus, const unsigned char binningFactor);

        /**
         * Get the default pixel binning factor of the device.
         */
        virtual unsigned char readDefaultPixelBinningFactor(const Bus &bus);

        /**
         * Set the default pixel binning factor on the device.
         */
        virtual void writeDefaultPixelBinningFactor(const Bus &bus, const unsigned char binningFactor);

        /**
         * Reset the default pixel binning factor on the device. This will reinstate the factory default of 0.
         */
        virtual void writeDefaultPixelBinningFactor(const Bus &bus);

        /**
         * Get the maximum pixel binning factor of the device.
         */
        virtual unsigned char readMaxPixelBinningFactor(const Bus &bus);
    };
  }
}

#endif /* OBPPIXELBINNINGPROTOCOL_H */
