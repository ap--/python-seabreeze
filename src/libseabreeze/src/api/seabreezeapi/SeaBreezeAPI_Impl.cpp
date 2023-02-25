/***************************************************//**
 * @file    SeaBreezeAPI_Impl.cpp
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper around the SeaBreeze driver. Please
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
#include "api/seabreezeapi/SeaBreezeAPI_Impl.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include "api/DeviceFactory.h"  // references device.h
#include "common/buses/network/IPv4SocketDeviceLocator.h"
#include "common/buses/network/IPv4NetworkProtocol.h"
#include "common/buses/rs232/RS232DeviceLocator.h"
#include "common/buses/DeviceLocationProberInterface.h"
#include "native/system/System.h"

#include <ctype.h>
#include <vector>
#include <string.h>
#include <stdio.h>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

static int __deviceID = 1;

SeaBreezeAPI_Impl::SeaBreezeAPI_Impl() {
    System::initialize();
}

SeaBreezeAPI_Impl::~SeaBreezeAPI_Impl() {
    vector<DeviceAdapter *>::iterator dIter;

    for(dIter = this->specifiedDevices.begin(); dIter != this->specifiedDevices.end(); dIter++) {
        delete *dIter;
    }

    for(dIter = this->probedDevices.begin(); dIter != this->probedDevices.end(); dIter++) {
        delete *dIter;
    }

    System::shutdown();
}

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif
int SeaBreezeAPI_Impl::probeDevices() {
    /* This function is a little ugly because it tries to find hardware
     * associated with every device type, but without allowing multiple
     * Device instances to be created for a particular Bus location.
     * This requires a bit of searching which translates to nested loops.
     * Fortunately, none of these loops has to iterate very long.
     */
    Device *dev;
    vector<DeviceAdapter *>::iterator devIter;
    vector<DeviceAdapter *>::iterator validIter;
    int i;
    vector<DeviceAdapter *> validDevices;

    DeviceFactory* deviceFactory = DeviceFactory::getInstance();

    for(i = 0; i < deviceFactory->getNumberOfDeviceTypes(); i++) {
        /* Try to create a device by its type index.  This does not require
         * knowing what type of device is actually being created.  This instance
         * will be an exemplar and will not actually be tied to real hardware.
         */
        dev = deviceFactory->create(i);

        vector<Bus *>::iterator iter;
        vector<Bus *> buses = dev->getBuses();

        for(iter = buses.begin(); iter != buses.end(); iter++) {
            DeviceLocationProberInterface *prober = dynamic_cast<DeviceLocationProberInterface *>(*iter);
            if(NULL != prober) {
                /* Found a type of Bus that can probe for
                 * real hardware to associate with this Device
                 */
                vector<DeviceLocatorInterface *> *locations;
                locations = prober->probeDevices();
                vector<DeviceLocatorInterface *>::iterator locIter;
                for(    locIter = locations->begin();
                        locIter != locations->end();
                        locIter++) {
                    /* For each device location, check whether it is already
                     * known.  If not, add it.  If so, skip over it.
                     */
                    bool locationKnown = false;
                    for(    devIter = this->probedDevices.begin();
                            devIter != this->probedDevices.end();
                            devIter++) {
                        /* For each known device, compare to the newly probed
                         * location and see if they match.
                         */
                        DeviceLocatorInterface *knownLoc = (*devIter)->getLocation();
                        if(true == (*locIter)->equals(*knownLoc)) {
                            /* This device location is already tracked. */
                            locationKnown = true;
                            /* Note that it has just been seen */
                            validDevices.push_back(*devIter);
                            break;
                        }
                    }
                    if(false == locationKnown) {
                        /* The location is not already known.  Create a new
                         * instance of the type of device in question and
                         * assign the new instance to this location.  This also
                         * effectively marks the new instance as being valid.
                         */
                        Device *newdev = deviceFactory->create(i);
                        newdev->setLocation(**locIter);
                        /* Note that this pre-increments the device ID to
                         * mitigate any race conditions
                         */
                        try {
                            DeviceAdapter *da = new DeviceAdapter(newdev, ++__deviceID);
                            this->probedDevices.push_back(da);
                            validDevices.push_back(da);
                        } catch (const IllegalArgumentException &iae) {
                            continue;
                        }
                    }
                }
                for(vector<DeviceLocatorInterface *>::iterator iter = locations->begin();
                        iter != locations->end(); iter++) {
                    delete *iter;
                }
                locations->clear();
                delete locations;
            }
        }

        delete dev;
    }

    /* Now go through the set of all probed devices and ensure that each of
     * them is still around.
     */
    for(    devIter = this->probedDevices.begin();
            devIter != this->probedDevices.end();
            ) {
        bool verified = false;
        DeviceLocatorInterface *knownLoc = (*devIter)->getLocation();
        for(    validIter = validDevices.begin();
                validIter != validDevices.end();
                validIter++) {
            DeviceLocatorInterface *validLoc = (*validIter)->getLocation();
            if(knownLoc->equals(*validLoc)) {
                verified = true;
                break;
            }
        }
        if(false == verified) {
            /* The device has disappeared since it was first probed.  Get rid
             * of the instance that was tracking it.
             */
            delete *devIter;
            devIter = this->probedDevices.erase(devIter);
        } else {
            devIter++;
        }
    }

    return (int) probedDevices.size();
}

