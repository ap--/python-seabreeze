/***************************************************//**
 * @file    DeviceFactory.cpp
 * @date    February 2012
 * @author  Ocean Optics, Inc.
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

#include "common/globals.h"
#include <string.h>
#include <string>
#include <vector>

#include "api/DeviceFactory.h"

#include "vendors/OceanOptics/devices/Apex.h"
#include "vendors/OceanOptics/devices/FlameX.h"
#include "vendors/OceanOptics/devices/FlameNIR.h"
#include "vendors/OceanOptics/devices/HR2000.h"
#include "vendors/OceanOptics/devices/HR2000Plus.h"
#include "vendors/OceanOptics/devices/HR4000.h"
#include "vendors/OceanOptics/devices/Jaz.h"
#include "vendors/OceanOptics/devices/Maya2000.h"
#include "vendors/OceanOptics/devices/Maya2000Pro.h"
#include "vendors/OceanOptics/devices/MayaLSL.h"
#include "vendors/OceanOptics/devices/NIRQuest256.h"
#include "vendors/OceanOptics/devices/NIRQuest512.h"
#include "vendors/OceanOptics/devices/QE65000.h"
#include "vendors/OceanOptics/devices/QEPro.h"
#include "vendors/OceanOptics/devices/Spark.h"
#include "vendors/OceanOptics/devices/STS.h"
#include "vendors/OceanOptics/devices/Torus.h"
#include "vendors/OceanOptics/devices/USB2000.h"
#include "vendors/OceanOptics/devices/USB2000Plus.h"
#include "vendors/OceanOptics/devices/USB4000.h"
#include "vendors/OceanOptics/devices/Ventana.h"

using namespace seabreeze;
using namespace std;

seabreeze::DeviceFactory* seabreeze::DeviceFactory::instance = NULL;

/* This is a generic factory that can create a Device of the templated type. */
template<typename T> Device *deviceFactory() {
   /* Create a new instance of the given Device subtype */
   T* t = new T();
   return t;
}

DeviceFactory* DeviceFactory::getInstance()
{
    if (NULL == instance)
    {
        instance = new DeviceFactory();
    }
    return instance;
}

void DeviceFactory::shutdown() {
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
}

DeviceFactory::DeviceFactory()
{
    nameToCreator.insert(make_pair("Apex",        (creatorFunction) &deviceFactory<Apex       >));
    nameToCreator.insert(make_pair("FlameX",      (creatorFunction) &deviceFactory<FlameX     >));
    nameToCreator.insert(make_pair("FlameNIR",    (creatorFunction) &deviceFactory<FlameNIR   >));
    nameToCreator.insert(make_pair("HR2000",      (creatorFunction) &deviceFactory<HR2000     >));
    nameToCreator.insert(make_pair("HR2000Plus",  (creatorFunction) &deviceFactory<HR2000Plus >));
    nameToCreator.insert(make_pair("HR4000",      (creatorFunction) &deviceFactory<HR4000     >));
    nameToCreator.insert(make_pair("Jaz",         (creatorFunction) &deviceFactory<Jaz        >));
    nameToCreator.insert(make_pair("Maya2000",    (creatorFunction) &deviceFactory<Maya2000   >));
    nameToCreator.insert(make_pair("Maya2000Pro", (creatorFunction) &deviceFactory<Maya2000Pro>));
    nameToCreator.insert(make_pair("MayaLSL",     (creatorFunction) &deviceFactory<MayaLSL    >));
    nameToCreator.insert(make_pair("NIRQuest256", (creatorFunction) &deviceFactory<NIRQuest256>));
    nameToCreator.insert(make_pair("NIRQuest512", (creatorFunction) &deviceFactory<NIRQuest512>));
    nameToCreator.insert(make_pair("QE65000",     (creatorFunction) &deviceFactory<QE65000    >));
    nameToCreator.insert(make_pair("QE-PRO",      (creatorFunction) &deviceFactory<QEPro      >));
    nameToCreator.insert(make_pair("Spark",       (creatorFunction) &deviceFactory<Spark      >));
    nameToCreator.insert(make_pair("STS",         (creatorFunction) &deviceFactory<STS        >));
    nameToCreator.insert(make_pair("Torus",       (creatorFunction) &deviceFactory<Torus      >));
    nameToCreator.insert(make_pair("USB2000",     (creatorFunction) &deviceFactory<USB2000    >));
    nameToCreator.insert(make_pair("USB2000Plus", (creatorFunction) &deviceFactory<USB2000Plus>));
    nameToCreator.insert(make_pair("USB4000",     (creatorFunction) &deviceFactory<USB4000    >));
    nameToCreator.insert(make_pair("Ventana",     (creatorFunction) &deviceFactory<Ventana    >));
}

Device *DeviceFactory::create(const string& className) {
    Device *retval = NULL;

    if (nameToCreator.count(className) > 0) {
        retval = (*(nameToCreator[className]))();
    }

    return retval;
}

Device *DeviceFactory::create(int index) {
    Device *retval = NULL;
    unsigned count = 0;
    for (map<string, creatorFunction>::iterator i = nameToCreator.begin(); i != nameToCreator.end(); i++)
    {
        if ((int) count == index)
        {
            retval = (*(i->second))();
            break;
        }
        count++;
    }

    return retval;
}

int DeviceFactory::getNumberOfDeviceTypes() {
    return (int) nameToCreator.size();
}

std::vector<std::string> DeviceFactory::getSupportedModels() {
    std::vector<std::string> supportedModels;
    for (map<string, creatorFunction>::iterator i = nameToCreator.begin(); i != nameToCreator.end(); i++)
    {
        supportedModels.push_back(std::string(i->first));
    }
    return supportedModels;
}
