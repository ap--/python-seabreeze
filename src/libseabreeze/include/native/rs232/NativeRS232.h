/***************************************************//**
 * @file    NativeRS232.h
 * @date    April 21, 2011
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
 ******************************************************/

#ifndef NATIVERS232_H
#define NATIVERS232_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OPEN_OK 0
#define NO_DEVICE_FOUND -1

void *RS232Open(char *device, int *errorCode);

int RS232Close(void *handle);

int RS232Write(void *handle, char *data, int numberOfBytes);

int RS232Read(void *handle, char *buffer, int numberOfBytes);

int RS232SetBaudRate(void *handle, int rate);

int RS232ClearInputBuffer(void *handle);

int RS232ClearOutputBuffer(void *handle);

int RS232WaitForWrite(void *handle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NATIVERS232_H */
