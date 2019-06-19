/***************************************************//**
 * @file    System.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * The System class provides an abstract interface to
 * certain system calls.
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
#include "native/system/System.h"
#include "native/system/NativeSystem.h"

using namespace seabreeze;

/* Function definitions */
System::System() {

}

System::~System() {

}

void System::sleepMilliseconds(unsigned int millis) {
    /* This delegates to the native C function in the default namespace.
     * The :: prefix is required to prevent this from simply calling
     * the System::sleepMilliseconds() function recursively.
     */
    ::sleepMilliseconds(millis);
}

bool System::initialize() {
    /* Delegate to the native C startup function. */
    int result = ::systemInitialize();
    if(0 == result) {
        return true;
    }
    
    return false;
}

void System::shutdown() {
    /* Delegate to the native C shutdown function */
    ::systemShutdown();
}
