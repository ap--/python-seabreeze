/***************************************************//**
 * @file    OpCodes.cpp
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

#include "common/globals.h"
#include "vendors/OceanOptics/protocols/ooi/constants/OpCodes.h"

using namespace seabreeze;
using namespace seabreeze::ooiProtocol;

const byte OpCodes::OP_ITIME            = 0x02;
const byte OpCodes::OP_STROBE           = 0x03;
const byte OpCodes::OP_GETINFO          = 0x05;
const byte OpCodes::OP_SETINFO          = 0x06;
const byte OpCodes::OP_REQUESTSPEC      = 0x09;
const byte OpCodes::OP_SETTRIGMODE      = 0x0A;
const byte OpCodes::OP_WRITE_REGISTER   = 0x6A;
const byte OpCodes::OP_READ_REGISTER    = 0x6B;
const byte OpCodes::OP_READ_IRRAD_CAL   = 0x6D;
const byte OpCodes::OP_WRITE_IRRAD_CAL  = 0x6E;
const byte OpCodes::OP_TECENABLE_QE     = 0x71;
const byte OpCodes::OP_READTEC_QE       = 0x72;
const byte OpCodes::OP_TECSETTEMP_QE    = 0x73;
