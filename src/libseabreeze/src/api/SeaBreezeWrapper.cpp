/***************************************************//**
 * @file    SeaBreezeWrapper.cpp
 * @date    March 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a simple wrapper around the SeaBreeze driver.
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
#include "common/Log.h"
#include "api/SeaBreezeWrapper.h"
#include "api/DeviceFactory.h"
#include "common/buses/rs232/RS232DeviceLocator.h"
#include "native/system/System.h"
#include "vendors/OceanOptics/buses/usb/OOIUSBInterface.h"
#include "vendors/OceanOptics/features/spectrometer/OOISpectrometerFeatureInterface.h"
#include "vendors/OceanOptics/features/data_buffer/DataBufferFeatureInterface.h"
#include "vendors/OceanOptics/features/fast_buffer/FastBufferFeatureInterface.h"
#include "vendors/OceanOptics/features/eeprom_slots/EEPROMSlotFeatureInterface.h"
#include "vendors/OceanOptics/features/shutter/ShutterFeatureInterface.h"
#include "vendors/OceanOptics/features/light_source/StrobeLampFeatureInterface.h"
#include "vendors/OceanOptics/features/light_source/LightSourceFeatureInterface.h"
#include "vendors/OceanOptics/features/continuous_strobe/ContinuousStrobeFeatureInterface.h"
#include "vendors/OceanOptics/features/serial_number/SerialNumberFeatureInterface.h"
#include "vendors/OceanOptics/features/thermoelectric/ThermoElectricFeatureInterface.h"
#include "vendors/OceanOptics/features/irradcal/IrradCalFeatureInterface.h"
#include "vendors/OceanOptics/features/acquisition_delay/AcquisitionDelayFeatureInterface.h"
#include "vendors/OceanOptics/features/network_configuration/NetworkConfigurationFeatureInterface.h"
#include "vendors/OceanOptics/features/ethernet_configuration/EthernetConfigurationFeatureInterface.h"
#include "vendors/OceanOptics/features/multicast/MulticastFeatureInterface.h"
#include "vendors/OceanOptics/features/ipv4/IPv4FeatureInterface.h"
#include "vendors/OceanOptics/features/wifi_configuration/WifiConfigurationFeatureInterface.h"
#include "vendors/OceanOptics/features/dhcp_server/DHCPServerFeatureInterface.h"
#include "vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeatureInterface.h"
#include "vendors/OceanOptics/features/gpio/gpioFeatureInterface.h"
#include "vendors/OceanOptics/features/i2c_master/i2cMasterFeatureInterface.h"

using namespace seabreeze;
using namespace std;

#include <ctype.h>
#include <vector>
#include <string.h>

#define SET_ERROR_CODE(code) do { if(NULL != errorCode) { *errorCode = code; }  } while(0)

#define ERROR_SUCCESS               0
#define ERROR_INVALID_ERROR         1
#define ERROR_NO_DEVICE             2
#define ERROR_FAILED_TO_CLOSE       3
#define ERROR_NOT_IMPLEMENTED       4
#define ERROR_FEATURE_NOT_FOUND     5
#define ERROR_TRANSFER_ERROR        6
#define ERROR_BAD_USER_BUFFER       7
#define ERROR_INPUT_OUT_OF_BOUNDS       8
#define ERROR_SPECTROMETER_SATURATED    9

#ifdef _WINDOWS
#pragma warning (disable: 4101) // unreferenced local variable
#endif

static const char *error_msgs[] = {
    "Success",
    "Error number undefined",
    "No device found",
    "Could not close device",
    "Feature not implemented",
    "No such feature on device",
    "Data transfer error",
    "Invalid user buffer provided"
};

static int number_error_msgs = sizeof (error_msgs) / sizeof (char *);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                          C++ Class Implementation                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

SeaBreezeWrapper *SeaBreezeWrapper::instance = NULL;

SeaBreezeWrapper::SeaBreezeWrapper() {
    LOG(__FUNCTION__);
    int i;
    
    System::initialize();
    
    for(i = 0; i < SEABREEZE_MAX_DEVICES; i++) {
        this->devices[i] = NULL;
    }
}

SeaBreezeWrapper::~SeaBreezeWrapper() {
    LOG(__FUNCTION__);
    int i;

    for(i = 0; i < SEABREEZE_MAX_DEVICES; i++) {
        if(NULL != this->devices[i]) {
            delete this->devices[i];
            this->devices[i] = NULL;
        }
    }
    
    System::shutdown();
}

SeaBreezeWrapper *SeaBreezeWrapper::getInstance() {
    if(NULL == instance) {
        instance = new SeaBreezeWrapper();
    }
    return instance;
}

void SeaBreezeWrapper::shutdown() {
    LOG(__FUNCTION__);
    if (NULL != instance)
    {
        delete instance;
        instance = NULL;
    }
    DeviceFactory::shutdown();
}

OOIUSBInterface *__seabreeze_getUSB(Device *dev, int *errorCode) {
    LOG(__FUNCTION__);
    if(NULL == dev) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return NULL;
    }

    vector<Bus *>::iterator iter;
    vector<Bus *> buses = dev->getBuses();

	// FIXME: This interates through all of the buses for this device and apparently
	//         picks the first one. Most of the time this would be USB... 
    for(iter = buses.begin(); iter != buses.end(); iter++) {
        OOIUSBInterface *usb = dynamic_cast<OOIUSBInterface *>(*iter);
        if(NULL != usb) {
            return usb;
        }
    }
    SET_ERROR_CODE(ERROR_NO_DEVICE);
    return NULL;
}

int __seabreeze_deviceCount(Device *dev) {
    LOG(__FUNCTION__);
    int retval = 0;
    vector<DeviceLocatorInterface *> *locations;
    vector<Bus *>::iterator iter;
    vector<Bus *> buses = dev->getBuses();
    for(iter = buses.begin(); iter != buses.end(); iter++) {
        OOIUSBInterface *usb = dynamic_cast<OOIUSBInterface *>(*iter);
        if(NULL != usb) {
            locations = usb->probeDevices();
            retval += (unsigned) locations->size();
            for(vector<DeviceLocatorInterface *>::iterator iter = locations->begin();
                    iter != locations->end(); iter++) {
                delete *iter;
            }
            locations->clear();
            delete locations;
        }
    }
    return retval;
}

// MZ: returns 1 on success, 0 on failure
int __seabreeze_openDevice(Device *dev, int instanceIndex, int *errorCode) {
    LOG(__FUNCTION__);

    int count;
    bool flag;

    // This will hold a list of all locations of the given Device type (e.g. USB2000PLUS)
    vector<DeviceLocatorInterface *> *locations;

    count = __seabreeze_deviceCount(dev);

    if(count > 0 && instanceIndex < count && instanceIndex >= 0) {
        /* There is at least one device of this type, so see if it
         * matches the desired index.
         */
        /* It is a little redundant to have to enumerate the bus
         * again, but this shouldn't hurt anything.
         */

        // get a USB interface specific to the specified device type
        OOIUSBInterface *usb = __seabreeze_getUSB(dev, errorCode);
        if(NULL != usb) {

            // get a list of all locations of the stated device type
            locations = usb->probeDevices();

            // error case: we're trying to open the n'th instance but
            // only (n - 1) were found
            if(instanceIndex >= (int) locations->size()) {
                // release memory
                if(locations->size() > 0) {
                    for(vector<DeviceLocatorInterface *>::iterator iter = locations->begin();
                            iter != locations->end();
                            iter++) {
                        delete *iter;
                    }
                    locations->clear();
                }
                delete locations;
                SET_ERROR_CODE(ERROR_NO_DEVICE);
                return 0;
            }

            if(locations->size() > 0) {
                usb->setLocation(*(*locations)[instanceIndex]);
                dev->setLocation(*(*locations)[instanceIndex]);
                dev->open();

                for(vector<DeviceLocatorInterface *>::iterator iter = locations->begin();
                        iter != locations->end(); iter++) {
                    delete *iter;
                }
                locations->clear();
                delete locations;
                flag = usb->open();
                if(true == flag) {
                    /* This API only ever needs to open the first device it
                     * finds.  This is pretty obsolete but should still work.
                     */
                    SET_ERROR_CODE(ERROR_SUCCESS);
                    return 1;
                }
            } else {
                delete locations;
            }
        }
    }

    SET_ERROR_CODE(ERROR_NO_DEVICE);
    return 0;
}

Bus *__seabreeze_getBus(Device *dev) {
    vector<Bus *> buses = dev->getBuses();
    return buses[0];
}

Protocol *__seabreeze_getProtocol(Device *dev) {
    vector<Protocol *> protocols = dev->getProtocols();
    return protocols[0];
}

template <class T>
T *__seabreeze_getFeature(Device *dev) {
    vector<Feature *>::iterator iter;
    vector<Feature *> features = dev->getFeatures();
    for(iter = features.begin(); iter != features.end(); iter++) {
        T *intf = dynamic_cast<T *>(*iter);
        if(NULL != intf) {
            return intf;
        }
    }
    return NULL;
}

/* This should set up default TEC parameters if available and gain adjustment
 * if necessary.  A no-argument initialize() function might be useful in
 * Device but this allows more explicit initialization around a particular
 * Bus which makes things cleaner underneath.
 */
void __seabreeze_initDevice(Device *dev) {
    Bus *bus = dev->getOpenedBus();
    if(NULL != bus) {
        dev->initialize(*bus);
    }
}

int SeaBreezeWrapper::readUSB(int index, int *errorCode, unsigned char endpoint, unsigned char *buffer, unsigned int length) {
	int bytesCopied = 0;
	Device * dev = this->devices[index];

	if (NULL == dev) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return bytesCopied;
	}

	if (NULL == buffer && length > 0) {
		SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		return bytesCopied;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	RawUSBBusAccessFeatureInterface *feature =
		__seabreeze_getFeature<RawUSBBusAccessFeatureInterface>(this->devices[index]);

	USBInterface *usb = __seabreeze_getUSB(dev, errorCode);

	vector<byte> data;

	if (NULL != feature && NULL != usb) {
		try {
			data = feature->readUSB(usb, endpoint, length);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return bytesCopied;
		}
	}

	bytesCopied = length > data.size() ? (int)data.size() : length;
	for (int i = 0; i < bytesCopied; i++)
	{
		buffer[i] = data[i];
	}

	return bytesCopied;
}

