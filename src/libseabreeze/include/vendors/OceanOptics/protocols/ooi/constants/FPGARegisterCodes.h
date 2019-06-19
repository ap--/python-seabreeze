/***************************************************//**
 * @file    FPGARegisterCodes.h
 * @date    October 2012
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

#ifndef SEABREEZE_FPGA_REGISTER_CODES_H
#define SEABREEZE_FPGA_REGISTER_CODES_H

#include "common/features/Feature.h"
#include "common/buses/Bus.h"
#include "common/exceptions/FeatureException.h"

namespace seabreeze {

    namespace ooiProtocol {

        namespace FPGARegisterCodes {

            /**
            * These codes were taken from the USB2000+ Data Sheet as it existed
            * in October 2012.  They are believed to apply to the vast majority
            * of Ocean Optics spectrometers, but because some variance is expected
            * between models and over time (especially with custom OEM firmware),
            * you should check your spectrometer's documentation before assuming
            * these codes are correct for your device.
            */
            class Base {
            public:
                static const byte FIRMWARE_VERSION;
                static const byte SINGLE_STROBE_HIGH_CLOCK_TRANSITION_DELAY_COUNT;
                static const byte SINGLE_STROBE_LOW_CLOCK_TRANSITION_DELAY_COUNT;
                static const byte LAMP_ENABLE;
                static const byte GPIO_MUX_REGISTER;
                static const byte GPIO_OUTPUT_ENABLE;
                static const byte GPIO_DATA_REGISTER;
                static const byte OFFSET_VALUE;
                static const byte OFFSET_CONTROL;
                static const byte MAXIMUM_SATURATION_LEVEL;

            };

            class V1 : public Base {
            public:
                static const byte CONTINUOUS_STROBE_TIMER_INTERVAL_DIVISOR;
                static const byte CONTINUOUS_STROBE_BASE_CLOCK_DIVISOR;
            };

            class V3 : public Base {
            public:
                static const byte CONTINUOUS_STROBE_TIMER_MSB;
                static const byte CONTINUOUS_STROBE_TIMER_LSB;
            };
        }
    }
}

#endif
