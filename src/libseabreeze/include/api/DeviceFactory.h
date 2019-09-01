/***************************************************//**
 * @file    DeviceFactory.h
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * Notes:
 *
 * This class allows Device class instances to be created
 * using just the name of the class as a string.
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
 */

#ifndef SEABREEZE_DEVICE_FACTORY_H
#define SEABREEZE_DEVICE_FACTORY_H

#include <string>
#include <map>
#include <vector>
#include <string>
#include "common/devices/Device.h"

namespace seabreeze {

    class DeviceFactory {

    public:
        static DeviceFactory* getInstance();
        static DeviceFactory* instance;
        static void shutdown();

        Device *create(const std::string& name);
        Device *create(int index);
        int getNumberOfDeviceTypes();

        std::vector<std::string> getSupportedModels();

    private:
        typedef Device *(*creatorFunction)(void);
        DeviceFactory();
        std::map<std::string, creatorFunction> nameToCreator;
    };

}

#endif