int SeaBreezeWrapper::writeUSB(int index, int *errorCode, unsigned char endpoint, unsigned char *buffer, unsigned int length) {

	int bytesWritten = 0;

	Device * dev = this->devices[index];

	if (NULL == dev) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return bytesWritten;
	}

	if (NULL == buffer && length > 0) {
		SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		return bytesWritten;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	RawUSBBusAccessFeatureInterface *feature =
		__seabreeze_getFeature<RawUSBBusAccessFeatureInterface>(this->devices[index]);

	USBInterface *usb = __seabreeze_getUSB(dev, errorCode);

	if (NULL != feature && NULL != usb) {
		try {
			vector<byte> data(length);
			for (unsigned i = 0; i < length; i++)
			{
				data[i] = buffer[i];
			}

			bytesWritten = feature->writeUSB(usb, endpoint, data);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return bytesWritten;
		}
	}

	return bytesWritten;
}

void SeaBreezeWrapper::setVerbose(bool flag) {
	LOG(__FUNCTION__);

	if (flag)
		logger.setLogLevel(OOI_LOG_LEVEL_DEBUG);
	else
		logger.setLogLevel(OOI_LOG_LEVEL_ERROR);

	seabreeze::USB::setVerbose(flag);
}

void SeaBreezeWrapper::setLogfile(char *pathname, int len) {
	LOG(__FUNCTION__);

	if (len <= 0 || pathname[0] == 0)
	{
		logger.setLogFile(stderr);
		return;
	}

	// extract pathname
	char path[256];
	memset(path, 0, sizeof(path));
	strncpy(path, pathname, len);
	logger.debug("setting logfile to %s", path);

	// open output file
	FILE *f = fopen(path, "a");

	// configure logging
	if (f != NULL)
	{
		logger.setLogLevel(OOI_LOG_LEVEL_DEBUG);
		logger.setLogFile(f);
		logger.debug("logfile set to %s", path);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Spectrometer feature
//////////////////////////////////////////////////////////////////////////////
int SeaBreezeWrapper::openSpectrometer(int index, int *errorCode) {
    LOG(__FUNCTION__);

    int i;
    int devicesFound;
    int instanceIndex;
    DeviceFactory *deviceFactory = DeviceFactory::getInstance();

    devicesFound = 0;
    SET_ERROR_CODE(ERROR_NO_DEVICE);
    for(i = 0; i < deviceFactory->getNumberOfDeviceTypes(); i++) {

        Device *dev = deviceFactory->create(i);  /* Create device exemplar */

        /* Query the number of devices of this type */
        logger.debug("Looking for %s", dev->getName().c_str());
        int instanceCount = __seabreeze_deviceCount(dev);

        if(instanceCount + devicesFound > index) {
            /* The specified index refers to a device of this type */
            instanceIndex = index - devicesFound;
            int flag = __seabreeze_openDevice(dev, instanceIndex, errorCode);
            if(flag > 0) {
                /* Got a valid device */
                if(NULL != this->devices[index]) {
                    closeSpectrometer(index, errorCode);
                }
                /* Store pointer to the new device */
                this->devices[index] = dev;
                __seabreeze_initDevice(dev);
                SET_ERROR_CODE(ERROR_SUCCESS);
                return 0;  /* Success */
            } else {
                /* Failed to open the device at that index so bail out */
                delete dev;
                return 1;
            }
        } else {
            /* Skip over all instances of this type */
            devicesFound += instanceCount;
            delete dev;
        }
    }

    /* Signal a failure to find any devices */
    return 1;
}

int SeaBreezeWrapper::closeSpectrometer(int index, int *errorCode) {
    bool flag = false;
    int error = ERROR_INVALID_ERROR;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 1;
    }

    OOIUSBInterface *usb = __seabreeze_getUSB(this->devices[index], errorCode);
    if(NULL != usb) {
        usb->close();
        error = ERROR_SUCCESS;
    } else {
        error = ERROR_NO_DEVICE;
    }

    SET_ERROR_CODE(error);

    delete this->devices[index];
    this->devices[index] = NULL;

    return (true == flag) ? 0 : 1;
}


void SeaBreezeWrapper::setTriggerMode(int index, int *errorCode, int mode) {
    LOG(__FUNCTION__);

    SpectrometerTriggerMode triggerMode(mode);

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    OOISpectrometerFeatureInterface *spec =
            __seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
    if(NULL != spec) {
        try {
            spec->setTriggerMode(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]),
                    triggerMode);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            logger.error("transfer error");
            return;
        }
    }
}


void SeaBreezeWrapper::setIntegrationTimeMicrosec(int index, int *errorCode,
        unsigned long integration_time_micros) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    OOISpectrometerFeatureInterface *spec =
            __seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
    if(NULL != spec) {
        try {
            spec->setIntegrationTimeMicros(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]),
                    integration_time_micros);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        } catch (IllegalArgumentException &iae) {
            SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
            return;
        }
    }

}

long SeaBreezeWrapper::getMinIntegrationTimeMicrosec(int index,
        int *errorCode) {
    long retval = -1;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return -1;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    OOISpectrometerFeatureInterface *spec =
            __seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
    if(NULL != spec) {
        try {
            retval = spec->getIntegrationTimeMinimum();
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return -1;
        }
    }
    return retval;
}

long SeaBreezeWrapper::getMaxIntegrationTimeMicrosec(int index,
        int *errorCode) {
    long retval = -1;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return -1;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    OOISpectrometerFeatureInterface *spec =
            __seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
    if(NULL != spec) {
        try {
            retval = spec->getIntegrationTimeMaximum();
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return -1;
        }
    }
    return retval;
}

int SeaBreezeWrapper::getMaximumIntensity(int index,
        int *errorCode) {
    long retval = -1;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return -1;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    OOISpectrometerFeatureInterface *spec =
            __seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
    if(NULL != spec) {
        try {
            retval = spec->getMaximumIntensity();
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return -1;
        }
    }
    return retval;
}


int SeaBreezeWrapper::getFastBufferSpectrum(int index, int *errorCode,
	unsigned char *buffer, int buffer_length, unsigned int numberOfSamplesToRetrieve) {
	vector<unsigned char> *spectrum;
	int bytesCopied = 0;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	if (NULL == buffer) {
		SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		try {
			spectrum = spec->getFastBufferSpectrum(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				numberOfSamplesToRetrieve);
			int bytes = (int)spectrum->size();
			bytesCopied = (bytes < buffer_length) ? bytes : buffer_length;
			memcpy(buffer, &((*spectrum)[0]),
				bytesCopied * sizeof(unsigned char));
			delete spectrum;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}
	return bytesCopied;
}

int SeaBreezeWrapper::getUnformattedSpectrum(int index, int *errorCode,
	unsigned char *buffer, int buffer_length) {
	vector<unsigned char> *spectrum;
	int bytesCopied = 0;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	if (NULL == buffer) {
		SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		try {
			spectrum = spec->getUnformattedSpectrum(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			int bytes = (int)spectrum->size();
			bytesCopied = (bytes < buffer_length) ? bytes : buffer_length;
			memcpy(buffer, &((*spectrum)[0]),
				bytesCopied * sizeof(unsigned char));
			delete spectrum;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}
	return bytesCopied;
}

int SeaBreezeWrapper::getFormattedSpectrum(int index, int *errorCode,
	double* buffer, int buffer_length) {

	LOG(__FUNCTION__);

	vector<double> *spectrum;
	int doublesCopied = 0;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	if (NULL == buffer) {
		SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		try {
			spectrum = spec->getFormattedSpectrum(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			int pixels = (int)spectrum->size();
			doublesCopied = (pixels < buffer_length) ? pixels : buffer_length;
			memcpy(buffer, &((*spectrum)[0]), doublesCopied * sizeof(double));
			delete spectrum;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}

	return doublesCopied;
}

int SeaBreezeWrapper::getUnformattedSpectrumLength(int index, int *errorCode) {
	/* This is, unfortunately, very hard to implement directly.
	* The readout length from the device is buried inside a particular
	* protocol message that would require a lot of digging to reach.
	* Since every protocol could have a different readout length,
	* this value cannot simply be stored in the spectrometer feature
	* like the number of pixels.
	*
	* This leaves two (not particularly nice) options.  The first is
	* to simply get an unformatted spectrum, check the length, and
	* throw it away.  The other is to create a lookup table of all
	* possible lengths based on the device type.  I am opting for
	* the former, even though it could have unpleasant side-effects.
	*/

	LOG(__FUNCTION__);

	vector<byte> *spectrum;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		try {
			spectrum = spec->getUnformattedSpectrum(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			int length = (int)spectrum->size();
			delete spectrum;
			SET_ERROR_CODE(ERROR_SUCCESS);
			return length;
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}
	return 0;
}

int SeaBreezeWrapper::getFormattedSpectrumLength(int index, int *errorCode) {

	int numberOfPixels = 0;
	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		try {
			numberOfPixels = spec->getNumberOfPixels();
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}
	return numberOfPixels;
}

int SeaBreezeWrapper::getWavelengths(int index, int *errorCode,
	double *wavelengths, int length) {
	int valuesCopied = 0;
	int i;
	vector<double> *wlVector;
	vector<double>::iterator iter;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		try {
			wlVector = spec->getWavelengths(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));

			/* It might be possible to do a memcpy() of the underlying vector into
			* the array, but that isn't the safest thing to do.  As long as this is
			* called once and the result cached, the inefficiency won't hurt.
			*/
			for (iter = wlVector->begin(), i = 0;
				iter != wlVector->end() && i < length; iter++, i++) {
				wavelengths[i] = *iter;
				valuesCopied++;
			}

			delete wlVector;

			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}
	return valuesCopied;
}

//////////////////////////////////////////////////////////////////////////////
// shutter feature
//////////////////////////////////////////////////////////////////////////////
void SeaBreezeWrapper::setShutterOpen(int index, int *errorCode,
        unsigned char opened) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    ShutterFeatureInterface *shutter =
            __seabreeze_getFeature<ShutterFeatureInterface>(this->devices[index]);
    if(NULL != shutter) {
        try {
            shutter->setShutterOpen(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]),
                    (0 == opened) ? false : true);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }

    return;
}

//////////////////////////////////////////////////////////////////////////////
// strobe feature
//////////////////////////////////////////////////////////////////////////////
void SeaBreezeWrapper::setStrobeEnable(int index, int *errorCode,
        unsigned char strobe_enable) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    StrobeLampFeatureInterface *lamp =
            __seabreeze_getFeature<StrobeLampFeatureInterface>(this->devices[index]);
    if(NULL != lamp) {
        try {
            lamp->setStrobeLampEnable(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0 == strobe_enable ? false : true);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }

    return;
}


//////////////////////////////////////////////////////////////////////////////
// Light Source feature
//////////////////////////////////////////////////////////////////////////////

int SeaBreezeWrapper::getLightSourceCount(int index, int *errorCode) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    LightSourceFeatureInterface *lightSource =
            __seabreeze_getFeature<LightSourceFeatureInterface>(this->devices[index]);
    if(NULL != lightSource) {
        try {
            int retval = lightSource->getLightSourceCount(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]));
            SET_ERROR_CODE(ERROR_SUCCESS);
            return retval;
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }
    }

    /* No light source feature found */
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    return 0;
}