int SeaBreezeAPI_Impl::addTCPIPv4DeviceLocation(char *deviceTypeName, char *ipAddr,
        int port) {
    string address(ipAddr);

    Device *dev = DeviceFactory::getInstance()->create(deviceTypeName);
    if(NULL == dev) {
        /* Failed to identify that type of device. */
        return 1;
    }

    IPv4NetworkProtocols protocols;

    IPv4SocketDeviceLocator locator(protocols.TCP_IP4, address, port);
    dev->setLocation(locator);

    try {
        /* Note that this pre-increments the device ID to mitigate any race conditions */
        this->specifiedDevices.push_back(new DeviceAdapter(dev, ++__deviceID));
    } catch (const IllegalArgumentException &iae) {
        /* Unable to create the adapter */
        return 2;
    }

    return 0;
}

int SeaBreezeAPI_Impl::addRS232DeviceLocation(char *deviceTypeName,
        char *deviceBusPath, unsigned int baud) {
    string path(deviceBusPath);

    Device *dev = DeviceFactory::getInstance()->create(deviceTypeName);
    if(NULL == dev) {
        /* Failed to identify that type of device. */
        return 1;
    }

    RS232DeviceLocator locator(path, baud);
    dev->setLocation(locator);

    try {
        /* Note that this pre-increments the device ID to mitigate any race conditions */
        this->specifiedDevices.push_back(new DeviceAdapter(dev, ++__deviceID));
    } catch (const IllegalArgumentException &iae) {
        /* Unable to create the adapter */
        return 2;
    }

    return 0;
}


int SeaBreezeAPI_Impl::getNumberOfDeviceIDs() {
    return (int) (this->specifiedDevices.size() + this->probedDevices.size());
}


int SeaBreezeAPI_Impl::getDeviceIDs(long *ids, unsigned long maxLength) {

    vector<DeviceAdapter *>::iterator iter;
    unsigned int i = 0;

    for(    iter = specifiedDevices.begin();
            iter != specifiedDevices.end() && i < maxLength;
            iter++, i++) {
        ids[i] = (*iter)->getID();
    }

    /* Note: this does not reset i and this is deliberate. */

    for(    iter = probedDevices.begin();
            iter != probedDevices.end() && i < maxLength;
            iter++, i++) {
        ids[i] = (*iter)->getID();
    }

    return i;
}

DeviceAdapter *SeaBreezeAPI_Impl::getDeviceByID(unsigned long id) {
    vector<DeviceAdapter *>::iterator iter;

    /* This gives priority to specified devices since they require more specific
     * information to set up.
     */
    for(iter = specifiedDevices.begin(); iter != specifiedDevices.end(); iter++) {
        if((*iter)->getID() == id) {
            return *iter;
        }
    }

    for(iter = probedDevices.begin(); iter != probedDevices.end(); iter++) {
        if((*iter)->getID() == id) {
            return *iter;
        }
    }

    return NULL;
}


int SeaBreezeAPI_Impl::getNumberOfSupportedModels() {
    std::vector<std::string> supportedModels = DeviceFactory::getInstance()->getSupportedModels();
    return (int) (supportedModels.size());
}


int SeaBreezeAPI_Impl::getSupportedModelName(int index, int *errorCode, char *buffer, int bufferLength) {
    std::vector<std::string> supportedModels = DeviceFactory::getInstance()->getSupportedModels();

    if (index < 0) {
        SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
        return -1;
    }
    if (index >= (int) supportedModels.size()) {
        SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
        return -1;
    }

    std::string & model = supportedModels.at(index);
    size_t n = model.copy( buffer, bufferLength - 1 );
    buffer[n] = '\0';

    SET_ERROR_CODE(ERROR_SUCCESS);
    return (int) n;
}


/**************************************************************************************/
//  Device Control  for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::openDevice(long id, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(id);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return -1;
    }

    return adapter->open(errorCode);
}

void SeaBreezeAPI_Impl::closeDevice(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->close();
    SET_ERROR_CODE(ERROR_SUCCESS);
}

int SeaBreezeAPI_Impl::getDeviceType(long id, int *errorCode,
            char *buffer, unsigned int length) {
    DeviceAdapter *adapter = getDeviceByID(id);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->getDeviceType(errorCode, buffer, length);
}


/**************************************************************************************/
//  USB endpoints are tied to the device, but facilitate raw usb access
/**************************************************************************************/

unsigned char SeaBreezeAPI_Impl::getDeviceEndpoint(long id, int *errorCode, usbEndpointType endpoint)
{
    DeviceAdapter *adapter = getDeviceByID(id);
    if(NULL == adapter)
    {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->getDeviceEndpoint(errorCode, endpoint);
}

/**************************************************************************************/
//  raw USB access Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfRawUSBBusAccessFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfRawUSBBusAccessFeatures();
}

int SeaBreezeAPI_Impl::getRawUSBBusAccessFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getRawUSBBusAccessFeatures(buffer, maxLength);
}

int SeaBreezeAPI_Impl::rawUSBBusAccessRead(long deviceID, long featureID,
        int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->rawUSBBusAccessRead(featureID, errorCode, buffer, bufferLength, endpoint);
}

int SeaBreezeAPI_Impl::rawUSBBusAccessWrite(long deviceID, long featureID,
        int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->rawUSBBusAccessWrite(featureID, errorCode, buffer, bufferLength, endpoint);
}




/**************************************************************************************/
//  Serial Number Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfSerialNumberFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfSerialNumberFeatures();
}

