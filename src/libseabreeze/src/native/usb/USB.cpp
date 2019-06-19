/***************************************************//**
 * @file    USB.cpp
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
#include "native/usb/USB.h"
#include "native/usb/NativeUSB.h"
#include <stdio.h>  /* For debugging, feel free to replace with iostream */
#include <string.h> /* for memset() */

using namespace seabreeze;
using namespace std;

bool seabreeze::USB::verbose = false;

USB::USB(unsigned long id) {
    this->opened = false;
    this->descriptor = NULL;
    this->deviceID = id;
}

USB::~USB() {

    /* Try to close the device descriptor if it is not already. */
    if(true == this->opened && this->descriptor != NULL) {
        this->close();
    }
}

bool USB::open() {
    int error = 0;

    if(true == this->opened || this->descriptor != NULL) {
        this->close();
    }

    this->descriptor = USBOpen(this->deviceID, &error);

    if(0 == error) {
        this->opened = true;
        if(true == this->verbose) {
            fprintf(stderr, "Opened device with ID %ld\n", deviceID);
        }
        return true;
    }
    else
    	printf("Could not open device with ID %ld. error=%d\n", deviceID, error);

    if(true == this->verbose) {
        fprintf(stderr, "Warning: failed to open device with ID %ld, error=%d\n",
                deviceID, error);
    }
    this->descriptor = NULL;
    /* FIXME: this should probably throw a specific exception instead. */
    return false;
}

bool USB::close() {

    int flag = 0;
    bool retval = false;

    if(NULL != this->descriptor) {
        flag = USBClose(this->descriptor);
        if(0 == flag) {
            retval = true;
        }
        /* FIXME: this should throw an exception on false.  Make sure the
         * descriptor and opened flags get cleared in either case.
         */
    }

    this->descriptor = NULL;
    this->opened = false;
    return retval;
}

int USB::write(int endpoint, void *data, unsigned int length_bytes) {

    int flag = 0;

    if(true == this->verbose) {
        // set 'true' for hexdump of output bytes BEFORE actual USB transfer
        this->describeTransfer(">>", length_bytes, data, endpoint, false); 
    }

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready or opened */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to write to a USB device that is not opened.\n");
        }
        return -1;
    }

    flag = USBWrite(this->descriptor, (unsigned char)endpoint, (char *)data, (int)length_bytes);

    if(flag < 0) {
        /* FIXME: throw an exception here */
        if(true == this->verbose) {
            fprintf(stderr, "Warning: got error %d while trying to write %d bytes over USB endpoint %d\n",
                    flag, length_bytes, endpoint);
        }
        return -1;
    }

    if(true == this->verbose) {
        this->usbHexDump(data, length_bytes, endpoint);
    }

    return flag;
}

int USB::read(int endpoint, void *data, unsigned int length_bytes) {
    int flag = 0;

    if(true == this->verbose) {
        this->describeTransfer("<<", length_bytes, data, endpoint, false);
    }

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready or opened */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a USB device that is not opened.\n");
        }
        return -1;
    }

    flag = USBRead(this->descriptor, (unsigned char)endpoint, (char *)data, (int)length_bytes);

    if(flag < 0) {
        /* FIXME: throw an exception here */
        if(true == this->verbose) {
            fprintf(stderr, "Warning: got error %d while trying to read %d bytes over USB endpoint %d\n",
                    flag, length_bytes, endpoint);
        }
        return -1;
    }

    if(true == this->verbose) {
        this->usbHexDump(data, length_bytes, endpoint);
    }

    return flag;
}

void USB::clearStall(int endpoint) {

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready or opened */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to access a USB device that is not opened.\n");
        }
        return;
    }

    USBClearStall(this->descriptor, (unsigned char)endpoint);
}

void USB::setVerbose(bool v) {
    verbose = v;
}

int USB::getDeviceDescriptor(struct USBDeviceDescriptor *desc) {

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a USB device that is not opened.\n");
        }
        return -1;
    }

    return USBGetDeviceDescriptor(this->descriptor, desc);
}

