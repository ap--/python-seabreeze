/***************************************************//**
 * @file    NativeSystemPOSIX.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * The NativeSystemPOSIX class provides an implementation
 * of non-portable system calls for POSIX.  This should
 * work for at least Linux, OSX, and any other UNIX-like
 * operating system.
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

/* Definitions */
#define _POSIX_C_SOURCE 199309  /* Needed for Linux to define POSIX level */

#include "common/globals.h"
#include <time.h>               /* For definition of nanosleep() */
#include "native/system/NativeSystem.h"

/* Function definitions */

void sleepMilliseconds(unsigned int msecs) {
    struct timespec ts;

    /* The preferred delay function in modern POSIX is nanosleep().  This
     * replaces the old sleep() (1-second resolution) and usleep() (microsecond
     * resolution) with something that can report time left in case the
     * delay was interrupted.
     *
     * If p-threads or some other mechanism is ever added to SeaBreeze then
     * this may need to be replaced with something more thread-aware.
     */
    ts.tv_sec = msecs/1000;                 /* Whole seconds portion */
    ts.tv_nsec = (msecs % 1000) * 1000000;  /* Fraction in nanoseconds */

    /* Sleep for the given period.  Note that this discards any information
     * about whether the sleep was interrupted; this is simply best-effort.
     */
    nanosleep(&ts, NULL);
}

int systemInitialize(void) {
    /* There are no system-wide services that need to be warmed up. */
    return 0;
}

void systemShutdown(void) {
    /* There are no system-wide services to shut down. */
}
