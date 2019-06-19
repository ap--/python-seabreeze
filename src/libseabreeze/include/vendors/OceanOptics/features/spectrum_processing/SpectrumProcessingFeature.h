/***************************************************//**
 * @file    SpectrumProcessingFeature.h
 * @date    February 2015
 * @author  Kirk Clendinning, Heliospectra
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

#ifndef SPECTRUMPROCESSINGFEATURE_H
#define SPECTRUMPROCESSINGFEATURE_H

#include <vector>

#include "vendors/OceanOptics/features/spectrum_processing/SpectrumProcessingFeatureInterface.h"
#include "common/protocols/Protocol.h"
#include "common/features/FeatureImpl.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"
#include "common/exceptions/IllegalArgumentException.h"

namespace seabreeze {

    class SpectrumProcessingFeature
                : public FeatureImpl, public SpectrumProcessingFeatureInterface {
    public:
        SpectrumProcessingFeature(std::vector<ProtocolHelper *> helpers);
        virtual ~SpectrumProcessingFeature();
        virtual unsigned char readSpectrumProcessingBoxcarWidth(
        	const Protocol &protocol, const Bus &bus) 
            throw (FeatureException);
        virtual unsigned short int readSpectrumProcessingScansToAverage(
        	const Protocol &protocol, const Bus &bus) 
        	throw (FeatureException);
        virtual void writeSpectrumProcessingBoxcarWidth(const Protocol &protocol,
        	const Bus &bus, unsigned char boxcarWidth) 
        	throw (FeatureException, IllegalArgumentException);
        virtual void writeSpectrumProcessingScansToAverage(const Protocol &protocol,
                const Bus &bus, unsigned short int scansToAverage)
                throw (FeatureException, IllegalArgumentException);
                
        /* Overriding from Feature */
        virtual FeatureFamily getFeatureFamily();
    };

}

#endif /* SPECTRUMPROCESSINGFEATURE_H */