int USB::getInterfaceDescriptor(USBInterfaceDescriptor *desc) {

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a USB device that is not opened.\n");
        }
        return -1;
    }

    return USBGetInterfaceDescriptor(this->descriptor, desc);
}

int USB::getEndpointDescriptor(int index, USBEndpointDescriptor *epDesc) {

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a USB device that is not opened.\n");
        }
        return -1;
    }

    return USBGetEndpointDescriptor(this->descriptor, index, epDesc);
}

string *USB::getStringDescriptor(int index) {
    string *retval;
    char buffer[64];
    int length;

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a USB device that is not opened.\n");
        }
        return NULL;
    }

    length = USBGetStringDescriptor(this->descriptor, index, buffer, 63);
    if(length > 0) {
        /* Got a valid string */
        buffer[length + 1] = '\0';  /* Guarantee a null terminator */
        retval = new string(buffer);
        return retval;
    }

    return NULL;
}

bool USB::isOpened() {
    return this->opened;
}

int USB::getMaxPacketSize() {

    struct USBInterfaceDescriptor intfDesc;
    struct USBEndpointDescriptor epDesc;
    int i;
    int flag;
    int retval = 0;

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a USB device that is not opened.\n");
        }
        return -1;
    }

    memset(&intfDesc, (int)0, sizeof(struct USBInterfaceDescriptor));
    memset(&epDesc, (int)0, sizeof(struct USBEndpointDescriptor));

    flag = getInterfaceDescriptor(&intfDesc);

    if(flag < 0) {
        /* FIXME: throw an exception for device not ready */
        return -1;
    }

    for(i = 0; i < intfDesc.bNumEndpoints; i++) {
        flag = getEndpointDescriptor(i, &epDesc);

        if(flag < 0) {
            /* FIXME: throw an exception for device not ready */
            return -1;
        }

        if(epDesc.wMaxPacketSize > retval) {
            retval = epDesc.wMaxPacketSize;
        }
    }

    if(true == this->verbose) {
        fprintf(stderr, "Max packet size is %d\n", retval);
    }

    return retval;
}

/* Debugging methods */
void USB::usbHexDump(void *x, int length, int endpoint) {
    /* FIXME: put in a system-independent timestamp here with usec resolution */
    fprintf(stderr, "Endpoint 0x%02X transferred %d bytes %s:\n",
            endpoint, length, endpoint & 0x80 ? "in" : "out");

    this->hexDump(x, length);
}

void USB::hexDump(void *x, int length) {
    /* This is a nice hexdump method that formats the given buffer into
     * blocks of 16 with the starting address in the far left column.  This
     * is being included to assist with USB device communication debugging.
     * Yes, it uses fprintf() and stderr because it was originally written
     * in C.  This can be ported to C++-style cerr::<< or ostream::<< if that
     * really seems important.
     */
    int i, j;
    int written;
    char buf[256];

    for(i = 0; i < (length / 16) + 1; i++) {
        written = 0;
        written += sprintf(buf + written, "[%04X]: ", (i * 16 & 0x00FFFF));
        for(j = 0; j < 16 && (j + (i * 16) < length); j++) {
            written += sprintf(buf + written,
                    "%02X ", ((char *)x)[j + (i * 16)] & 0x00FF);
        }
        written += sprintf(buf + written, "\n");
        fprintf(stderr, "%s", buf);
        if(j < 16 && (j + i * 16 >= length)) {
            break;
        }
    }
    fflush(stderr);
}

void USB::describeTransfer(const char *label, int length, void *data, int endpoint, bool hexdump) {
    /* FIXME: put in a system-independent timestamp here with usec resolution */
    fprintf(stderr, "%s Transferring %d bytes via endpoint 0x%02X:",
            label, length, endpoint);
    if (hexdump)
    {
        for (int i = 0; i < length; i++)
        {
            if (i % 16 == 0)
                fprintf(stderr, "\n%s    %04x:", label, i);
            fprintf(stderr, " %02x", ((unsigned char*)data)[i]);
        }
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}