int SeaBreezeAPI_Impl::getSerialNumberFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getSerialNumberFeatures(buffer, maxLength);
}

int SeaBreezeAPI_Impl::getSerialNumber(long deviceID, long featureID, int *errorCode,
            char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->getSerialNumber(featureID, errorCode, buffer, bufferLength);
}

unsigned char SeaBreezeAPI_Impl::getSerialNumberMaximumLength(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->getSerialNumberMaximumLength(featureID, errorCode);
}

/**************************************************************************************/
//  Spectrometer Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfSpectrometerFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfSpectrometerFeatures();
}

int SeaBreezeAPI_Impl::getSpectrometerFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getSpectrometerFeatures(buffer, maxLength);
}



void SeaBreezeAPI_Impl::spectrometerSetTriggerMode(long deviceID, long featureID,
            int *errorCode, int mode) {

    SpectrometerTriggerMode triggerMode(mode);

    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->spectrometerSetTriggerMode(featureID, errorCode, mode);
}


void SeaBreezeAPI_Impl::spectrometerSetIntegrationTimeMicros(long deviceID,
        long featureID, int *errorCode,
        unsigned long integrationTimeMicros) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->spectrometerSetIntegrationTimeMicros(featureID,
        errorCode, integrationTimeMicros);
}

unsigned long SeaBreezeAPI_Impl::spectrometerGetMinimumIntegrationTimeMicros(
        long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetMinimumIntegrationTimeMicros(featureID, errorCode);
}

unsigned long SeaBreezeAPI_Impl::spectrometerGetMaximumIntegrationTimeMicros(
        long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetMaximumIntegrationTimeMicros(featureID, errorCode);
}

double SeaBreezeAPI_Impl::spectrometerGetMaximumIntensity(
        long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetMaximumIntensity(featureID, errorCode);
}

int SeaBreezeAPI_Impl::spectrometerGetFastBufferSpectrum(long deviceID,
	long featureID, int *errorCode, unsigned char *buffer, int bufferLength, unsigned int numberOfSamplesToRetrieve) {
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->spectrometerGetFastBufferSpectrum(featureID, errorCode,
		buffer, bufferLength, numberOfSamplesToRetrieve);
}

int SeaBreezeAPI_Impl::spectrometerGetUnformattedSpectrum(long deviceID,
        long featureID, int *errorCode, unsigned char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetUnformattedSpectrum(featureID, errorCode,
            buffer, bufferLength);
}

int SeaBreezeAPI_Impl::spectrometerGetFormattedSpectrum(long deviceID,
        long featureID, int *errorCode, double *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetFormattedSpectrum(featureID, errorCode,
            buffer, bufferLength);
}

int SeaBreezeAPI_Impl::spectrometerGetUnformattedSpectrumLength(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetUnformattedSpectrumLength(featureID, errorCode);
}

int SeaBreezeAPI_Impl::spectrometerGetFormattedSpectrumLength(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetFormattedSpectrumLength(featureID, errorCode);
}

int SeaBreezeAPI_Impl::spectrometerGetWavelengths(long deviceID,
        long featureID, int *errorCode, double *wavelengths, int length) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetWavelengths(featureID, errorCode,
                wavelengths, length);
}

int SeaBreezeAPI_Impl::spectrometerGetElectricDarkPixelCount(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetElectricDarkPixelCount(featureID, errorCode);
}

int SeaBreezeAPI_Impl::spectrometerGetElectricDarkPixelIndices(long deviceID,
        long featureID, int *errorCode, int *indices, int length) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrometerGetElectricDarkPixelIndices(featureID, errorCode,
                indices, length);
}

/**************************************************************************************/
//  Pixel binning features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfPixelBinningFeatures(long id, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(id);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfPixelBinningFeatures();
}

int SeaBreezeAPI_Impl::getPixelBinningFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getPixelBinningFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::binningSetPixelBinningFactor(long deviceID, long featureID, int *errorCode, const unsigned char binningFactor) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
    }

    adapter->binningSetPixelBinningFactor(featureID, errorCode, binningFactor);
}

unsigned char SeaBreezeAPI_Impl::binningGetPixelBinningFactor(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->binningGetPixelBinningFactor(featureID, errorCode);
}

void SeaBreezeAPI_Impl::binningSetDefaultPixelBinningFactor(long deviceID, long featureID, int *errorCode, const unsigned char binningFactor) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
    }

    adapter->binningSetDefaultPixelBinningFactor(featureID, errorCode, binningFactor);
}

void SeaBreezeAPI_Impl::binningSetDefaultPixelBinningFactor(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
    }

    adapter->binningSetDefaultPixelBinningFactor(featureID, errorCode);
}

unsigned char SeaBreezeAPI_Impl::binningGetDefaultPixelBinningFactor(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->binningGetDefaultPixelBinningFactor(featureID, errorCode);
}

unsigned char SeaBreezeAPI_Impl::binningGetMaxPixelBinningFactor(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->binningGetMaxPixelBinningFactor(featureID, errorCode);
}

/**************************************************************************************/
//  Thermo-electric Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfThermoElectricFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfThermoElectricFeatures();
}

int SeaBreezeAPI_Impl::getThermoElectricFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getThermoElectricFeatures(buffer, maxLength);
}


double SeaBreezeAPI_Impl::tecReadTemperatureDegreesC(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->tecReadTemperatureDegreesC(featureID, errorCode);
}

