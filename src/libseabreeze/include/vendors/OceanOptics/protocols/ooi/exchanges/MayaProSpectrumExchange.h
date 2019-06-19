/***************************************************//**
 * @file    MayaProSpectrumExchange.h
 * @date    November 2009
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

#ifndef SEABREEZE_MAYAPROSPECTRUMEXCHANGE_H
#define SEABREEZE_MAYAPROSPECTRUMEXCHANGE_H

#include "vendors/OceanOptics/protocols/ooi/exchanges/ReadSpectrumExchange.h"
#include "vendors/OceanOptics/features/spectrometer/GainAdjustedSpectrometerFeature.h"
#include "common/Data.h"

namespace seabreeze {
  namespace ooiProtocol {
    class MayaProSpectrumExchange : public ReadSpectrumExchange {
    public:
        MayaProSpectrumExchange(unsigned int readoutLength,
                unsigned int numberOfPixels,
                GainAdjustedSpectrometerFeature *spectrometer);
        virtual ~MayaProSpectrumExchange();

        /* Inherited */
        virtual Data *transfer(TransferHelper *helper) throw (ProtocolException);

    private:
        /* This is necessary so that the saturation level which is determined
         * at initialization is available to certain protocol messages.
         */
        GainAdjustedSpectrometerFeature *spectrometerFeature;
    };
  }
}

#endif
