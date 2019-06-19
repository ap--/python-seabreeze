/***************************************************//**
 * @file    NativeSystemWindows.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * The NativeSystemPOSIX class provides an implementation
 * of non-portable system calls for the Windows API.
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
#include <winsock2.h>              /* Must include winsock2.h before windows.h */
#include <windows.h>               /* For definition of Sleep() */
#include "native/system/NativeSystem.h"

/* Function definitions */

void sleepMilliseconds(unsigned int msecs) {
    /* Simply delegate to the Windows API Sleep() function.  This delays for
     * the given number of milliseconds.
     */
    Sleep(msecs);
}

int systemInitialize() {
    /* Need to start up WinSock to be able to use network functionality. */
    WSADATA wsaData;
    WORD wVersionRequested;
    int err;
    
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if(0 != err) {
        /* Unable to find suitable WinSock implementation. */
        return -1;
    }
    
    return 0;
}

void systemShutdown() {
    /* Need to tell WinSock to shut down cleanly. */
    WSACleanup();
}