void SeaBreezeAPI_Impl::tecSetTemperatureSetpointDegreesC(long deviceID, long featureID,
        int *errorCode, double temperatureDegreesCelsius) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->tecSetTemperatureSetpointDegreesC(featureID, errorCode,
            temperatureDegreesCelsius);
}

void SeaBreezeAPI_Impl::tecSetEnable(long deviceID, long featureID, int *errorCode,
        unsigned char tecEnable) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->tecSetEnable(featureID, errorCode, 0 == tecEnable ? false : true);
}


/**************************************************************************************/
//  IrradCal Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfIrradCalFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfIrradCalFeatures();
}

int SeaBreezeAPI_Impl::getIrradCalFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getIrradCalFeatures(buffer, maxLength);
}

int SeaBreezeAPI_Impl::irradCalibrationRead(long deviceID, long featureID,
        int *errorCode, float *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->irradCalibrationRead(featureID, errorCode, buffer, bufferLength);
}

int SeaBreezeAPI_Impl::irradCalibrationWrite(long deviceID, long featureID,
        int *errorCode, float *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->irradCalibrationWrite(featureID, errorCode, buffer, bufferLength);
}

int SeaBreezeAPI_Impl::irradCalibrationHasCollectionArea(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->irradCalibrationHasCollectionArea(featureID, errorCode);
}

float SeaBreezeAPI_Impl::irradCalibrationReadCollectionArea(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->irradCalibrationReadCollectionArea(featureID, errorCode);
}

void SeaBreezeAPI_Impl::irradCalibrationWriteCollectionArea(long deviceID, long featureID,
        int *errorCode, float area) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->irradCalibrationWriteCollectionArea(featureID, errorCode, area);
}


/**************************************************************************************/
//  Ethernet Configuration Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfEthernetConfigurationFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfEthernetConfigurationFeatures();
}

int SeaBreezeAPI_Impl::getEthernetConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getEthernetConfigurationFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::ethernetConfiguration_Get_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->ethernetConfiguration_Get_MAC_Address(featureID, errorCode, interfaceIndex, macAddress);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

}

void SeaBreezeAPI_Impl::ethernetConfiguration_Set_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->ethernetConfiguration_Set_MAC_Address(featureID, errorCode, interfaceIndex, macAddress);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}

unsigned char SeaBreezeAPI_Impl::ethernetConfiguration_Get_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->ethernetConfiguration_Get_GbE_Enable_Status(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::ethernetConfiguration_Set_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->ethernetConfiguration_Set_GbE_Enable_Status(featureID, errorCode, interfaceIndex, enableState);
}







/**************************************************************************************/
//  GPIO Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfGPIOFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfGPIOFeatures();
}

int SeaBreezeAPI_Impl::getGPIOFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getGPIOFeatures(buffer, maxLength);
}


unsigned char SeaBreezeAPI_Impl::getGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioGetNumberOfPins(featureID, errorCode);
}

unsigned int SeaBreezeAPI_Impl::getGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioGetOutputEnableVector(featureID, errorCode);
}


void SeaBreezeAPI_Impl::setGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->gpioSetOutputEnableVector(featureID, errorCode, outputEnableVector, bitMask);
}

unsigned int SeaBreezeAPI_Impl::getGPIO_ValueVector(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioGetValueVector(featureID, errorCode);
}


void SeaBreezeAPI_Impl::setGPIO_ValueVector(long deviceID, long featureID, int *errorCode, unsigned int valueVector, unsigned int bitMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->gpioSetValueVector(featureID, errorCode, valueVector, bitMask);
}


unsigned char SeaBreezeAPI_Impl::getEGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioExtensionGetNumberOfPins(featureID, errorCode);
}


unsigned char SeaBreezeAPI_Impl::getEGPIO_AvailableModes(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maximumModeCount)
{
	unsigned char arraySize = 0;

	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		arraySize = adapter->gpioExtensionGetAvailableModes(featureID, errorCode, pinNumber, availableModes, maximumModeCount);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
	return arraySize;
}


unsigned char SeaBreezeAPI_Impl::getEGPIO_CurrentMode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioExtensionGetCurrentMode(featureID, errorCode, pinNumber);
}

void SeaBreezeAPI_Impl::setEGPIO_Mode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char mode, float value)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->gpioExtensionSetMode(featureID, errorCode, pinNumber, mode, value);
}

unsigned int SeaBreezeAPI_Impl::getEGPIO_OutputVector(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioExtensionGetOutputVector(featureID, errorCode);
}

void SeaBreezeAPI_Impl::setEGPIO_OutputVector(long deviceID, long featureID, int *errorCode, unsigned int outputVector, unsigned int bitMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->gpioExtensionSetOutputVector(featureID, errorCode, outputVector, bitMask);
}

float SeaBreezeAPI_Impl::getEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->gpioExtensionGetValue(featureID, errorCode, pinNumber);
}

void SeaBreezeAPI_Impl::setEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, float value)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->gpioExtensionSetValue(featureID, errorCode, pinNumber, value);
}












/**************************************************************************************/
//  Multicast Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfMulticastFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfMulticastFeatures();
}

int SeaBreezeAPI_Impl::getMulticastFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getMulticastFeatures(buffer, maxLength);
}

#if 0
void SeaBreezeAPI_Impl::getMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(&groupAddress)[4])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->getMulticastGroupAddress(featureID, errorCode, interfaceIndex, groupAddress);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

}