void SeaBreezeWrapper::setLightSourceEnable(int index, int *errorCode,
            int lightIndex, unsigned char enable) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    /* Note: this implementation only looks for a single light source feature
     * instance.  This is sufficient for many cases (e.g. ISS-UV-VIS,
     * Ventana 785L) but does not cover all (e.g. Jaz with multiple lamp modules).
     * When a larger set of light sources is supported, this might need to be
     * made a bit smarter, iterating through multiple feature instances to count
     * up to the light source index requested.
     */
    LightSourceFeatureInterface *lightSource =
            __seabreeze_getFeature<LightSourceFeatureInterface>(this->devices[index]);
    if(NULL != lightSource) {
        try {
            Bus *bus = __seabreeze_getBus(this->devices[index]);
            Protocol *protocol = __seabreeze_getProtocol(this->devices[index]);

            /* Do some bounds checking */
            int sourceCount = lightSource->getLightSourceCount(*protocol, *bus);
            if(lightIndex < 0 || lightIndex >= sourceCount) {
                /* Out of bounds */
                SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
                return;
            }

            /* Ensure that the light source enable is legal */
            bool canEnable = lightSource->hasLightSourceEnable(*protocol, *bus,
                                lightIndex);
            if(false == canEnable) {
                /* The indicated light source cannot be enabled/disabled */
                SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
                return;
            }

            /* Proceed with trying to change the enable status */
            lightSource->setLightSourceEnable(
                    *protocol, *bus, lightIndex, 0 == enable ? false : true);
            SET_ERROR_CODE(ERROR_SUCCESS);
            return;
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
}

void SeaBreezeWrapper::setLightSourceIntensity(int index, int *errorCode,
            int lightIndex, double intensity) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    /* Note: this implementation only looks for a single light source feature
     * instance.  This is sufficient for many cases (e.g. ISS-UV-VIS,
     * Ventana 785L) but does not cover all (e.g. Jaz with multiple lamp modules).
     * When a larger set of light sources is supported, this might need to be
     * made a bit smarter, iterating through multiple feature instances to count
     * up to the light source index requested.
     */
    LightSourceFeatureInterface *lightSource =
            __seabreeze_getFeature<LightSourceFeatureInterface>(this->devices[index]);
    if(NULL != lightSource) {
        try {
            Bus *bus = __seabreeze_getBus(this->devices[index]);
            Protocol *protocol = __seabreeze_getProtocol(this->devices[index]);

            /* Do some bounds checking */
            int sourceCount = lightSource->getLightSourceCount(*protocol, *bus);
            if(lightIndex < 0 || lightIndex >= sourceCount) {
                /* Out of bounds */
                SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
                return;
            }

            /* Ensure that setting the intensity is legal */
            bool variable = lightSource->hasVariableIntensity(*protocol, *bus,
                                lightIndex);
            if(false == variable) {
                /* The indicated light source cannot have its intensity set */
                SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
                return;
            }

            /* Proceed with trying to change the intensity */
            lightSource->setLightSourceIntensity(*protocol, *bus,
                                lightIndex, intensity);
            SET_ERROR_CODE(ERROR_SUCCESS);
            return;
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
}


//////////////////////////////////////////////////////////////////////////////
// serial number feature
//////////////////////////////////////////////////////////////////////////////

unsigned char SeaBreezeWrapper::getSerialNumberMaximumLength(int index, int *errorCode)
{
	unsigned char serialNumberLength = 0;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	SerialNumberFeatureInterface *sn =
		__seabreeze_getFeature<SerialNumberFeatureInterface>(this->devices[index]);
	if (NULL != sn) {
		try 
		{
			serialNumberLength = sn->readSerialNumberMaximumLength(
				*__seabreeze_getProtocol(this->devices[index]), *__seabreeze_getBus(this->devices[index]));
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}
	}

	if (serialNumberLength == 0) {
		SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		return 0;
	}

	SET_ERROR_CODE(ERROR_SUCCESS);
	return serialNumberLength;
}

int SeaBreezeWrapper::getSerialNumber(int index, int *errorCode, char *buffer, int buffer_length) {

    string *serialNumber = NULL;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    SerialNumberFeatureInterface *sn =
        __seabreeze_getFeature<SerialNumberFeatureInterface>(this->devices[index]);
    if(NULL != sn) {
        try {
            serialNumber = sn->readSerialNumber(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]));
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }
    }

    if(NULL == serialNumber) {
        SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        return 0;
    }

    memset(buffer, (int)0, buffer_length);

    string::iterator iter;
    int i = 0;
    for(iter = serialNumber->begin(); iter != serialNumber->end() && i < buffer_length; iter++, i++) {
        buffer[i] = *iter;
    }
    delete serialNumber;

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}

//////////////////////////////////////////////////////////////////////////////
// data buffer feature
//////////////////////////////////////////////////////////////////////////////


void SeaBreezeWrapper::clearBuffer(int index, int *errorCode) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            buffer->clearBuffer(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}

void SeaBreezeWrapper::removeOldestSpectraFromBuffer(int index, int *errorCode, unsigned int numberOfSpectra) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            buffer->removeOldestSpectraFromBuffer(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0,
                    numberOfSpectra);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}


unsigned long SeaBreezeWrapper::getBufferElementCount(int index, int *errorCode) {
    unsigned long retval = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            retval = buffer->getNumberOfElements(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }

    }
    return retval;
}

unsigned long SeaBreezeWrapper::getBufferCapacity(int index, int *errorCode) {
    unsigned long retval = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            retval = buffer->getBufferCapacity(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }

    }
    return retval;
}

unsigned char SeaBreezeWrapper::getBufferingEnable(int index, int *errorCode) {
    unsigned char retval = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    FastBufferFeatureInterface *buffer =
            __seabreeze_getFeature<FastBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            retval = buffer->getBufferingEnable(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }

    }
    return retval;
}

//////////////////////////////////////////////////////////////////////////////
// fast buffer feature
//////////////////////////////////////////////////////////////////////////////

unsigned int SeaBreezeWrapper::getConsecutiveSampleCount(int index, int *errorCode) {
	unsigned int retval = 0;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	FastBufferFeatureInterface *buffer =
		__seabreeze_getFeature<FastBufferFeatureInterface>(this->devices[index]);
	if (NULL != buffer) {
		try {
			retval = buffer->getConsecutiveSampleCount(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]), 0);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			return 0;
		}

	}
	return retval;
}

unsigned long SeaBreezeWrapper::getBufferCapacityMaximum(int index, int *errorCode) {
    unsigned long retval = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            retval = buffer->getBufferCapacityMaximum(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }

    }
    return retval;
}

unsigned long SeaBreezeWrapper::getBufferCapacityMinimum(int index, int *errorCode) {
    unsigned long retval = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            retval = buffer->getBufferCapacityMinimum(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }

    }
    return retval;
}

void SeaBreezeWrapper::setBufferCapacity(int index, int *errorCode, unsigned long capacity) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    DataBufferFeatureInterface *buffer =
            __seabreeze_getFeature<DataBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            buffer->setBufferCapacity(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0, capacity);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}

void SeaBreezeWrapper::setBufferingEnable(int index, int *errorCode, unsigned char isEnabled) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    FastBufferFeatureInterface *buffer =
            __seabreeze_getFeature<FastBufferFeatureInterface>(this->devices[index]);
    if(NULL != buffer) {
        try {
            buffer->setBufferingEnable(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0, isEnabled);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}

void SeaBreezeWrapper::setConsecutiveSampleCount(int index, int *errorCode, unsigned int consecutiveSampleCount) {
	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}
	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	FastBufferFeatureInterface *buffer =
		__seabreeze_getFeature<FastBufferFeatureInterface>(this->devices[index]);
	if (NULL != buffer) {
		try {
			buffer->setConsecutiveSampleCount(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]), 0, consecutiveSampleCount);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe) {
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// eeprom feature
//////////////////////////////////////////////////////////////////////////////

int SeaBreezeWrapper::readEEPROMSlot(int index, int *errorCode,
        int slot_number, unsigned char *buffer, int buffer_length) {

    int bytesCopied = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    EEPROMSlotFeatureInterface *eeprom =
            __seabreeze_getFeature<EEPROMSlotFeatureInterface>(this->devices[index]);
    if(NULL != eeprom) {
        vector<byte> *info;

        try {
            info = eeprom->readEEPROMSlot(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), slot_number);

            int bytes = (int) info->size();
            bytesCopied = (bytes < buffer_length) ? bytes : buffer_length;
            memcpy(buffer, &((*info)[0]), bytesCopied * sizeof (unsigned char));
            delete info;
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }

    }
    return bytesCopied;
}

int SeaBreezeWrapper::writeEEPROMSlot(int index, int *errorCode,
        int slot_number, unsigned char *buffer, int buffer_length) {

    int bytesWritten = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return bytesWritten;
    }
    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    EEPROMSlotFeatureInterface *feature =
            __seabreeze_getFeature<EEPROMSlotFeatureInterface>(this->devices[index]);
    if(NULL != feature) {
        vector<byte> data(buffer_length);
        for (int i = 0; i < buffer_length; i++) {
            data[i] = buffer[i];
        }
        try {
            bytesWritten = feature->writeEEPROMSlot(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), slot_number, data);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }

    return bytesWritten;
}

