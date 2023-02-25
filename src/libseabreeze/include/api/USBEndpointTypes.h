/***************************************************//**
 * @file    USBEndpointTypes.h
 * @date    May 2016
 * @author  Ocean Optics, Inc.
 *
 * This provides a structure defining USB endpoints in a manner that
 * can be used with the raw USB read/write capabilities of the API.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2016, Ocean Optics Inc
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
#ifndef USBENDPOINTTYPES_H
#define USBENDPOINTTYPES_H

// usb endpoints are associated with a particular device.
typedef enum usbEndpointType {
    kEndpointTypePrimaryOut, // slow speed
    kEndpointTypePrimaryIn,  // slow speed
    kEndpointTypeSecondaryOut, // could be high speed
    kEndpointTypeSecondaryIn,  // could be high speed
    kEndpointTypeSecondaryIn2  // generally high speed
} usbEndpointType;

#endif /* USBENDPOINTTYPES_H */
