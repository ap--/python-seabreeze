/***************************************************//**
 * @file    GainAdjustedSpectrometerFeature.h
 * @date    July 2009
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

#ifndef GAINADJUSTEDSPECTROMETERFEATURE_H
#define GAINADJUSTEDSPECTROMETERFEATURE_H

#include "vendors/OceanOptics/features/spectrometer/OOISpectrometerFeature.h"
#include "vendors/OceanOptics/features/spectrometer/ProgrammableSaturationFeature.h"

namespace seabreeze {

    class GainAdjustedSpectrometerFeature : public OOISpectrometerFeature {
    public:
        GainAdjustedSpectrometerFeature(
                ProgrammableSaturationFeature *saturationFeature);
        virtual ~GainAdjustedSpectrometerFeature();

        virtual unsigned int getSaturationLevel();

        /* Inherited from Feature */
        virtual bool initialize(const Protocol &protocol, const Bus &bus);

    protected:
        ProgrammableSaturationFeature *saturation;
    };

}

#endif /* GAINADJUSTEDSPECTROMETERFEATURE_H */
