/***************************************************//**
 * @file    WindowsGUID.h
 * @date    June 2009
 * @author  Ocean Optics, Inc.
 *
 * This provides GUID definitions for supported devices.
 * These must match what appears in the .inf file for the
 * devices to be found properly through the WinUSB API.
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

#pragma once

#ifndef SEABREEZE_WINDOWS_GUID_H
#define SEABREEZE_WINDOWS_GUID_H

// {BAD36DAB-A3D2-4a0e-8B2E-DA36202187D4}
DEFINE_GUID(GUID_DEVCLASS_OCEANOPTICS_USB,
0xbad36dab, 0xa3d2, 0x4a0e, 0x8b, 0x2e, 0xda, 0x36, 0x20, 0x21, 0x87, 0xd4);

// {DBBAD306-1786-4f2e-A8AB-340D45F0653F}
DEFINE_GUID(GUID_DEVINTERFACE_OCEANOPTICS_USB,
0xdbbad306, 0x1786, 0x4f2e, 0xa8, 0xab, 0x34, 0xd, 0x45, 0xf0, 0x65, 0x3f);

#endif