void SeaBreezeAPI_Impl::setMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char groupAddress[4])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->setMulticastGroupAddress(featureID, errorCode, interfaceIndex, groupAddress);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}
#endif

unsigned char SeaBreezeAPI_Impl::getMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->getMulticastEnableState(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::setMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->setMulticastEnableState(featureID, errorCode, interfaceIndex, enableState);
}






/**************************************************************************************/
//  IPv4 Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfIPv4Features(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfIPv4Features();
}

int SeaBreezeAPI_Impl::getIPv4Features(long deviceID, int *errorCode, long *buffer,  int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getIPv4Features(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::get_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->get_IPv4_DHCP_Enable_State(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::set_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->set_IPv4_DHCP_Enable_State(featureID, errorCode, interfaceIndex, enableState);
}

unsigned char SeaBreezeAPI_Impl::get_Number_Of_IPv4_Addresses(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->get_Number_Of_IPv4_Addresses(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::get_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->get_IPv4_Default_Gateway(featureID, errorCode, interfaceIndex, defaultGatewayAddress);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

}

void SeaBreezeAPI_Impl::set_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->set_IPv4_Default_Gateway(featureID, errorCode, interfaceIndex, defaultGatewayAddress);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}

void SeaBreezeAPI_Impl::get_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->get_IPv4_Address(featureID, errorCode, interfaceIndex, addressIndex, IPv4_Address, netMask);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

}

void SeaBreezeAPI_Impl::add_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->add_IPv4_Address(featureID, errorCode, interfaceIndex, IPv4_Address, netMask);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}

void SeaBreezeAPI_Impl::delete_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->delete_IPv4_Address(featureID, errorCode, interfaceIndex, addressIndex);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}



/**************************************************************************************/
//  Wifi Configuration Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfWifiConfigurationFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfWifiConfigurationFeatures();
}

int SeaBreezeAPI_Impl::getWifiConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getWifiConfigurationFeatures(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::getWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->wifiConfigurationGetMode(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::setWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char mode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->wifiConfigurationSetMode(featureID, errorCode, interfaceIndex, mode);
}

unsigned char SeaBreezeAPI_Impl::getWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->wifiConfigurationGetSecurityType(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::setWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char securityType)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->wifiConfigurationSetSecurityType(featureID, errorCode, interfaceIndex, securityType);
}


unsigned char SeaBreezeAPI_Impl::getWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*ssid)[32])
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		return adapter->wifiConfigurationGetSSID(featureID, errorCode, interfaceIndex, ssid);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

}

void SeaBreezeAPI_Impl::setWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->wifiConfigurationSetSSID(featureID, errorCode, interfaceIndex, ssid, length);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}

void SeaBreezeAPI_Impl::setWifiConfigurationPassPhrase(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->wifiConfigurationSetPassPhrase(featureID, errorCode, interfaceIndex, passPhrase, passPhraseLength);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}


/**************************************************************************************/
//  DHCP Server Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfDHCPServerFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfDHCPServerFeatures();
}

int SeaBreezeAPI_Impl::getDHCPServerFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getDHCPServerFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::dhcpServerGetAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->dhcpServerGetAddress(featureID, errorCode, interfaceIndex, serverAddress, netMask);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

}

void SeaBreezeAPI_Impl::dhcpServerSetAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		adapter->dhcpServerSetAddress(featureID, errorCode, interfaceIndex, serverAddress, netMask);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
}

unsigned char SeaBreezeAPI_Impl::dhcpServerGetEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->dhcpServerGetEnableState(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::dhcpServerSetEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->dhcpServerSetEnableState(featureID, errorCode, interfaceIndex, enableState);
}






















/**************************************************************************************/
//  Network Configuration Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfNetworkConfigurationFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfNetworkConfigurationFeatures();
}

int SeaBreezeAPI_Impl::getNetworkConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNetworkConfigurationFeatures(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::getNumberOfNetworkInterfaces(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->getNumberOfNetworkInterfaces(featureID, errorCode);
}

unsigned char SeaBreezeAPI_Impl::getNetworkInterfaceConnectionType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->getNetworkInterfaceConnectionType(featureID, errorCode, interfaceIndex);
}

unsigned char SeaBreezeAPI_Impl::runNetworkInterfaceSelfTest(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->runNetworkInterfaceSelfTest(featureID, errorCode, interfaceIndex);
}

unsigned char SeaBreezeAPI_Impl::getNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->getNetworkInterfaceEnableState(featureID, errorCode, interfaceIndex);
}

void SeaBreezeAPI_Impl::setNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->setNetworkInterfaceEnableState(featureID, errorCode, interfaceIndex, enableState);
}

void SeaBreezeAPI_Impl::saveNetworkInterfaceConnectionSettings(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->saveNetworkInterfaceConnectionSettings(featureID, errorCode, interfaceIndex);
}


/**************************************************************************************/
//  EEProm Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfEEPROMFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfEEPROMFeatures();
}

int SeaBreezeAPI_Impl::getEEPROMFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getEEPROMFeatures(buffer, maxLength);
}


int SeaBreezeAPI_Impl::eepromReadSlot(long deviceID, long featureID, int *errorCode,
        int slotNumber, unsigned char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->eepromReadSlot(featureID, errorCode, slotNumber,
            buffer, bufferLength);
}



/**************************************************************************************/
//  Lamp Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfLampFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfStrobeLampFeatures();
}

