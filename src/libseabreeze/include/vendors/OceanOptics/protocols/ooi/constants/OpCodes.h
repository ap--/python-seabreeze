/***************************************************//**
 * @file    OpCodes.h
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

#ifndef SEABREEZE_OPCODES_H
#define SEABREEZE_OPCODES_H

#include "common/SeaBreeze.h"

namespace seabreeze {
  namespace ooiProtocol {
    class OpCodes {
    public:

        /* Set the integration time */
        static const unsigned char OP_ITIME;

        /* Set the strobe/lamp enable */
        static const unsigned char OP_STROBE;

        /* Read 15 bytes from an EEPROM slot */
        static const unsigned char OP_GETINFO;

        /* Write 15 bytes to an EEPROM slot */
        static const unsigned char OP_SETINFO;

        /* Request a spectrum */
        static const unsigned char OP_REQUESTSPEC;

        /* Set the trigger mode */
        static const unsigned char OP_SETTRIGMODE;

        /* Set the thermoelectric cooling enable on the QE */
        static const unsigned char OP_TECENABLE_QE;

        /* Read the current TEC temperature on the QE */
        static const unsigned char OP_READTEC_QE;

        /* Set the TEC set point on the QE */
        static const unsigned char OP_TECSETTEMP_QE;

        /* Write a field to the FPGA status register */
        static const unsigned char OP_WRITE_REGISTER;

        /* Read a field from the FPGA status register */
        static const unsigned char OP_READ_REGISTER;

        /* Read the irradiance calibration from EEPROM (if available) */
        static const unsigned char OP_READ_IRRAD_CAL;

        /* Write the irradiance calibration to EEPROM (if available) */
        static const unsigned char OP_WRITE_IRRAD_CAL;

    };
  }
}

#endif
