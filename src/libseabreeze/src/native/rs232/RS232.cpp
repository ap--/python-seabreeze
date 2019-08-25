/***************************************************//**
 * @file    RS232.cpp
 * @date    April 2011
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
#include "native/rs232/RS232.h"
#include "native/rs232/NativeRS232.h"
#include "common/exceptions/IllegalArgumentException.h"
#include <stdlib.h>
#include <stdio.h>  /* For debugging, feel free to replace with iostream */
#include <string.h> /* for memset() */
#include <string>

using namespace seabreeze;
using namespace std;

RS232::RS232(const char *path, int baud) {
    int length;

    this->verbose = false;
    this->opened = false;
    this->descriptor = NULL;
    this->baudRate = baud;

    if(NULL == path) {
        string cause("Device path cannot be NULL.");
        throw IllegalArgumentException(cause);
    }

    length = (int) strlen(path);
    this->devicePath = (char *)calloc(length + 1, sizeof(unsigned char));
    memcpy(this->devicePath, path, length);
}

RS232::~RS232() {

    /* Try to close the device descriptor if it is not already. */
    if(true == this->opened && this->descriptor != NULL) {
        this->close();
    }
}

bool RS232::open() {
    int error = 0;
    int actualRate = 0;

    if(true == this->opened || this->descriptor != NULL) {
        this->close();
    }

    this->descriptor = RS232Open(this->devicePath, &error);

    if(0 != error) {
        fprintf(stderr, "Warning: failed to open device %s, error=%d\n",
            this->devicePath, error);
        this->descriptor = NULL;
        /* FIXME: this should probably throw a specific exception instead. */
        return false;
    }

    /* Flush the input and output buffers to make sure everything is clean. */
    RS232ClearInputBuffer(this->descriptor);
    RS232ClearOutputBuffer(this->descriptor);

    actualRate = RS232SetBaudRate(this->descriptor, this->baudRate);
    if(actualRate != this->baudRate) {
        if(true == this->verbose) {
            fprintf(stderr, "Failed to set desired baud rate (%d).  Closing port.\n",
                    baudRate);
        }
        RS232Close(this->descriptor);
        this->descriptor = NULL;
        return false;
    }

    this->opened = true;
    if(true == this->verbose) {
        fprintf(stderr, "Opened device %s\n", this->devicePath);
    }

    return true;
}

bool RS232::close() {

    int flag = 0;
    bool retval = false;

    if(NULL != this->descriptor) {
        flag = RS232Close(this->descriptor);
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

int RS232::write(void *data, unsigned int length_bytes) {

    int flag = 0;

    if(true == this->verbose) {
        this->describeTransfer(length_bytes, true);
    }

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready or opened */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to write to a serial device that is not opened.\n");
        }
        return -1;
    }

    flag = RS232Write(this->descriptor, (char *)data, (int)length_bytes);

    if(flag < 0) {
        /* FIXME: throw an exception here */
        if(true == this->verbose) {
            fprintf(stderr, "Warning: got error %d while trying to write %d bytes via RS232\n",
                    flag, length_bytes);
        }
        return -1;
    }

    if(true == this->verbose) {
        this->rs232HexDump(data, length_bytes, true);
    }

    return flag;
}

int RS232::read(void *data, unsigned int length_bytes) {
    int flag = 0;

    if(true == this->verbose) {
        this->describeTransfer(length_bytes, false);
    }

    if(NULL == this->descriptor || false == this->opened) {
        /* FIXME: throw an exception for device not ready or opened */
        if(true == this->verbose) {
            fprintf(stderr, "ERROR: tried to read a serial device that is not opened.\n");
        }
        return -1;
    }

    flag = RS232Read(this->descriptor, (char *)data, (int)length_bytes);

    if(flag < 0) {
        /* FIXME: throw an exception here */
        if(true == this->verbose) {
            fprintf(stderr, "Warning: got error %d while trying to read %d bytes via RS232\n",
                    flag, length_bytes);
        }
        return -1;
    }

    if(true == this->verbose) {
        this->rs232HexDump(data, length_bytes, false);
    }

    return flag;
}


bool RS232::isOpened() {
    return this->opened;
}

/* Debugging methods */
void RS232::rs232HexDump(void *x, int length, bool out) {
    /* FIXME: put in a system-independent timestamp here with usec resolution */
    fprintf(stderr, "RS232 transferred %d bytes %s:\n",
                length, out ? "out" : "in");

    this->hexDump(x, length);
}

void RS232::hexDump(void *x, int length) {
    /* removed because python 2.7 compilation errors on windows and i can't be bothered */
    fflush(stderr);
}

void RS232::describeTransfer(int length, bool out) {
    /* FIXME: put in a system-independent timestamp here with usec resolution */
    fprintf(stderr, "Transferring %d bytes %s\n",
            length, out ? "out" : "in");
    fflush(stderr);
}