//////////////////////////////////////////////////////////////////////////////
// irradiance calibration feature
//////////////////////////////////////////////////////////////////////////////

int SeaBreezeWrapper::readIrradCalibration(int index, int *errorCode,
        float *buffer, int buffer_length) {

    int floatsCopied = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    IrradCalFeatureInterface *irradCal =
        __seabreeze_getFeature<IrradCalFeatureInterface>(this->devices[index]);
    if(NULL != irradCal) {
        vector<float> *cal;

        try {
            cal = irradCal->readIrradCalibration(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]));
            int floats = (int) cal->size();
            floatsCopied = (floats < buffer_length) ? floats : buffer_length;
            memcpy(buffer, &((*cal)[0]), floatsCopied * sizeof(float));

            delete cal;
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }
    }
    return floatsCopied;
}

int SeaBreezeWrapper::writeIrradCalibration(int index, int *errorCode,
        float *buffer, int buffer_length) {
    int floatsCopied = 0;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    IrradCalFeatureInterface *irradCal =
        __seabreeze_getFeature<IrradCalFeatureInterface>(this->devices[index]);
    if(NULL != irradCal) {
        vector<float> *floatVector = new vector<float>;
        floatVector->resize(buffer_length);
        memcpy(&((*floatVector)[0]), buffer, buffer_length * sizeof(float));

        try {
            floatsCopied = irradCal->writeIrradCalibration(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]),
                *floatVector);
            delete floatVector;
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            delete floatVector;
            return 0;
        }
    }
    return floatsCopied;
}


int SeaBreezeWrapper::hasIrradCollectionArea(int index, int *errorCode) {
    int retval;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    IrradCalFeatureInterface *irradCal =
        __seabreeze_getFeature<IrradCalFeatureInterface>(this->devices[index]);
    if(NULL != irradCal) {
        /* hasCollectionArea() does not throw any exceptions. */
        retval = irradCal->hasCollectionArea(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]));
        SET_ERROR_CODE(ERROR_SUCCESS);
        return retval;
    }
    return 0;
}

float SeaBreezeWrapper::readIrradCollectionArea(int index, int *errorCode) {
    float area = 1.0;   /* Safe default since it removes area from calculations */

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    IrradCalFeatureInterface *irradCal =
        __seabreeze_getFeature<IrradCalFeatureInterface>(this->devices[index]);
    if(NULL != irradCal) {
        try {
            area = (float) irradCal->readCollectionArea(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]));
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
    return area;
}

void SeaBreezeWrapper::writeIrradCollectionArea(int index, int *errorCode,
        float area) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    IrradCalFeatureInterface *irradCal =
        __seabreeze_getFeature<IrradCalFeatureInterface>(this->devices[index]);
    if(NULL != irradCal) {
        try {
            irradCal->writeCollectionArea(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]),
                area);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// ethernet feature
//////////////////////////////////////////////////////////////////////////////

void SeaBreezeWrapper::get_MAC_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6]) 
{

    if(NULL == this->devices[index]) 
    {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    EthernetConfigurationFeatureInterface *ethernetConfigurationFI =
        __seabreeze_getFeature<EthernetConfigurationFeatureInterface>(this->devices[index]);

    if(NULL != ethernetConfigurationFI) 
    {
        vector<byte> macAddressBytes;

        try 
        {
            macAddressBytes = ethernetConfigurationFI->get_MAC_Address(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]),
                interfaceIndex);

			if (macAddressBytes.size() == 6)
			{
				memcpy(macAddress, &(macAddressBytes[0]), 6);
				SET_ERROR_CODE(ERROR_SUCCESS);
			}
			else
			{
				SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			}
	
        } 
        catch (FeatureException &fe) 
        {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}

void SeaBreezeWrapper::set_MAC_Address(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6]) 
{
    if(NULL == this->devices[index]) 
    {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    EthernetConfigurationFeatureInterface *ethernetConfigurationFI =
        __seabreeze_getFeature<EthernetConfigurationFeatureInterface>(this->devices[index]);
    if(NULL != ethernetConfigurationFI) 
	{
        vector<byte> *byteVector = new vector<byte>(6);
        memcpy(&((*byteVector)[0]), macAddress, 6 * sizeof(unsigned char));

        try 
        {
            ethernetConfigurationFI->set_MAC_Address(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]),
                interfaceIndex,
                *byteVector);
            delete byteVector;
            SET_ERROR_CODE(ERROR_SUCCESS);
        } 
        catch (FeatureException &fe) 
        {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            delete byteVector;
        }
    }
}

