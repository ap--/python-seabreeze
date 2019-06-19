/***************************************************//**
 * @file    OOIUSBEndpointMaps.h
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

#ifndef OOIUSBENDPOINTMAPS_H
#define OOIUSBENDPOINTMAPS_H

namespace seabreeze {

    class OOIUSBCypressEndpointMap {
    public:
        int getLowSpeedInEP() const;
        int getLowSpeedOutEP() const;
        int getHighSpeedInEP() const;
        int getHighSpeedIn2EP() const;

    protected:
        int lowSpeedIn;
        int lowSpeedOut;
        int highSpeedIn;
        int highSpeedIn2;
    };

    /* This map is appropriate for the following spectrometers:
     * USB2000 (and its variants), HR2000, ADC1000-USB,
     * NIR256, NIR512, SAS
     */
    class OOIUSBLegacy2KEndpointMap : public OOIUSBCypressEndpointMap {
    public:
        OOIUSBLegacy2KEndpointMap();
        virtual ~OOIUSBLegacy2KEndpointMap();

    };

    /* This map is appropriate for the following spectrometers:
     * HR2000+, HR4000, Maya, Maya2000Pro, MayaLSL, QE65000, USB2000+, USB4000
     */
    class OOIUSBFPGAEndpointMap : public OOIUSBCypressEndpointMap {
    public:
        OOIUSBFPGAEndpointMap();
        virtual ~OOIUSBFPGAEndpointMap();

    };

    class OOIUSBJazEndpointMap : public OOIUSBCypressEndpointMap {
    public:
        OOIUSBJazEndpointMap();
        virtual ~OOIUSBJazEndpointMap();
    };

    /* This defines a simple interface for an endpoint map that has a one input
     * and one output endpoint.
     */
    class OOIUSBBidrectionalEndpointMap {
        public:
            OOIUSBBidrectionalEndpointMap();
            virtual ~OOIUSBBidrectionalEndpointMap();
            virtual int getPrimaryInEndpoint() const;
            virtual int getPrimaryOutEndpoint() const;
        protected:
            int primaryInEndpoint;
            int primaryOutEndpoint;
    };

    /* This map is appropriate for the following spectrometers:
     * Ventana
     */
    class OOIUSBVentanaEndpointMap : public OOIUSBBidrectionalEndpointMap {
        public:
            OOIUSBVentanaEndpointMap();
            virtual ~OOIUSBVentanaEndpointMap();
    };

    class OOIUSBDualBidirectionalEndpointMap : public OOIUSBBidrectionalEndpointMap {
        public:
            OOIUSBDualBidirectionalEndpointMap();
            virtual ~OOIUSBDualBidirectionalEndpointMap();
            int getSecondaryInEndpoint() const;
            int getSecondaryOutEndpoint() const;

        protected:
            int secondaryInEndpoint;
            int secondaryOutEndpoint;
    };

    /* This map is appropriate for the following spectrometers:
     * STS, QEPro
     */
    class OOIUSBSimpleDualEndpointMap : public OOIUSBDualBidirectionalEndpointMap {
        public:
            OOIUSBSimpleDualEndpointMap();
            virtual ~OOIUSBSimpleDualEndpointMap();
    };

}


#endif /* OOIUSBENDPOINTMAPS_H */