int SeaBreezeAPI_Impl::getLampFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getStrobeLampFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::lampSetLampEnable(long deviceID, long featureID,
        int *errorCode, bool strobeEnable) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->lampSetStrobeEnable(featureID, errorCode, strobeEnable);
}


/**************************************************************************************/
//  Shutter Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfShutterFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfShutterFeatures();
}

int SeaBreezeAPI_Impl::getShutterFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getShutterFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::shutterSetShutterOpen(long deviceID, long featureID,
        int *errorCode, bool opened) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->shutterSetShutterOpen(featureID, errorCode, opened);
}

/**************************************************************************************/
//  Light Source Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfLightSourceFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfLightSourceFeatures();
}

int SeaBreezeAPI_Impl::getLightSourceFeatures(long deviceID, int *errorCode,
            long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getLightSourceFeatures(buffer, maxLength);
}

int SeaBreezeAPI_Impl::lightSourceGetCount(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->lightSourceGetCount(featureID, errorCode);
}

bool SeaBreezeAPI_Impl::lightSourceHasEnable(long deviceID, long featureID, int *errorCode,
        int lightSourceIndex) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return false;
    }

    return adapter->lightSourceHasEnable(featureID, errorCode, lightSourceIndex);
}

bool SeaBreezeAPI_Impl::lightSourceIsEnabled(long deviceID, long featureID, int *errorCode,
        int lightSourceIndex) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return false;
    }

    return adapter->lightSourceIsEnabled(featureID, errorCode, lightSourceIndex);
}

void SeaBreezeAPI_Impl::lightSourceSetEnable(long deviceID, long featureID, int *errorCode,
        int lightSourceIndex, bool enable) {

    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->lightSourceSetEnable(featureID, errorCode, lightSourceIndex, enable);
}

bool SeaBreezeAPI_Impl::lightSourceHasVariableIntensity(long deviceID, long featureID,
        int *errorCode, int lightSourceIndex) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return false;
    }

    return adapter->lightSourceHasVariableIntensity(featureID, errorCode,
            lightSourceIndex);
}

double SeaBreezeAPI_Impl::lightSourceGetIntensity(long deviceID, long featureID, int *errorCode,
        int lightSourceIndex) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return false;
    }

    return adapter->lightSourceGetIntensity(featureID, errorCode, lightSourceIndex);
}

void SeaBreezeAPI_Impl::lightSourceSetIntensity(long deviceID, long featureID, int *errorCode,
        int lightSourceIndex, double intensity) {

    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->lightSourceSetIntensity(featureID, errorCode,
            lightSourceIndex, intensity);
}


/**************************************************************************************/
//  NonLinearityCoeffs Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfNonlinearityCoeffsFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfNonlinearityCoeffsFeatures();
}


int SeaBreezeAPI_Impl::getNonlinearityCoeffsFeatures(long deviceID, int *errorCode,
        long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNonlinearityCoeffsFeatures(buffer, maxLength);
}


int SeaBreezeAPI_Impl::nonlinearityCoeffsGet(long deviceID, long featureID,
        int *errorCode, double *buffer, int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->nonlinearityCoeffsGet(featureID, errorCode, buffer, maxLength);
}


/**************************************************************************************/
//  Continuous Strobe Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfContinuousStrobeFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfContinuousStrobeFeatures();
}

int SeaBreezeAPI_Impl::getContinuousStrobeFeatures(long deviceID, int *errorCode, long *buffer,
        unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getContinuousStrobeFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::continuousStrobeSetContinuousStrobeEnable(long deviceID, long featureID,
        int *errorCode, bool strobeEnable) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    adapter->continuousStrobeSetEnable(featureID, errorCode, strobeEnable);
}

void SeaBreezeAPI_Impl::continuousStrobeSetContinuousStrobePeriodMicroseconds(long deviceID,
        long featureID, int *errorCode, unsigned long strobePeriodMicroseconds) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    adapter->continuousStrobeSetPeriodMicroseconds(featureID, errorCode, strobePeriodMicroseconds);
}

/**************************************************************************************/
//  Temperature Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfTemperatureFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfTemperatureFeatures();
}

int SeaBreezeAPI_Impl::getTemperatureFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getTemperatureFeatures(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::temperatureCountGet(long deviceID, long temperatureFeatureID,
        int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->temperatureCountGet(temperatureFeatureID, errorCode);
}

double SeaBreezeAPI_Impl::temperatureGet(long deviceID, long temperatureFeatureID,
        int *errorCode, int index) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->temperatureGet(temperatureFeatureID, errorCode, index);
}

int SeaBreezeAPI_Impl::temperatureGetAll(long deviceID, long temperatureFeatureID,
        int *errorCode, double *buffer, int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->temperatureGetAll(temperatureFeatureID, errorCode, buffer, maxLength);
}

/**************************************************************************************/
//  Introspection Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfIntrospectionFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfIntrospectionFeatures();
}

int SeaBreezeAPI_Impl::getIntrospectionFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getIntrospectionFeatures(buffer, maxLength);
}

unsigned short int SeaBreezeAPI_Impl::introspectionNumberOfPixelsGet(long deviceID, long introspectionFeatureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->introspectionNumberOfPixelsGet(introspectionFeatureID, errorCode);
}

int SeaBreezeAPI_Impl::introspectionActivePixelRangesGet(long deviceID, long introspectionFeatureID, int *errorCode, unsigned int *buffer, int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->introspectionActivePixelRangesGet(introspectionFeatureID, errorCode, buffer, maxLength);
}

