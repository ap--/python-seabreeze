/***************************************************//**
 * @file    NativeRS232Windows.c
 * @date    April 21, 2011
 * @author  Aaron Gage
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
#include "native/rs232/NativeRS232.h"
#include "native/rs232/windows/NativeRS232Windows.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h" // for SET_ERROR_CODE

/* Macro and constant definitions */
#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

#define RX_BUFFER_SIZE 1024
#define TX_BUFFER_SIZE 1024

/* Local structs */
struct __ooi_rs232_driver_info {
    HANDLE dev;
    BOOL opened;
    DCB current;
    DCB original;
};

void *RS232Open(char *device, int *errorCode) {
    HANDLE dev;
    char portString[50];
    WCHAR wideString[50];

    struct __ooi_rs232_driver_info *devInfo;

    devInfo = (struct __ooi_rs232_driver_info *)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, sizeof(struct __ooi_rs232_driver_info));

    if(NULL == devInfo) {
        /* Unable to allocate memory, probably not a good sign */
        SET_ERROR_CODE(NO_DEVICE_FOUND);
        return NULL;
    }

    _snprintf(portString,49,"\\\\.\\%s", device);
    /* Convert to a wide string for use with CreateFile */
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, portString, (int) strlen(portString) + 1, wideString, 50);
#ifdef __MINGW32__
    dev = CreateFile((LPCSTR)wideString, (GENERIC_READ | GENERIC_WRITE), 0, NULL,
            OPEN_EXISTING, 0, NULL);
#else
    dev = CreateFile(wideString, (GENERIC_READ | GENERIC_WRITE), 0, NULL,
            OPEN_EXISTING, 0, NULL);
#endif
    if(dev == INVALID_HANDLE_VALUE) {
        /* Failed to open the device. */
        SET_ERROR_CODE(NO_DEVICE_FOUND);
        return NULL;
    }

    SetupComm(dev, RX_BUFFER_SIZE, TX_BUFFER_SIZE);
    GetCommState(dev, &(devInfo->current));
    GetCommState(dev, &(devInfo->original));

    /* Set up some default serial parameters */
    devInfo->current.ByteSize = 8;  /* 8 bits word length */
    devInfo->current.Parity = NOPARITY;    /* No parity */
    devInfo->current.StopBits = ONESTOPBIT;  /* 1 stop bit */

    SetCommState(devInfo->dev, &(devInfo->current));

    devInfo->dev = dev;
    devInfo->opened = TRUE;

    return (void *)devInfo;
}

int RS232Close(void *handle) {
    struct __ooi_rs232_driver_info *devInfo;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Device must be open before it is allowed to be closed. */
        return -2;
    }

    SetCommState(devInfo->dev, &(devInfo->original));
    CloseHandle(devInfo->dev);

    devInfo->opened = FALSE;
    devInfo->dev = NULL;

    /* FIXME: there should be a mechanism somewhere to free
     * the memory created by RS232Open, but I am not sure that
     * this is the best place.
     */
    return 0;  /* success */
}

int RS232Write(void *handle, char *data, int numberOfBytes) {
    struct __ooi_rs232_driver_info *devInfo;
    int offset = 0;
    int bytesToWrite;
    DWORD lastWrite = 0;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Cannot read from a device unless it is open */
        return -2;
    }

    while(offset < numberOfBytes) {
        bytesToWrite = MIN(numberOfBytes - offset, TX_BUFFER_SIZE);
        WriteFile(devInfo->dev, &(data[offset]), bytesToWrite, &lastWrite, NULL);
        offset += lastWrite;
        if(0 == lastWrite) {
            /* This has written as much as is possible for now.  The caller can
             * try again if this happens or go off and do something else while
             * the bytes get transmitted.
             */
            break;
        }
    }

    return offset;
}

int RS232Read(void *handle, char *buffer, int numberOfBytes) {
    struct __ooi_rs232_driver_info *devInfo;
    int offset = 0;
    int bytesToRead;
    DWORD lastRead = 0;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Cannot read from a device unless it is open */
        return -2;
    }

    while (offset < numberOfBytes) {
        bytesToRead = MIN(numberOfBytes - offset, RX_BUFFER_SIZE);
        ReadFile(devInfo->dev, &(buffer[offset]), bytesToRead, &lastRead, NULL);
        offset += lastRead;
        if(0 == lastRead) {
            /* All bytes on the serial port have been received.  Stop reading
             * here.  The caller can always retry if they were expecting more.
             */
            break;
        }
    }

    return offset;
}

int RS232SetBaudRate(void *handle, int rate) {
    struct __ooi_rs232_driver_info *devInfo;
    int retval;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Cannot read from a device unless it is open */
        return -2;
    }

    devInfo->current.BaudRate = rate;
    retval = SetCommState(devInfo->dev, &(devInfo->current));

    if(0 != retval) {
        /* Success */
        return rate;
    }

    return retval;
}

int RS232ClearInputBuffer(void *handle) {
    struct __ooi_rs232_driver_info *devInfo;
    int retval;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Cannot read from a device unless it is open */
        return -2;
    }

    retval = PurgeComm(devInfo->dev, PURGE_RXCLEAR);

    return retval;
}

int RS232ClearOutputBuffer(void *handle) {
    struct __ooi_rs232_driver_info *devInfo;
    int retval;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Cannot read from a device unless it is open */
        return -2;
    }

    retval = PurgeComm(devInfo->dev, PURGE_TXCLEAR);

    return retval;
}

int RS232WaitForWrite(void *handle) {
    struct __ooi_rs232_driver_info *devInfo;
    int retval;

    if(NULL == handle) {
        /* Invalid state */
        return -1;
    }

    devInfo = (struct __ooi_rs232_driver_info *)handle;

    if(FALSE == devInfo->opened) {
        /* Cannot read from a device unless it is open */
        return -2;
    }

    retval = FlushFileBuffers(devInfo->dev);

    return retval;
}