unsigned char SeaBreezeWrapper::get_GbE_Enable_Status(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;
	
    if(NULL == this->devices[index]) 
    {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    EthernetConfigurationFeatureInterface *ethernetConfigurationIF =
        __seabreeze_getFeature<EthernetConfigurationFeatureInterface>(this->devices[index]);
    if(NULL != ethernetConfigurationIF)
    {
        try 
        {
            enableState = ethernetConfigurationIF->get_GbE_Enable_Status(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } 
        catch (FeatureException &fe) 
        {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
    return enableState;
}

void SeaBreezeWrapper::set_GbE_Enable_Status(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState) 
{
    if(NULL == this->devices[index]) 
	{
        SET_ERROR_CODE(ERROR_NO_DEVICE);
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    EthernetConfigurationFeatureInterface *ethernetConfigurationIF =
        __seabreeze_getFeature<EthernetConfigurationFeatureInterface>(this->devices[index]);
    if(NULL != ethernetConfigurationIF) 
	{
        try 
		{
			ethernetConfigurationIF->set_GbE_Enable_Status(
                *__seabreeze_getProtocol(this->devices[index]),
                *__seabreeze_getBus(this->devices[index]),
                interfaceIndex,
                enableState);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } 
		catch (FeatureException &fe) 
		{
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
        }
    }
}






//////////////////////////////////////////////////////////////////////////////
// gpio feature
//////////////////////////////////////////////////////////////////////////////



unsigned char SeaBreezeWrapper::getGPIO_NumberOfPins(int index, int *errorCode)
{
	unsigned char numberOfPins = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			numberOfPins = GPIO_IF->getGPIO_NumberOfPins(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return numberOfPins;
}

unsigned int SeaBreezeWrapper::getGPIO_OutputEnableVector(int index, int *errorCode)
{
	unsigned int outputEnableVector = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			outputEnableVector = GPIO_IF->getGPIO_OutputEnableVector(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return outputEnableVector;
}

void SeaBreezeWrapper::setGPIO_OutputEnableVector(int index, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);

	if (NULL != GPIO_IF)
	{
		try
		{
			GPIO_IF->setGPIO_OutputEnableVector(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				outputEnableVector,
				bitMask);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

unsigned int SeaBreezeWrapper::getGPIO_ValueVector(int index, int *errorCode)
{
	unsigned int valueVector = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			valueVector = GPIO_IF->getGPIO_ValueVector(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return valueVector;
}

void SeaBreezeWrapper::setGPIO_ValueVector(int index, int *errorCode, unsigned int valueVector, unsigned int bitMask)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);

	if (NULL != GPIO_IF)
	{
		try
		{
			GPIO_IF->setGPIO_ValueVector(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				valueVector,
				bitMask);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

unsigned char SeaBreezeWrapper::getEGPIO_NumberOfPins(int index, int *errorCode)
{
	unsigned char numberOfPins = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			numberOfPins = GPIO_IF->getGPIO_NumberOfPins(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return numberOfPins;
}

unsigned char SeaBreezeWrapper::getEGPIO_AvailableModes(int index, int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maxLength)
{
	unsigned char modeCount = 0;
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return modeCount;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);


	if (NULL != GPIO_IF)
	{
		vector<byte> availableModesForEGPIO;

		try
		{
			availableModesForEGPIO = GPIO_IF->getEGPIO_AvailableModes(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				pinNumber);

			if (availableModesForEGPIO.size()<= maxLength)
			{
				memcpy(availableModes, &(availableModesForEGPIO[0]), availableModesForEGPIO.size());
				modeCount = availableModesForEGPIO.size() & 0xFF;
				SET_ERROR_CODE(ERROR_SUCCESS);
			}
			else
			{
				SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			}

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return modeCount;
}

unsigned char SeaBreezeWrapper::getEGPIO_CurrentMode(int index, int *errorCode, unsigned char pinNumber)
{
	unsigned char aMode = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			aMode = GPIO_IF->getEGPIO_CurrentMode(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				pinNumber);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return aMode;
}


void SeaBreezeWrapper::setEGPIO_Mode(int index, int *errorCode, unsigned char pinNumber, unsigned char mode, float value)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);

	if (NULL != GPIO_IF)
	{
		try
		{
			GPIO_IF->setEGPIO_Mode(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				pinNumber, mode, value);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

unsigned int SeaBreezeWrapper::getEGPIO_OutputVector(int index, int *errorCode)
{
	unsigned int outputVector = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			outputVector = GPIO_IF->getEGPIO_OutputVector(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return outputVector;
}

void SeaBreezeWrapper::setEGPIO_OutputVector(int index, int *errorCode, unsigned int outputVector, unsigned int bitMask)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);

	if (NULL != GPIO_IF)
	{
		try
		{
			GPIO_IF->setEGPIO_OutputVector(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				outputVector,
				bitMask);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

float SeaBreezeWrapper::getEGPIO_Value(int index, int *errorCode, unsigned char pinNumber)
{
	float aValue = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);
	if (NULL != GPIO_IF)
	{
		try
		{
			aValue = GPIO_IF->getEGPIO_Value(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				pinNumber);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return aValue;
}

void SeaBreezeWrapper::setEGPIO_Value(int index, int *errorCode, unsigned char pinNumber, float value)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	gpioFeatureInterface *GPIO_IF = __seabreeze_getFeature<gpioFeatureInterface>(this->devices[index]);

	if (NULL != GPIO_IF)
	{
		try
		{
			GPIO_IF->setEGPIO_Value(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				pinNumber,
				value);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}






//////////////////////////////////////////////////////////////////////////////
// i2c master feature
//////////////////////////////////////////////////////////////////////////////

unsigned char SeaBreezeWrapper::getI2CMasterNumberOfBuses(int index, int *errorCode)
{
	unsigned char numberOfBuses = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	i2cMasterFeatureInterface *i2cMasterFI = __seabreeze_getFeature<i2cMasterFeatureInterface>(this->devices[index]);
	if (NULL != i2cMasterFI)
	{
		try
		{
			numberOfBuses = i2cMasterFI->i2cMasterGetNumberOfBuses(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return numberOfBuses;
}

unsigned short SeaBreezeWrapper::readI2CMasterBus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
{
	unsigned short i2cReadLength = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return i2cReadLength;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	i2cMasterFeatureInterface *i2cMasterFI = __seabreeze_getFeature<i2cMasterFeatureInterface>(this->devices[index]);


	if (NULL != i2cMasterFI)
	{
		vector<unsigned char> i2cReadData;

		try
		{
			i2cReadData = i2cMasterFI->i2cMasterReadBus(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				busIndex,
				slaveAddress,
				numberOfBytes);
			i2cReadLength = i2cReadData.size() & 0xFFFF;

				memcpy(readData, &(i2cReadData[0]), i2cReadLength);
				SET_ERROR_CODE(ERROR_SUCCESS);

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}

	return i2cReadLength;
}

unsigned short SeaBreezeWrapper::writeI2CMasterBus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
{
	unsigned short writeDataLength = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return writeDataLength;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	i2cMasterFeatureInterface *i2cMasterFI = __seabreeze_getFeature<i2cMasterFeatureInterface>(this->devices[index]);

	if (NULL != i2cMasterFI)
	{
		vector<unsigned char> *byteVector = new vector<unsigned char>(numberOfBytes);
		memcpy(&((*byteVector)[0]), writeData, numberOfBytes);

		try
		{
			writeDataLength = i2cMasterFI->i2cMasterWriteBus(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				busIndex,
				slaveAddress,
				*byteVector);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
	return writeDataLength;
}









//////////////////////////////////////////////////////////////////////////////
// multicast feature
//////////////////////////////////////////////////////////////////////////////

#if 0
void SeaBreezeWrapper::getMulticastGroupAddress(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(&groupAddress)[4])
{

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	MulticastFeatureInterface *multicastFI =
		__seabreeze_getFeature<MulticastFeatureInterface>(this->devices[index]);

	if (NULL != multicastFI)
	{
		vector<byte> groupAddressBytes;

		try
		{
			groupAddressBytes = multicastFI->getGroupAddress(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);

			if (groupAddressBytes.size() == 6)
			{
				memcpy(groupAddress, &(groupAddressBytes[0]), 6);
				SET_ERROR_CODE(ERROR_SUCCESS);
			}
			else
			{
				SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			}

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

void SeaBreezeWrapper::setMulticastGroupAddress(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char groupAddress[4])
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	MulticastInterface *multicastFI =
		__seabreeze_getFeature<MulticastFeatureInterface>(this->devices[index]);
	if (NULL != MmlticastFI)
	{
		vector<byte> *byteVector = new vector<byte>(4);
		memcpy(&((*byteVector)[0]), groupAddress, 4 * sizeof(unsigned char));

		try
		{
			multicastFI->getGroupAddress(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				*byteVector);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
}

#endif

unsigned char SeaBreezeWrapper::getMutlicastEnableState(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	MulticastFeatureInterface *multicastIF =
		__seabreeze_getFeature<MulticastFeatureInterface>(this->devices[index]);
	if (NULL != multicastIF)
	{
		try
		{
			enableState = multicastIF->getEnableState(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return enableState;
}

void SeaBreezeWrapper::setMulticastEnableState(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	MulticastFeatureInterface *multicastIF =
		__seabreeze_getFeature<MulticastFeatureInterface>(this->devices[index]);
	if (NULL != multicastIF)
	{
		try
		{
			multicastIF->setEnableState(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				enableState);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// dhcp server feature
//////////////////////////////////////////////////////////////////////////////

void SeaBreezeWrapper::get_DHCP_Server_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(*dhcpServerAddress)[4], unsigned char *netMask)
{

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	DHCPServerFeatureInterface *DHCPServerFI =
		__seabreeze_getFeature<DHCPServerFeatureInterface>(this->devices[index]);

	if (NULL != DHCPServerFI)
	{
		vector<byte> dhcpServerAddressBytes;
		
		try
		{
			DHCPServerFI->getServerAddress(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex, &dhcpServerAddressBytes, netMask);

			if (dhcpServerAddressBytes.size() == 4)
			{
				memcpy(dhcpServerAddress, &(dhcpServerAddressBytes[0]), 4);
				SET_ERROR_CODE(ERROR_SUCCESS);
			}
			else
			{
				SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			}

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

void SeaBreezeWrapper::set_DHCP_Server_Address(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char dhcpServerAddress[4],  unsigned char netMask)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	DHCPServerFeatureInterface *dhcpServerFI =
		__seabreeze_getFeature<DHCPServerFeatureInterface>(this->devices[index]);
	if (NULL != dhcpServerFI)
	{
		vector<byte> *byteVector = new vector<byte>(4);
		memcpy(&((*byteVector)[0]), dhcpServerAddress, 4 * sizeof(unsigned char));

		try
		{
			dhcpServerFI->setServerAddress(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				*byteVector, netMask);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
}

unsigned char SeaBreezeWrapper::get_DHCP_Server_Enable_State(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	DHCPServerFeatureInterface *dhcpServerIF =
		__seabreeze_getFeature<DHCPServerFeatureInterface>(this->devices[index]);
	if (NULL != dhcpServerIF)
	{
		try
		{
			enableState = dhcpServerIF->getServerEnableState(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return enableState;
}

void SeaBreezeWrapper::set_DHCP_Server_Enable_State(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	DHCPServerFeatureInterface *dhcpServerIF =
		__seabreeze_getFeature<DHCPServerFeatureInterface>(this->devices[index]);
	if (NULL != dhcpServerIF)
	{
		try
		{
			dhcpServerIF->setServerEnableState(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				enableState);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// wifi feature
//////////////////////////////////////////////////////////////////////////////


unsigned char SeaBreezeWrapper::getWifiConfigurationMode(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char mode = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationIF =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != wifiConfigurationIF)
	{
		try
		{
			mode = wifiConfigurationIF->getMode(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return mode;
}

void SeaBreezeWrapper::setWifiConfigurationMode(int index, int *errorCode, unsigned char interfaceIndex, unsigned char mode)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationIF =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != wifiConfigurationIF)
	{
		try
		{
			wifiConfigurationIF->setMode(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				mode);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

unsigned char SeaBreezeWrapper::getWifiConfigurationSecurityType(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char securityType = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationIF =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != wifiConfigurationIF)
	{
		try
		{
			securityType = wifiConfigurationIF->getSecurityType(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return securityType;
}

void SeaBreezeWrapper::setWifiConfigurationSecurityType(int index, int *errorCode, unsigned char interfaceIndex, unsigned char securityType)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationIF =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != wifiConfigurationIF)
	{
		try
		{
			wifiConfigurationIF->setMode(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				securityType);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

unsigned char SeaBreezeWrapper::getWifiConfigurationSSID(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(*ssid)[32])
{
	unsigned char result = 0;
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationFI =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);

	if (NULL != wifiConfigurationFI)
	{
		vector<unsigned char> ssidBytes;

		try
		{
			ssidBytes = wifiConfigurationFI->getSSID(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);


			memcpy(ssid, &(ssidBytes[0]), ssidBytes.size());
			SET_ERROR_CODE(ERROR_SUCCESS);
			result = ssidBytes.size() & 0xFF;

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return result;
}

void SeaBreezeWrapper::setWifiConfigurationSSID(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationFI =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != wifiConfigurationFI)
	{
		vector<unsigned char> *byteVector = new vector<unsigned char>(length);
		memcpy(&((*byteVector)[0]), ssid, length * sizeof(unsigned char));

		try
		{
			wifiConfigurationFI->setSSID(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				*byteVector);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
}


void SeaBreezeWrapper::setWifiConfigurationPassPhrase(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength )
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	WifiConfigurationFeatureInterface *wifiConfigurationFI =
		__seabreeze_getFeature<WifiConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != wifiConfigurationFI)
	{
		vector<unsigned char> *byteVector = new vector<unsigned char>(passPhraseLength);
		memcpy(&((*byteVector)[0]), passPhrase, passPhraseLength * sizeof(unsigned char));

		try
		{
			wifiConfigurationFI->setSSID(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				*byteVector);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// ipv4 addressing feature
//////////////////////////////////////////////////////////////////////////////

unsigned char SeaBreezeWrapper::get_IPv4_DHCP_Enable_State(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4FI =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);
	if (NULL != IPv4FI)
	{
		try
		{
			enableState = IPv4FI->get_IPv4_DHCP_Enable_State(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return enableState;
}

void SeaBreezeWrapper::set_IPv4_DHCP_Enable_State(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4IF =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);
	if (NULL != IPv4IF)
	{
		try
		{
			IPv4IF->set_IPv4_DHCP_Enable_State(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				enableState);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}


unsigned char SeaBreezeWrapper::get_Number_Of_IPv4_Addresses(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char numberOfAddresses = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4FI =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);
	if (NULL != IPv4FI)
	{
		try
		{
			numberOfAddresses = IPv4FI->get_Number_Of_IPv4_Addresses(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return numberOfAddresses;
}


void SeaBreezeWrapper::get_IPv4_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask)
{

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4FI =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);

	if (NULL != IPv4FI)
	{
		vector<byte> IPv4_Address_bytes;

		try
		{
			IPv4FI->get_IPv4_Address(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex, addressIndex, &IPv4_Address_bytes, netMask);

			if (IPv4_Address_bytes.size() == 4)
			{
				memcpy(IPv4_Address, &(IPv4_Address_bytes[0]), 4);
				SET_ERROR_CODE(ERROR_SUCCESS);
			}
			else
			{
				SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			}

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

void SeaBreezeWrapper::get_IPv4_Default_Gateway(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4])
{

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4FI =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);

	if (NULL != IPv4FI)
	{
		vector<byte> defaultGatewayAddressBytes;

		try
		{
			defaultGatewayAddressBytes = IPv4FI->get_IPv4_Default_Gateway(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);

			if (defaultGatewayAddressBytes.size() == 4)
			{
				memcpy(defaultGatewayAddress, &(defaultGatewayAddressBytes[0]), 4);
				SET_ERROR_CODE(ERROR_SUCCESS);
			}
			else
			{
				SET_ERROR_CODE(ERROR_INPUT_OUT_OF_BOUNDS);
			}

		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

void SeaBreezeWrapper::set_IPv4_Default_Gateway(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4])
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4FI =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);
	if (NULL != IPv4FI)
	{
		vector<byte> *byteVector = new vector<byte>(4);
		memcpy(&((*byteVector)[0]), defaultGatewayAddress, 4 * sizeof(unsigned char));

		try
		{
			IPv4FI->set_IPv4_Default_Gateway(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				*byteVector);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
}

void SeaBreezeWrapper::add_IPv4_Address(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4FI =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);
	if (NULL != IPv4FI)
	{
		vector<byte> *byteVector = new vector<byte>(4);
		memcpy(&((*byteVector)[0]), IPv4_Address, 4 * sizeof(unsigned char));

		try
		{
			IPv4FI->add_IPv4_Address(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				*byteVector,
				netMask);
			delete byteVector;
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
			delete byteVector;
		}
	}
}

void SeaBreezeWrapper::delete_IPv4_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	IPv4FeatureInterface *IPv4IF =
		__seabreeze_getFeature<IPv4FeatureInterface>(this->devices[index]);
	if (NULL != IPv4IF)
	{
		try
		{
			IPv4IF->delete_IPv4_Address(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				addressIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// network configuration feature
//////////////////////////////////////////////////////////////////////////////

unsigned char SeaBreezeWrapper::getNumberOfNetworkInterfaces(int index, int *errorCode)
{
	unsigned char numberOfInterfaces = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	NetworkConfigurationFeatureInterface *networkConfigurationIF =
		__seabreeze_getFeature<NetworkConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != networkConfigurationIF)
	{
		try
		{
			numberOfInterfaces = networkConfigurationIF->getNumberOfNetworkInterfaces(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]));
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return numberOfInterfaces;
}


unsigned char SeaBreezeWrapper::getNetworkInterfaceConnectionType(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char connectionType = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	NetworkConfigurationFeatureInterface *networkConfigurationIF =
		__seabreeze_getFeature<NetworkConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != networkConfigurationIF)
	{
		try
		{
			connectionType = networkConfigurationIF->getNetworkInterfaceConnectionType(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return connectionType;
}

unsigned char SeaBreezeWrapper::getNetworkInterfaceEnableState(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char enableState = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	NetworkConfigurationFeatureInterface *networkConfigurationIF =
		__seabreeze_getFeature<NetworkConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != networkConfigurationIF)
	{
		try
		{
			enableState = networkConfigurationIF->getNetworkInterfaceEnableState(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return enableState;
}

unsigned char SeaBreezeWrapper::runNetworkInterfaceSelfTest(int index, int *errorCode, unsigned char interfaceIndex)
{
	unsigned char result = 0;

	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	NetworkConfigurationFeatureInterface *networkConfigurationIF =
		__seabreeze_getFeature<NetworkConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != networkConfigurationIF)
	{
		try
		{
			result = networkConfigurationIF->runNetworkInterfaceSelfTest(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
	return result;
}

void SeaBreezeWrapper::setNetworkInterfaceEnableState(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	NetworkConfigurationFeatureInterface *networkConfigurationIF =
		__seabreeze_getFeature<NetworkConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != networkConfigurationIF)
	{
		try
		{
			networkConfigurationIF->setNetworkInterfaceEnableState(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex,
				enableState);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

void SeaBreezeWrapper::saveNetworkInterfaceConnectionSettings(int index, int *errorCode, unsigned char interfaceIndex)
{
	if (NULL == this->devices[index])
	{
		SET_ERROR_CODE(ERROR_NO_DEVICE);
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	NetworkConfigurationFeatureInterface *networkConfigurationIF =
		__seabreeze_getFeature<NetworkConfigurationFeatureInterface>(this->devices[index]);
	if (NULL != networkConfigurationIF)
	{
		try
		{
			networkConfigurationIF->saveNetworkInterfaceConnectionSettings(
				*__seabreeze_getProtocol(this->devices[index]),
				*__seabreeze_getBus(this->devices[index]),
				interfaceIndex);
			SET_ERROR_CODE(ERROR_SUCCESS);
		}
		catch (FeatureException &fe)
		{
			SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// thermo electric control feature
//////////////////////////////////////////////////////////////////////////////


double SeaBreezeWrapper::readTECTemperature(int index, int *errorCode) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    ThermoElectricFeatureInterface *tec =
            __seabreeze_getFeature<ThermoElectricFeatureInterface>(this->devices[index]);
    if(NULL != tec) {
        double retval;

        try {
            retval = tec->getTemperatureCelsius(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]));
            SET_ERROR_CODE(ERROR_SUCCESS);
            return retval;
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return 0;
        }
    }

    return 0;
}

void SeaBreezeWrapper::setTECTemperature(int index, int *errorCode,
        double temperature_degrees_celsius) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    ThermoElectricFeatureInterface *tec =
            __seabreeze_getFeature<ThermoElectricFeatureInterface>(this->devices[index]);
    if(NULL != tec) {
        try {
            tec->setTemperatureSetPointCelsius(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]),
                    temperature_degrees_celsius);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }

    return;
}

void SeaBreezeWrapper::setTECEnable(int index, int *errorCode,
        unsigned char tec_enable) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
    ThermoElectricFeatureInterface *tec =
            __seabreeze_getFeature<ThermoElectricFeatureInterface>(this->devices[index]);
    if(NULL != tec) {
        try {
            tec->setThermoElectricEnable(*__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]), 0 == tec_enable ? false : true);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }

    return;
}

void SeaBreezeWrapper::setTECFanEnable(int index, int *errorCode,
        unsigned char tec_fan_enable) {
    SET_ERROR_CODE(ERROR_NOT_IMPLEMENTED);
    /* FIXME: MISSING_IMPL */
    return;
}



//////////////////////////////////////////////////////////////////////////////
// Introspection feature
//////////////////////////////////////////////////////////////////////////////


unsigned short SeaBreezeWrapper::getNumberOfPixels(int index, int *errorCode) {
	unsigned int numberOfPixels = 0;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		numberOfPixels = spec->getNumberOfPixels();

		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	return numberOfPixels;
}

int SeaBreezeWrapper::getElectricDarkPixelIndices(int index, int *errorCode,
        int *indices, int length) {
    int valuesCopied = 0;
    int i;
    vector<unsigned int> pixelVector;
    vector<unsigned int>::iterator iter;

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return 0;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    OOISpectrometerFeatureInterface *spec =
        __seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
    if(NULL != spec) {
        pixelVector = spec->getElectricDarkPixelIndices();

        /* It might be possible to do a memcpy() of the underlying vector into
         * the array, but that isn't the safest thing to do.  As long as this is
         * called once and the result cached, the inefficiency won't hurt.
         */
        for(iter = pixelVector.begin(), i = 0;
                iter != pixelVector.end() && i < length; iter++, i++) {
            indices[i] = *iter;
            valuesCopied++;
        }

        SET_ERROR_CODE(ERROR_SUCCESS);
    }
    return valuesCopied;
}

int SeaBreezeWrapper::getOpticalDarkPixelIndices(int index, int *errorCode,
	int *indices, int length) {
	int valuesCopied = 0;
	int i;
	vector<unsigned int> pixelVector;
	vector<unsigned int>::iterator iter;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		pixelVector = spec->getOpticalDarkPixelIndices();

		/* It might be possible to do a memcpy() of the underlying vector into
		* the array, but that isn't the safest thing to do.  As long as this is
		* called once and the result cached, the inefficiency won't hurt.
		*/
		for (iter = pixelVector.begin(), i = 0;
			iter != pixelVector.end() && i < length; iter++, i++) {
			indices[i] = *iter;
			valuesCopied++;
		}

		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	return valuesCopied;
}

int SeaBreezeWrapper::getActivePixelIndices(int index, int *errorCode,
	int *indices, int length) {
	int valuesCopied = 0;
	int i;
	vector<unsigned int> pixelVector;
	vector<unsigned int>::iterator iter;

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		return 0;
	}

	SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

	OOISpectrometerFeatureInterface *spec =
		__seabreeze_getFeature<OOISpectrometerFeatureInterface>(this->devices[index]);
	if (NULL != spec) {
		pixelVector = spec->getActivePixelIndices();

		/* It might be possible to do a memcpy() of the underlying vector into
		* the array, but that isn't the safest thing to do.  As long as this is
		* called once and the result cached, the inefficiency won't hurt.
		*/
		for (iter = pixelVector.begin(), i = 0;
			iter != pixelVector.end() && i < length; iter++, i++) {
			indices[i] = *iter;
			valuesCopied++;
		}

		SET_ERROR_CODE(ERROR_SUCCESS);
	}
	return valuesCopied;
}


int SeaBreezeWrapper::getModel(int index, int *errorCode,
	char *buffer, int buffer_length) {

	memset(buffer, (int)0, buffer_length);

	if (NULL == this->devices[index]) {
		SET_ERROR_CODE(ERROR_NO_DEVICE);
		strncpy(buffer, "NONE", buffer_length);
		return buffer_length < 4 ? buffer_length : 4;
	}

	string name = this->devices[index]->getName();
	string::iterator iter;

	int i = 0;
	for (iter = name.begin(); iter != name.end() && i < buffer_length; iter++) {
		if ('+' == *iter) {
			strcat(buffer, "PLUS");
			i += 4;
		}
		else {
			buffer[i] = toupper(*iter);
			i++;
		}
	}

	buffer[buffer_length - 1] = '\0'; /* Guarantee a null terminator */

	SET_ERROR_CODE(ERROR_SUCCESS);
	return i;
}

int SeaBreezeWrapper::getModelStringMaximumLength(void)
{

	int maxLength = -1;

	for (int index = 0; index < SEABREEZE_MAX_DEVICES; index++)
	{
		if (this->devices[index] != NULL)
		{
			size_t myLength = this->devices[index]->getName().length();
			maxLength = myLength & 0xFFFFFFFF;
		}
		else
			break;

	}
	return maxLength + 1; // leave room for null terminiation
}

int SeaBreezeWrapper::getErrorStringMaximumLength(void)
{
	int maxLength = -1;

	for (int i = 0; i<number_error_msgs; i++)
	{
		size_t myLength = strlen(error_msgs[i]);
			maxLength = myLength & 0xFFFFFFFF;
	}
	return maxLength + 1; // leave room for null terminiation
}

int SeaBreezeWrapper::getErrorString(int error_code, char *buffer, int buffer_length) {
    const char* msg = NULL;
    if((error_code < 0) || (error_code >= number_error_msgs)) {
        msg = error_msgs[ERROR_INVALID_ERROR];
    } else {
        msg = error_msgs[error_code];
    }

    memset(buffer, 0, buffer_length);
    int len = (int) strlen(msg);
    if (len > buffer_length - 1) { // -1 leaves room for terminating NULL
        len = buffer_length - 1;
    }

    strncpy(buffer, msg, len);
    buffer[len] = '\0';

    return len;
}

int SeaBreezeWrapper::getAPIVersionString(char *buffer, int len) {
    memset(buffer, 0, len);
    strncpy(buffer, SEABREEZE_API_VERSION, len);
    buffer[len - 1] = 0;
    return (int) strlen(buffer);
}

int SeaBreezeWrapper::getUSBDescriptorString(int index, int *errorCode, int id, unsigned char *buffer, int length) {
    int bytesCopied = 0;
    memset(buffer, 0, length);

    Device * dev = this->devices[index];

    if (NULL == dev) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return bytesCopied;
    }

    if (NULL == buffer && length > 0) {
        SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
        return bytesCopied;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    RawUSBBusAccessFeatureInterface *feature =
        __seabreeze_getFeature<RawUSBBusAccessFeatureInterface>(this->devices[index]);

    USBInterface *usb = __seabreeze_getUSB(dev, errorCode);

    if(NULL != feature && NULL != usb) {
        try {
            string* desc = usb->getUSBDescriptor()->getStringDescriptor(id);
            if (NULL != desc) {
                int descLen = (int) desc->size();
                bytesCopied = descLen < length ? descLen : length;
                strncpy((char*) buffer, desc->c_str(), bytesCopied);
                delete(desc);
                SET_ERROR_CODE(ERROR_SUCCESS);
            } else {
                SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            }
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return bytesCopied;
        }
    }

    return bytesCopied;
}

//////////////////////////////////////////////////////////////////////////////
// continuous strobe feature
//////////////////////////////////////////////////////////////////////////////
void SeaBreezeWrapper::setContinuousStrobePeriodMicrosec(int index, int *errorCode,
        unsigned short strobe_id, unsigned long period_usec) {
    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    ContinuousStrobeFeatureInterface *feature =
        __seabreeze_getFeature<ContinuousStrobeFeatureInterface>(this->devices[index]);
    if(NULL != feature) {
        try {
            feature->setContinuousStrobePeriodMicroseconds(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]),
                    strobe_id, period_usec);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// acquisition delay feature
//////////////////////////////////////////////////////////////////////////////
void SeaBreezeWrapper::setAcquisitionDelayMicrosec(int index, int *errorCode,
        unsigned long delay_usec) {

    if(NULL == this->devices[index]) {
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return;
    }

    SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);

    AcquisitionDelayFeatureInterface *feature =
        __seabreeze_getFeature<AcquisitionDelayFeatureInterface>(this->devices[index]);
    if(NULL != feature) {
        try {
            feature->setAcquisitionDelayMicroseconds(
                    *__seabreeze_getProtocol(this->devices[index]),
                    *__seabreeze_getBus(this->devices[index]),
                    delay_usec);
            SET_ERROR_CODE(ERROR_SUCCESS);
        } catch (FeatureException &fe) {
            SET_ERROR_CODE(ERROR_TRANSFER_ERROR);
            return;
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                           C Function Wrappers                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// see documentation in SeaBreezeWrapper.h

int seabreeze_open_spectrometer(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->openSpectrometer(index, error_code);
}

int seabreeze_close_spectrometer(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->closeSpectrometer(index, error_code);
}



int seabreeze_get_error_string_maximum_length(void)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getErrorStringMaximumLength();
}

int seabreeze_get_error_string(int error_code, char *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getErrorString(error_code, buffer, buffer_length);
}

int seabreeze_get_model_string_maximum_length(void)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getModelStringMaximumLength();
}

int seabreeze_get_model(int index, int *error_code, char *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getModel(index, error_code, buffer, buffer_length);
}



void seabreeze_set_trigger_mode(int index, int *error_code, int mode) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setTriggerMode(index, error_code, mode);
}

void seabreeze_set_integration_time_microsec(int index, int *error_code, unsigned long integration_time_micros) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setIntegrationTimeMicrosec(index, error_code, integration_time_micros);
}

long seabreeze_get_min_integration_time_microsec(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getMinIntegrationTimeMicrosec(index, error_code);
}



void seabreeze_set_shutter_open(int index, int *error_code, unsigned char opened) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setShutterOpen(index, error_code, opened);
}



void seabreeze_set_strobe_enable(int index, int *error_code, unsigned char strobe_enable)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setStrobeEnable(index, error_code, strobe_enable);
}



int seabreeze_get_light_source_count(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getLightSourceCount(index, error_code);
}

void seabreeze_set_light_source_enable(int index, int *error_code, int light_index, unsigned char enable) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setLightSourceEnable(index, error_code, light_index, enable);
}

void seabreeze_set_light_source_intensity(int index, int *error_code, int light_index, double intensity) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setLightSourceIntensity(index, error_code, light_index, intensity);
}



int seabreeze_read_eeprom_slot(int index, int *error_code, int slot_number, unsigned char *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->readEEPROMSlot(index, error_code, slot_number, buffer,
            buffer_length);
}

int seabreeze_write_eeprom_slot(int index, int *error_code, int slot_number, unsigned char *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->writeEEPROMSlot(index, error_code, slot_number, buffer,
            buffer_length);
}



int seabreeze_read_irrad_calibration(int index, int *error_code, float *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->readIrradCalibration(index, error_code, buffer,
        buffer_length);
}

int seabreeze_write_irrad_calibration(int index, int *error_code, float *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->writeIrradCalibration(index, error_code, buffer,
                buffer_length);
}

int seabreeze_has_irrad_collection_area(int index, int *error_code)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->hasIrradCollectionArea(index, error_code);
}

float seabreeze_read_irrad_collection_area(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->readIrradCollectionArea(index, error_code);
}

void seabreeze_write_irrad_collection_area(int index, int *error_code, float area) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->writeIrradCollectionArea(index, error_code, area);
}


void seabreeze_get_mac_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char (*macAddress)[6])
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_MAC_Address(index, error_code, interfaceIndex, macAddress);
}

void seabreeze_set_mac_address(int index, int *error_code, unsigned char interfaceIndex, const unsigned char macAddress[6])
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->set_MAC_Address(index, error_code, interfaceIndex, macAddress);
}

unsigned char seabreeze_get_gbe_enable(int index, int *error_code, unsigned char interfaceIndex)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_GbE_Enable_Status(index, error_code, interfaceIndex);
}

void seabreeze_set_gbe_enable(int index, int *error_code, unsigned char interfaceIndex, unsigned char GbE_Enable) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->set_GbE_Enable_Status(index, error_code, interfaceIndex, GbE_Enable);
}

unsigned char seabreeze_get_multicast_enable(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getMutlicastEnableState(index, error_code, interfaceIndex);
}

void seabreeze_set_multicast_enable(int index, int *error_code, unsigned char interfaceIndex, unsigned char multicastEnable)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setMulticastEnableState(index, error_code, interfaceIndex, multicastEnable);
}

// this is a convenience function. in the future the port will be settable
unsigned short seabreeze_get_multicast_group_port(int index, int *error_code, unsigned char interfaceIndex)
{
	//SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	//return wrapper->getMutlicastGroupPort(index, error_code, interfaceIndex);
	return 57357;
}

// this is a convenience function. in the future the group address will be settable
void seabreeze_get_multicast_group_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char(*groupAddress)[4])
{
	//SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	// wrapper->getMulticastGroupAddress(index, error_code, interfaceIndex, groupAddress);
	for (int i = 0; i < 4; i++)
		(*groupAddress[i]) = 239;
}

unsigned char seabreeze_get_wifi_mode(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getWifiConfigurationMode(index, error_code, interfaceIndex);
}

void seabreeze_set_wifi_mode(int index, int *error_code, unsigned char interfaceIndex, unsigned char wifiMode)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setWifiConfigurationMode(index, error_code, interfaceIndex, wifiMode);
}

unsigned char seabreeze_get_wifi_security_type(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getWifiConfigurationSecurityType(index, error_code, interfaceIndex);
}

void seabreeze_set_wifi_security_type(int index, int *error_code, unsigned char interfaceIndex, unsigned char wifiMode)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setWifiConfigurationSecurityType(index, error_code, interfaceIndex, wifiMode);
}

unsigned char seabreeze_get_wifi_ssid(int index, int *error_code, unsigned char interfaceIndex, unsigned char(*ssid)[32])
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getWifiConfigurationSSID(index, error_code, interfaceIndex, ssid);
}

void seabreeze_set_wifi_ssid(int index, int *error_code, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setWifiConfigurationSSID(index, error_code, interfaceIndex, ssid, length);
}

void seabreeze_set_wifi_pass_phrase(int index, int *error_code, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setWifiConfigurationPassPhrase(index, error_code, interfaceIndex, passPhrase, passPhraseLength);
}


void seabreeze_get_dhcp_server_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char (*serverAddress)[4], unsigned char *netMask)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_DHCP_Server_Address(index, error_code, interfaceIndex, serverAddress, netMask);
}

void seabreeze_set_dhcp_server_address(int index, int *error_code, unsigned char interfaceIndex, const unsigned char serverAddress[4], const unsigned char netMask)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->set_DHCP_Server_Address(index, error_code, interfaceIndex, serverAddress, netMask);
}

unsigned char seabreeze_get_dhcp_server_enable_state(int index, int *error_code, unsigned char interfaceIndex)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_DHCP_Server_Enable_State(index, error_code, interfaceIndex);
}

void seabreeze_set_dhcp_server_enable_state(int index, int *error_code, unsigned char interfaceIndex, unsigned char serverEnable)
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->set_DHCP_Server_Enable_State(index, error_code, interfaceIndex, serverEnable);
}



unsigned char seabreeze_get_number_of_network_interfaces(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getNumberOfNetworkInterfaces(index, error_code);
}

unsigned char seabreeze_get_network_interface_connection_type(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getNetworkInterfaceConnectionType(index, error_code, interfaceIndex);
}

unsigned char seabreeze_get_network_interface_enable_state(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getNetworkInterfaceEnableState(index, error_code, interfaceIndex);
}

void seabreeze_set_network_interface_enable_state(int index, int *error_code, unsigned char interfaceIndex, unsigned char enableState)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setNetworkInterfaceEnableState(index, error_code, interfaceIndex, enableState);
}

unsigned char seabreeze_run_network_interface_self_test(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->runNetworkInterfaceSelfTest(index, error_code, interfaceIndex);
}

void seabreeze_save_network_interface_connection_settings(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->saveNetworkInterfaceConnectionSettings(index, error_code, interfaceIndex);
}



unsigned char seabreeze_get_ipv4_dhcp_enable_state(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_IPv4_DHCP_Enable_State(index, error_code, interfaceIndex);
}

void seabreeze_set_ipv4_dhcp_enable_state(int index, int *error_code, unsigned char interfaceIndex, unsigned char enableState)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->set_DHCP_Server_Enable_State(index, error_code, interfaceIndex, enableState);
}

unsigned char seabreeze_get_Number_Of_IPv4_Addresses(int index, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_Number_Of_IPv4_Addresses(index, error_code, interfaceIndex);
}

void seabreeze_get_ipv4_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_IPv4_Address(index, error_code, interfaceIndex, addressIndex, IPv4_Address, netMask);
}

void seabreeze_get_ipv4_default_gateway(int index, int *error_code, unsigned char interfaceIndex, unsigned char(*IPv4_Address)[4])
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->get_IPv4_Default_Gateway(index, error_code, interfaceIndex, IPv4_Address);
}

void seabreeze_set_ipv4_default_gateway(int index, int *error_code, unsigned char interfaceIndex, const unsigned char IPv4_Address[4])
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->set_IPv4_Default_Gateway(index, error_code, interfaceIndex, IPv4_Address);
}

void seabreeze_add_ipv4_address(int index, int *error_code, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->add_IPv4_Address(index, error_code, interfaceIndex, IPv4_Address, netMask);
}

void seabreeze_delete_ipv4_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char addressIndex)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->delete_IPv4_Address(index, error_code, interfaceIndex, addressIndex);
}



double seabreeze_read_tec_temperature(int index, int *error_code) {
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->readTECTemperature(index, error_code);
}

void seabreeze_set_tec_temperature(int index, int *error_code, double temperature_degrees_celsius) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setTECTemperature(index, error_code, temperature_degrees_celsius);
}

void seabreeze_set_tec_enable(int index, int *error_code, unsigned char tec_enable) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setTECEnable(index, error_code, tec_enable);
}

void seabreeze_set_tec_fan_enable(int index, int *error_code, unsigned char tec_fan_enable) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setTECFanEnable(index, error_code, tec_fan_enable);
}



int seabreeze_get_unformatted_spectrum(int index, int *error_code, unsigned char *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getUnformattedSpectrum(index, error_code, buffer,
            buffer_length);
}

int seabreeze_get_formatted_spectrum(int index, int *error_code, double* buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getFormattedSpectrum(index, error_code, buffer,
            buffer_length);
}

int seabreeze_get_unformatted_spectrum_length(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getUnformattedSpectrumLength(index, error_code);
}

int seabreeze_get_formatted_spectrum_length(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getFormattedSpectrumLength(index, error_code);
}

int seabreeze_get_fast_buffer_spectrum(int index, int *error_code, unsigned char *buffer, int buffer_length, unsigned int numberOfSamplesToRetrieve)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getFastBufferSpectrum(index, error_code, buffer, buffer_length, numberOfSamplesToRetrieve);
}

int seabreeze_get_wavelengths(int index, int *error_code, double *wavelengths, int length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getWavelengths(index, error_code, wavelengths, length);
}



unsigned char seabreeze_get_serial_number_max_length(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getSerialNumberMaximumLength(index, error_code);
}

int seabreeze_get_serial_number(int index, int *error_code, char *buffer, int buffer_length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getSerialNumber(index, error_code, buffer, buffer_length);
}



unsigned short seabreeze_get_number_of_pixels(int index, int *error_code) 
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getNumberOfPixels(index, error_code);
}

int seabreeze_get_electric_dark_pixel_indices(int index, int *error_code, int *indices, int length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getElectricDarkPixelIndices(index, error_code,
        indices, length);
}

int seabreeze_get_optical_dark_pixel_indices(int index, int *error_code, int *indices, int length) 
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getOpticalDarkPixelIndices(index, error_code,
		indices, length);
}

int seabreeze_get_active_pixel_indices(int index, int *error_code, int *indices, int length) 
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getActivePixelIndices(index, error_code,
		indices, length);
}



void seabreeze_clear_buffer(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->clearBuffer(index, error_code);
}

void seabreeze_remove_oldest_spectra_from_buffer(int index, int *error_code, unsigned int numberOfSpectra) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->removeOldestSpectraFromBuffer(index, error_code, numberOfSpectra);
}

unsigned long seabreeze_get_buffer_element_count(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getBufferElementCount(index, error_code);
}

unsigned long seabreeze_get_buffer_capacity(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getBufferCapacity(index, error_code);
}

unsigned char seabreeze_get_buffering_enable(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getBufferingEnable(index, error_code);
}

unsigned long seabreeze_get_buffer_capacity_maximum(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getBufferCapacityMaximum(index, error_code);
}

unsigned long seabreeze_get_buffer_capacity_minimum(int index, int *error_code) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getBufferCapacityMinimum(index, error_code);
}

void seabreeze_set_buffer_capacity(int index, int *error_code, unsigned long capacity) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setBufferCapacity(index, error_code, capacity);
}

void seabreeze_set_buffering_enable(int index, int *error_code, unsigned char isEnabled) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    wrapper->setBufferingEnable(index, error_code, isEnabled);
}



void seabreeze_shutdown() 
{
    SeaBreezeWrapper::shutdown();
}

int seabreeze_get_api_version_string(char *buffer, int len) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getAPIVersionString(buffer, len);
}



int seabreeze_get_usb_descriptor_string(int index, int *errorCode, int id, unsigned char *buffer, int len) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->getUSBDescriptorString(index, errorCode, id, buffer, len);
}



void seabreeze_set_continuous_strobe_period_microsec(int index, int *errorCode, unsigned short strobe_id, unsigned long period_usec) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->setContinuousStrobePeriodMicrosec(index, errorCode, strobe_id, period_usec);
}

void seabreeze_set_acquisition_delay_microsec(int index, int *errorCode, unsigned long delay_usec) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->setAcquisitionDelayMicrosec(index, errorCode, delay_usec);
}


int seabreeze_read_usb(int index, int *errorCode, unsigned char endpoint, unsigned char *buffer, unsigned int length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->readUSB(index, errorCode, endpoint, buffer, length);
}

int seabreeze_write_usb(int index, int *errorCode, unsigned char endpoint, unsigned char *buffer, unsigned int length) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->writeUSB(index, errorCode, endpoint, buffer, length);
}


unsigned char seabreeze_get_gpio_number_of_pins(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getGPIO_NumberOfPins(index, error_code);
}

unsigned int seabreeze_get_gpio_output_enable_vector(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getGPIO_OutputEnableVector(index, error_code);
}

void seabreeze_set_gpio_output_enable_vector(int index, int *error_code, unsigned int outputEnableVector, unsigned bitMask)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setGPIO_OutputEnableVector(index, error_code, outputEnableVector, bitMask);
}

unsigned int seabreeze_get_gpio_value_vector(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getGPIO_ValueVector(index, error_code);
}

void seabreeze_set_gpio_value_vector(int index, int *error_code, unsigned int value, unsigned bitMask)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setGPIO_ValueVector(index, error_code, value, bitMask);
}

unsigned char seabreeze_get_egpio_number_of_pins(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getEGPIO_NumberOfPins(index, error_code);
}

unsigned char seabreeze_get_egpio_available_modes(int index, int *error_code, unsigned char pinNumber, unsigned char *availableModes, unsigned char maximumModeCount)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getEGPIO_AvailableModes(index, error_code, pinNumber, availableModes, maximumModeCount);
}

unsigned int seabreeze_get_egpio_current_mode(int index, int *error_code, unsigned char pinNumber)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getEGPIO_CurrentMode(index, error_code, pinNumber);
}

void seabreeze_set_egpio_mode(int index, int *error_code, unsigned char pinNumber, unsigned char mode, float value)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setEGPIO_Mode(index, error_code, pinNumber, mode, value);
}

unsigned int seabreeze_get_egpio_output_vector(int index, int *error_code)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getEGPIO_OutputVector(index, error_code);
}

void seabreeze_set_egpio_output_vector(int index, int *error_code, unsigned int value, unsigned bitMask)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setEGPIO_OutputVector(index, error_code, value, bitMask);
}

float seabreeze_get_egpio_value(int index, int *error_code, unsigned pinNumber)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getEGPIO_Value(index, error_code, pinNumber);
}

void seabreeze_set_egpio_value(int index, int *error_code, unsigned pinNumber, float value)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->setEGPIO_Value(index, error_code, pinNumber, value);
}


unsigned char seabreeze_get_i2c_master_number_of_buses(int index, int *errorCode)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->getI2CMasterNumberOfBuses(index, errorCode);
}

unsigned short seabreeze_read_12c_master_bus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->readI2CMasterBus(index, errorCode, busIndex, slaveAddress, readData, numberOfBytes);
}

unsigned short seabreeae_write_i2c_master_bus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
{
	SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
	return wrapper->writeI2CMasterBus(index, errorCode, busIndex, slaveAddress, writeData, numberOfBytes);
}




void seabreeze_set_verbose(int flag) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->setVerbose(flag != 0);
}

void seabreeze_set_logfile(char *pathname, int len) 
{
    SeaBreezeWrapper *wrapper = SeaBreezeWrapper::getInstance();
    return wrapper->setLogfile(pathname, len);
}