int SeaBreezeAPI_Impl::introspectionElectricDarkPixelRangesGet(long deviceID, long introspectionFeatureID, int *errorCode, unsigned int *buffer, int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->introspectionElectricDarkPixelRangesGet(introspectionFeatureID, errorCode, buffer, maxLength);
}

int SeaBreezeAPI_Impl::introspectionOpticalDarkPixelRangesGet(long deviceID, long introspectionFeatureID, int *errorCode, unsigned int *buffer, int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->introspectionOpticalDarkPixelRangesGet(introspectionFeatureID, errorCode, buffer, maxLength);
}

/**************************************************************************************/
//  Revision Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfRevisionFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfRevisionFeatures();
}

int SeaBreezeAPI_Impl::getRevisionFeatures(long deviceID, int *errorCode,
        long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getRevisionFeatures(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::revisionHardwareGet(long deviceID, long revisionFeatureID,
        int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->revisionHardwareGet(revisionFeatureID, errorCode);
}

unsigned short int SeaBreezeAPI_Impl::revisionFirmwareGet(long deviceID, long revisionFeatureID,
        int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->revisionFirmwareGet(revisionFeatureID, errorCode);
}

/**************************************************************************************/
//  Optical Bench Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfOpticalBenchFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfOpticalBenchFeatures();
}

int SeaBreezeAPI_Impl::getOpticalBenchFeatures(long deviceID, int *errorCode,
        long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getOpticalBenchFeatures(buffer, maxLength);
}

unsigned short int SeaBreezeAPI_Impl::opticalBenchGetFiberDiameterMicrons(long deviceID, long opticalBenchFeatureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetFiberDiameterMicrons(opticalBenchFeatureID, errorCode);
}

unsigned short int SeaBreezeAPI_Impl::opticalBenchGetSlitWidthMicrons(long deviceID, long opticalBenchFeatureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetSlitWidthMicrons(opticalBenchFeatureID, errorCode);
}

int SeaBreezeAPI_Impl::opticalBenchGetID(long deviceID, long featureID, int *errorCode,
            char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetID(featureID, errorCode, buffer, bufferLength);
}

int SeaBreezeAPI_Impl::opticalBenchGetSerialNumber(long deviceID, long featureID, int *errorCode,
            char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetSerialNumber(featureID, errorCode, buffer, bufferLength);
}

int SeaBreezeAPI_Impl::opticalBenchGetCoating(long deviceID, long featureID, int *errorCode,
            char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetCoating(featureID, errorCode, buffer, bufferLength);
}

int SeaBreezeAPI_Impl::opticalBenchGetFilter(long deviceID, long featureID, int *errorCode,
            char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetFilter(featureID, errorCode, buffer, bufferLength);
}

int SeaBreezeAPI_Impl::opticalBenchGetGrating(long deviceID, long featureID, int *errorCode,
            char *buffer, int bufferLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->opticalBenchGetGrating(featureID, errorCode, buffer, bufferLength);
}

/**************************************************************************************/
//  Spectrum Processing Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfSpectrumProcessingFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfSpectrumProcessingFeatures();
}

int SeaBreezeAPI_Impl::getSpectrumProcessingFeatures(long deviceID, int *errorCode,
        long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getSpectrumProcessingFeatures(buffer, maxLength);
}

unsigned short int SeaBreezeAPI_Impl::spectrumProcessingScansToAverageGet(long deviceID, long spectrumProcessingFeatureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrumProcessingScansToAverageGet(spectrumProcessingFeatureID, errorCode);
}

unsigned char SeaBreezeAPI_Impl::spectrumProcessingBoxcarWidthGet(long deviceID, long spectrumProcessingFeatureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->spectrumProcessingBoxcarWidthGet(spectrumProcessingFeatureID, errorCode);
}

void SeaBreezeAPI_Impl::spectrumProcessingScansToAverageSet(long deviceID, long featureID,
        int *errorCode, unsigned short int scansToAverage) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->spectrumProcessingScansToAverageSet(featureID, errorCode,
            scansToAverage);
}

void SeaBreezeAPI_Impl::spectrumProcessingBoxcarWidthSet(long deviceID, long featureID,
    int *errorCode, unsigned char boxcarWidth) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->spectrumProcessingBoxcarWidthSet(featureID, errorCode,
            boxcarWidth);
}

/**************************************************************************************/
//  stray light Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfStrayLightCoeffsFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfStrayLightCoeffsFeatures();
}


int SeaBreezeAPI_Impl::getStrayLightCoeffsFeatures(long deviceID, int *errorCode,
        long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getStrayLightCoeffsFeatures(buffer, maxLength);
}
int SeaBreezeAPI_Impl::strayLightCoeffsGet(long deviceID, long featureID,
        int *errorCode, double *buffer, int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->strayLightCoeffsGet(featureID, errorCode, buffer, maxLength);
}


/**************************************************************************************/
//  Data buffer Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfDataBufferFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfDataBufferFeatures();
}

int SeaBreezeAPI_Impl::getDataBufferFeatures(long deviceID, int *errorCode, long *buffer,
        unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getDataBufferFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::dataBufferClear(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->dataBufferClear(featureID, errorCode);
}

void SeaBreezeAPI_Impl::dataBufferRemoveOldestSpectra(long deviceID, long featureID, int *errorCode, unsigned int numberOfSpectra) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->dataBufferRemoveOldestSpectra(featureID, errorCode, numberOfSpectra);
}

