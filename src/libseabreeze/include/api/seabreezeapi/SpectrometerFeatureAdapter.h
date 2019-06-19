/***************************************************//**
 * @file    SpectrometerFeatureAdapter.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze OOISpectrometerFeature instances.
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

#ifndef SEABREEZE_SPECTROMETER_FEATURE_ADAPTER_H
#define SEABREEZE_SPECTROMETER_FEATURE_ADAPTER_H

#include "api/seabreezeapi/FeatureAdapterTemplate.h"
#include "common/buses/Bus.h"
#include "common/protocols/Protocol.h"
#include "vendors/OceanOptics/features/spectrometer/OOISpectrometerFeatureInterface.h"

namespace seabreeze {
    namespace api {

        class SpectrometerFeatureAdapter : public FeatureAdapterTemplate<OOISpectrometerFeatureInterface> {
        public:
            SpectrometerFeatureAdapter(
                OOISpectrometerFeatureInterface *spec,
                    const FeatureFamily &f,
                    Protocol *p, Bus *b, unsigned short instanceIndex);
            virtual ~SpectrometerFeatureAdapter();

            /* Spectrometer commands */
            int getUnformattedSpectrum(int *errorCode,unsigned char *buffer, int bufferLength);
			int getFastBufferSpectrum(int *errorCode, unsigned char *buffer, int bufferLength, unsigned int numberOfSamplesToRetrieve);
            int getFormattedSpectrum(int *errorCode,double* buffer, int bufferLength);
            int getUnformattedSpectrumLength(int *errorCode);
            int getFormattedSpectrumLength(int *errorCode);
            void setTriggerMode(int *errorCode, int mode);
            int getWavelengths(int *errorCode, double *wavelengths, int length);
            int getElectricDarkPixelCount(int *errorCode);
            int getElectricDarkPixelIndices(int *errorCode, int *indices, int length);
			int getOpticalDarkPixelCount(int *errorCode);
			int getOpticalDarkPixelIndices(int *errorCode, int *indices, int length);
			int getActivePixelCount(int *errorCode);
			int getActivePixelIndices(int *errorCode, int *indices, int length);
			unsigned short getNumberOfPixels(int *errorCode);
            void setIntegrationTimeMicros(int *errorCode,
            unsigned long integrationTimeMicros);
            long getMinimumIntegrationTimeMicros(int *errorCode);
            long getMaximumIntegrationTimeMicros(int *errorCode);
            double getMaximumIntensity(int *errorCode);
        };

    }
}

#endif
