/***************************************************//**
 * @file    NativeUSB.h
 * @date    October 31, 2007
 * @author  Ocean Optics, Inc.
 *
 * This provides a relatively simple interface for
 * opening, closing, writing to, and reading from Ocean
 * Optics USB Devices.  It has been extended to provide
 * more complete USB functionality at least for Linux,
 * and equivalent functionality should be brought back
 * in for Windows and MacOSX.
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
 ******************************************************/

#ifndef NATIVEUSB_H
#define NATIVEUSB_H

#ifdef __cplusplus
extern "C" {
#endif /* cplusplus */

#define OPEN_OK          		0
#define NO_DEVICE_FOUND 		-1
#define NO_DEVICE_MATCH 		-2
#define CLAIM_INTERFACE_FAILED 	-3
#define CLOSE_OK         		0
#define CLOSE_ERROR     		-1
#define WRITE_FAILED    		-1
#define READ_FAILED     		-1
#define ABORT_OK         		0
#define ABORT_FAILED    		-1
#define RESET_OK         		0
#define RESET_FAILED    		-1

struct USBConfigurationDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short wTotalLength;
    unsigned char bNumInterfaces;
    unsigned char bConfigurationValue;
    unsigned char iConfiguration;
    unsigned char bmAttributes;
    unsigned char MaxPower;
};

struct USBDeviceDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize0;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialNumber;
    unsigned char bNumConfigurations;
};

struct USBInterfaceDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bInterfaceNumber;
    unsigned char bAlternateSetting;
    unsigned char bNumEndpoints;
    unsigned char bInterfaceClass;
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char iInterface;
};

struct USBEndpointDescriptor {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned char bEndpointAddress;
    unsigned char bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char bInterval;
};

//------------------------------------------------------------------------------
// This function attempts to discover all devices with the given product
// and vendor IDs.  Descriptors for each found device will be placed in the
// provided long buffer.
//
// PARAMETERS:
// vendorID:  The vendor ID to match with devices on the bus
// productID: The product ID to match with devices on the bus
// output:    A buffer of longs that will be populated with unique IDs for each
//            device found that matches the VID and PID
// max_devices: A limit on how many IDs can be put into the output buffer
//
// RETURN VALUE:
// The number of devices successfully found, or -1 if there was an error.
int
USBProbeDevices(int vendorID, int productID, unsigned long *output,
        int max_devices);

//------------------------------------------------------------------------------
// This function attempts to open a device with the given product and vendor
// ID's at the specified index.
//
// PARAMETERS:
// deviceID:    The ID of a device that has been provided by probeDevices().
// errorCode: A pointer to an integer that will be set to an error code of
//            either NO_DEVICE_FOUND or NO_DEVICE_MATCH if an error occured
//            while trying to open the device or OPEN_OK if the device was
//            found and opened.
//
// RETURN VALUE:
// Returns a void pointer which equals a handle to the device if the device was
// found and opened successfully, or NULL if the device was not opened
// successfully.  The value of the 'errorCode' parameter should be checked
// against the following before using the handle:
//  - NO_DEVICE_FOUND signifying that no device was found with the specified ID
//        or that the device with the specified ID could not be opened
//------------------------------------------------------------------------------
void *
USBOpen(unsigned long deviceID, int *errorCode);

//------------------------------------------------------------------------------
// This function attempts to close the device attached to the given handle.
//
// PARAMETERS:
// handle: The device handle obtained via the open() function.
//
// RETURN VALUE:
// Returns an integer which will be equal to either:
//  - CLOSE_OK if the device was closed successfully
//  - CLOSE_ERROR if some error occured
//------------------------------------------------------------------------------
int
USBClose(void *handle);

//------------------------------------------------------------------------------
// This function writes the given data to the device attached to the given
// handle.
//
// PARAMETERS:
// handle: The device handle obtained via the open() function.
// endpoint: The endpoint on the device to write the data to.
// data: A pointer to the dynamically allocated byte array of data to be written
// size: The number of bytes to be written
//
// RETURN VALUE:
// Returns an integer which will be equal to either:
//  - The number of bytes written to the endpoint if the write was successful
//  - WRITE_FAILED if the data was not written to the device
//------------------------------------------------------------------------------
int
USBWrite(void *handle, unsigned char endpoint, char * data, int numberOfBytes);

//------------------------------------------------------------------------------
// This function reads data from the device attached to the given handle into
// the specified byte array.
//
// PARAMETERS:
// handle: The device handle obtained via the open() function.
// endpoint: The endpoint on the device to read the data from.
// data: A pointer to the dynamically allocated byte array to store the data.
// size: The number of bytes to be read.
//
// RETURN VALUE:
// Returns an integer which will be equal to either:
//  - The number of bytes read from the endpoint if the read was successful
//  - READ_FAILED if the data was not successfully read from the device
//------------------------------------------------------------------------------
int
USBRead(void *handle, unsigned char endpoint, char * data, int numberOfBytes);

//------------------------------------------------------------------------------
// This function attempts to clear any stall on the given endpoint.
//
// PARAMETERS:
// handle: The device handle obtained via the open() function.
// endpoint: The endpoint on the device to clear a stall condition on.
//
// RETURN VALUE:
// No value is returned (void).
//------------------------------------------------------------------------------
void
USBClearStall(void *handle, unsigned char endpoint);

int
USBGetDeviceDescriptor(void *handle, struct USBDeviceDescriptor *desc);

int
USBGetInterfaceDescriptor(void *handle, struct USBInterfaceDescriptor *desc);

int
USBGetEndpointDescriptor(void *handle, int endpoint_index, struct USBEndpointDescriptor *desc);

int
USBGetStringDescriptor(void *handle, unsigned int string_index, char *buffer, int maxLength);


#ifdef __cplusplus
}
#endif /* _cplusplus */

#endif /* NATIVEUSB_H */
