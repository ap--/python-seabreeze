/***************************************************//**
 * @file    SeaBreezeAPI.cpp
 * @date    January 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper around the SeaBreeze driver.
 * Both C and C++ language interfaces are provided.  Please
 * note that this wrapper should try very hard to recover
 * from errors -- like the user trying to read data before
 * opening the device -- and set an appropriate error code.
 * Even if a method here is called grossly out of order,
 * it should not be possible to crash anything.  The only
 * case where it may be reasonable to crash is when trying
 * to fill in a buffer that the user has not properly
 * allocated first.  All other cases should recover.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2017, Ocean Optics Inc
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
#include "api/seabreezeapi/SeaBreezeAPI.h"
#include "api/seabreezeapi/SeaBreezeAPI_Impl.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/DeviceFactory.h"

#include <ctype.h>
#include <vector>
#include <string.h>
#include <stdio.h>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

SeaBreezeAPI *SeaBreezeAPI::instance = NULL;

SeaBreezeAPI *SeaBreezeAPI::getInstance() {
    if(NULL == instance) {
        instance = new SeaBreezeAPI_Impl();
    }
    return instance;
}

void SeaBreezeAPI::shutdown() {
    if(NULL != instance) {
        delete instance;
        instance = NULL;
    }
    DeviceFactory::shutdown();
}

SeaBreezeAPI::SeaBreezeAPI() {

}

SeaBreezeAPI::~SeaBreezeAPI() {

}