unsigned long SeaBreezeAPI_Impl::dataBufferGetNumberOfElements(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->dataBufferGetNumberOfElements(featureID, errorCode);
}

unsigned long SeaBreezeAPI_Impl::dataBufferGetBufferCapacity(long deviceID, long featureID, int *errorCode)
{
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter)
    {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->dataBufferGetBufferCapacity(featureID, errorCode);
}


unsigned long SeaBreezeAPI_Impl::dataBufferGetBufferCapacityMaximum(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->dataBufferGetBufferCapacityMaximum(featureID, errorCode);
}

unsigned long SeaBreezeAPI_Impl::dataBufferGetBufferCapacityMinimum(long deviceID, long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->dataBufferGetBufferCapacityMinimum(featureID, errorCode);
}

void SeaBreezeAPI_Impl::dataBufferSetBufferCapacity(long deviceID, long featureID, int *errorCode, unsigned long capacity)
{
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter)
    {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->dataBufferSetBufferCapacity(featureID, errorCode, capacity);
}



/**************************************************************************************/
//  Fast buffer Features for the SeaBreeze API class
/**************************************************************************************/


int SeaBreezeAPI_Impl::getNumberOfFastBufferFeatures(long deviceID, int *errorCode) {
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfFastBufferFeatures();
}

int SeaBreezeAPI_Impl::getFastBufferFeatures(long deviceID, int *errorCode, long *buffer,
	unsigned int maxLength) {
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getFastBufferFeatures(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::fastBufferGetBufferingEnable(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->fastBufferGetBufferingEnable(featureID, errorCode);
}

void SeaBreezeAPI_Impl::fastBufferSetBufferingEnable(long deviceID, long featureID, int *errorCode, unsigned char isEnabled)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->fastBufferSetBufferingEnable(featureID, errorCode, isEnabled);
}

unsigned int SeaBreezeAPI_Impl::fastBufferGetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->fastBufferGetConsecutiveSampleCount(featureID, errorCode);
}

void SeaBreezeAPI_Impl::fastBufferSetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode, unsigned int consecutiveSampleCount)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	adapter->fastBufferSetConsecutiveSampleCount(featureID, errorCode, consecutiveSampleCount);
}

/**************************************************************************************/
//  Acquisition delay Features for the SeaBreeze API class
/**************************************************************************************/
int SeaBreezeAPI_Impl::getNumberOfAcquisitionDelayFeatures(long deviceID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getNumberOfAcquisitionDelayFeatures();
}

int SeaBreezeAPI_Impl::getAcquisitionDelayFeatures(long deviceID,
        int *errorCode, long *buffer, unsigned int maxLength) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_SUCCESS);
    return adapter->getAcquisitionDelayFeatures(buffer, maxLength);
}

void SeaBreezeAPI_Impl::acquisitionDelaySetDelayMicroseconds(long deviceID, long featureID,
        int *errorCode, unsigned long delay_usec) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    adapter->acquisitionDelaySetDelayMicroseconds(featureID, errorCode, delay_usec);
}

unsigned long SeaBreezeAPI_Impl::acquisitionDelayGetDelayMicroseconds(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->acquisitionDelayGetDelayMicroseconds(featureID, errorCode);
}

unsigned long SeaBreezeAPI_Impl::acquisitionDelayGetDelayIncrementMicroseconds(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->acquisitionDelayGetDelayIncrementMicroseconds(featureID, errorCode);
}

unsigned long SeaBreezeAPI_Impl::acquisitionDelayGetDelayMaximumMicroseconds(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->acquisitionDelayGetDelayMaximumMicroseconds(featureID, errorCode);
}

unsigned long SeaBreezeAPI_Impl::acquisitionDelayGetDelayMinimumMicroseconds(long deviceID,
        long featureID, int *errorCode) {
    DeviceAdapter *adapter = getDeviceByID(deviceID);
    if(NULL == adapter) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    return adapter->acquisitionDelayGetDelayMinimumMicroseconds(featureID, errorCode);
}


/**************************************************************************************/
//  I2C Master Features for the SeaBreeze API class
/**************************************************************************************/

int SeaBreezeAPI_Impl::getNumberOfI2CMasterFeatures(long deviceID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getNumberOfI2CMasterFeatures();
}

int SeaBreezeAPI_Impl::getI2CMasterFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter)
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return adapter->getI2CMasterFeatures(buffer, maxLength);
}

unsigned char SeaBreezeAPI_Impl::i2cMasterGetNumberOfBuses(long deviceID, long featureID, int *errorCode)
{
	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL == adapter) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	return adapter->i2cMasterGetNumberOfBuses(featureID, errorCode);
}

unsigned short SeaBreezeAPI_Impl::i2cMasterReadBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
{
	unsigned short dataLength = 0;

	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		dataLength = adapter->i2cMasterReadBus(featureID, errorCode, busIndex, slaveAddress, readData, numberOfBytes);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
	return dataLength;
}

unsigned short SeaBreezeAPI_Impl::i2cMasterWriteBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
{
	unsigned short dataLength = 0;

	DeviceAdapter *adapter = getDeviceByID(deviceID);
	if (NULL != adapter)
	{
		dataLength = adapter->i2cMasterWriteBus(featureID, errorCode, busIndex, slaveAddress, writeData, numberOfBytes);
	}
	else
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}
	return dataLength;
}
