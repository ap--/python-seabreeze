/***************************************************//**
 * @file    DeviceAdapter.cpp
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a wrapper that allows
 * access to SeaBreeze DeviceInterface instances.
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
#include "api/seabreezeapi/DeviceAdapter.h"  // references device.h
#include "api/seabreezeapi/FeatureFamilies.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"
#include <string>
#include <string.h>

using namespace seabreeze;
using namespace seabreeze::api;
using namespace std;

template <class T>
vector<T *> *__sbapi_getFeatures(Device *dev) {
    /* This is a templated function to get all of the features of a particular
     * type based on the feature interface.  These can be used directly to
     * create feature adapters, but the logic to do that is too much for a
     * simple templated function to deal with.
     */
    vector<Feature *>::iterator iter;
    vector<Feature *> features = dev->getFeatures();
    vector<T *> *retval = new vector<T *>();
    for(iter = features.begin(); iter != features.end(); iter++) {
        T *intf = dynamic_cast<T *>(*iter);
        if(NULL != intf) {
            retval->push_back(intf);
        }
    }
    return retval;
}

DeviceAdapter::DeviceAdapter(Device *dev, unsigned long id) {
    this->device = dev;
    this->instanceID = id;

    if(NULL == this->device) {
        std::string error("Null device is not allowed.");
        throw IllegalArgumentException(error);
    }
}

template <class T> void __delete_feature_adapters(vector<T *> &features) {
    unsigned int i;

    for(i = 0; i < features.size(); i++) {
        delete features[i];
    }
}

DeviceAdapter::~DeviceAdapter() {
    __delete_feature_adapters<RawUSBBusAccessFeatureAdapter>(rawUSBBusAccessFeatures);
    __delete_feature_adapters<SerialNumberFeatureAdapter>(serialNumberFeatures);
    __delete_feature_adapters<SpectrometerFeatureAdapter>(spectrometerFeatures);
    __delete_feature_adapters<ThermoElectricCoolerFeatureAdapter>(tecFeatures);
    __delete_feature_adapters<IrradCalFeatureAdapter>(irradCalFeatures);
	__delete_feature_adapters<EthernetConfigurationFeatureAdapter>(ethernetConfigurationFeatures);
	__delete_feature_adapters<MulticastFeatureAdapter>(multicastFeatures);
	__delete_feature_adapters<WifiConfigurationFeatureAdapter>(wifiConfigurationFeatures);
	__delete_feature_adapters<IPv4FeatureAdapter>(IPv4Features);
	__delete_feature_adapters<DHCPServerFeatureAdapter>(dhcpServerFeatures);
	__delete_feature_adapters<NetworkConfigurationFeatureAdapter>(networkConfigurationFeatures);
    __delete_feature_adapters<EEPROMFeatureAdapter>(eepromFeatures);
    __delete_feature_adapters<StrobeLampFeatureAdapter>(strobeLampFeatures);
    __delete_feature_adapters<ContinuousStrobeFeatureAdapter>(continuousStrobeFeatures);
    __delete_feature_adapters<ShutterFeatureAdapter>(shutterFeatures);
    __delete_feature_adapters<NonlinearityCoeffsFeatureAdapter>(nonlinearityFeatures);
    __delete_feature_adapters<TemperatureFeatureAdapter>(temperatureFeatures);
	__delete_feature_adapters<IntrospectionFeatureAdapter>(introspectionFeatures);
    __delete_feature_adapters<RevisionFeatureAdapter>(revisionFeatures);
    __delete_feature_adapters<OpticalBenchFeatureAdapter>(opticalBenchFeatures);
    __delete_feature_adapters<SpectrumProcessingFeatureAdapter>(spectrumProcessingFeatures);
    __delete_feature_adapters<StrayLightCoeffsFeatureAdapter>(strayLightFeatures);
    __delete_feature_adapters<LightSourceFeatureAdapter>(lightSourceFeatures);
    __delete_feature_adapters<PixelBinningFeatureAdapter>(pixelBinningFeatures);
    __delete_feature_adapters<DataBufferFeatureAdapter>(dataBufferFeatures);
	__delete_feature_adapters<FastBufferFeatureAdapter>(fastBufferFeatures);
    __delete_feature_adapters<AcquisitionDelayFeatureAdapter>(acquisitionDelayFeatures);
	__delete_feature_adapters<gpioFeatureAdapter>(gpioFeatures);
	__delete_feature_adapters<I2CMasterFeatureAdapter>(i2cMasterFeatures);

    delete this->device;
}

template <class T, class U> void __create_feature_adapters(Device *device, vector<U *> &adapters, Bus *bus, const FeatureFamily &family)
{

    unsigned short i;
    vector<Protocol *> protocols;

    /* Purge anything already in the adapters */
    adapters.clear();

    /* Create feature list */
    vector<T *> *features = __sbapi_getFeatures<T>(device);
    for(i = 0; i < features->size(); i++) {
        /* All FeatureInterface instances should also be Features, but
         * do a proper cast to be sure.
         */
        Feature *f = dynamic_cast<Feature *>((*features)[i]);
        if(NULL == f) {
            /* This should never happen if the class is defined properly. */
            continue;
        }
        ProtocolFamily protocolFamily = device->getSupportedProtocol(
                f->getFeatureFamily(), bus->getBusFamily());
        protocols = device->getProtocolsByFamily(protocolFamily);
        if(protocols.size() < 1) {
            /* No supported protocol for this feature on the opened bus. */
            continue;
        }
        adapters.push_back(new U(((*features)[i]), family, (protocols[0]), bus, i));
    }
    delete features;
}


int DeviceAdapter::open(int *errorCode) {
    Bus *bus;
    vector<Protocol *> protocols;
    FeatureFamilies featureFamilies;
    int flag;

    flag = this->device->open();
    if(0 != flag || NULL == this->device->getOpenedBus()) {
        /* Failed to open the device. */
        SET_ERROR_CODE(ERROR_NO_DEVICE);
        return flag;
    }

    bus = this->device->getOpenedBus();

    /* This gives the device a chance to probe the hardware and update its
     * set of Feature instances based on what is detected.
     */
    this->device->initialize(*bus);

    /* Create raw usb access feature list */
    __create_feature_adapters<RawUSBBusAccessFeatureInterface,
    	            RawUSBBusAccessFeatureAdapter>(this->device,
            rawUSBBusAccessFeatures, bus, featureFamilies.RAW_USB_BUS_ACCESS);

    /* Create serial number feature list */
    __create_feature_adapters<SerialNumberFeatureInterface,
                    SerialNumberFeatureAdapter>(this->device,
            serialNumberFeatures, bus, featureFamilies.SERIAL_NUMBER);

    /* Create spectrometer feature list */
    __create_feature_adapters<OOISpectrometerFeatureInterface,
                    SpectrometerFeatureAdapter>(this->device,
            spectrometerFeatures, bus, featureFamilies.SPECTROMETER);

    /* Create TEC feature list */
    __create_feature_adapters<ThermoElectricFeatureInterface,
                    ThermoElectricCoolerFeatureAdapter>(this->device,
            tecFeatures, bus, featureFamilies.THERMOELECTRIC);

    /* Create irradiance calibration feature list */
    __create_feature_adapters<IrradCalFeatureInterface,
                    IrradCalFeatureAdapter>(this->device,
            irradCalFeatures, bus, featureFamilies.IRRAD_CAL);

	/* Create ethernet configuration feature list */
	__create_feature_adapters<EthernetConfigurationFeatureInterface,
		EthernetConfigurationFeatureAdapter>(this->device,
			ethernetConfigurationFeatures, bus, featureFamilies.ETHERNET_CONFIGURATION);

	/* Create multicast configuration feature list */
	__create_feature_adapters<MulticastFeatureInterface,
		MulticastFeatureAdapter>(this->device,
			multicastFeatures, bus, featureFamilies.IPV4_MULTICAST);

	/* Create ipv4 feature list */
	__create_feature_adapters<IPv4FeatureInterface,
		IPv4FeatureAdapter>(this->device,
			IPv4Features, bus, featureFamilies.IPV4_ADDRESS);

	/* Create wifi configuration feature list */
	__create_feature_adapters<WifiConfigurationFeatureInterface,
		WifiConfigurationFeatureAdapter>(this->device,
			wifiConfigurationFeatures, bus, featureFamilies.WIFI_CONFIGURATION);

	/* Create dhcp server feature list */
	__create_feature_adapters<DHCPServerFeatureInterface,
		DHCPServerFeatureAdapter>(this->device,
			dhcpServerFeatures, bus, featureFamilies.DHCP_SERVER);

	/* Create network configuration feature list */
	__create_feature_adapters<NetworkConfigurationFeatureInterface,
		NetworkConfigurationFeatureAdapter>(this->device,
			networkConfigurationFeatures, bus, featureFamilies.NETWORK_CONFIGURATION);

    /* Create EEPROM feature list */
    __create_feature_adapters<EEPROMSlotFeatureInterface,
                    EEPROMFeatureAdapter>(this->device,
            eepromFeatures, bus, featureFamilies.EEPROM);

    /* Create light source feature list */
    __create_feature_adapters<LightSourceFeatureInterface,
                    LightSourceFeatureAdapter>(this->device,
            lightSourceFeatures, bus, featureFamilies.LIGHT_SOURCE);

    /* Create strobe/lamp enable feature list */
    __create_feature_adapters<StrobeLampFeatureInterface,
                    StrobeLampFeatureAdapter>(this->device,
            strobeLampFeatures, bus, featureFamilies.STROBE_LAMP_ENABLE);

    /* Create continuous strobe feature list */
    __create_feature_adapters<ContinuousStrobeFeatureInterface,
                    ContinuousStrobeFeatureAdapter>(this->device,
            continuousStrobeFeatures, bus, featureFamilies.CONTINUOUS_STROBE);

    /* Create shutter feature list */
    __create_feature_adapters<ShutterFeatureInterface,
                    ShutterFeatureAdapter>(this->device,
            shutterFeatures, bus, featureFamilies.SHUTTER);

    /* Create nonlinearity coefficients feature list */
    __create_feature_adapters<NonlinearityCoeffsFeatureInterface,
                    NonlinearityCoeffsFeatureAdapter>(this->device,
            nonlinearityFeatures, bus, featureFamilies.NONLINEARITY_COEFFS);

    /* Create temperature feature list */
    __create_feature_adapters<TemperatureFeatureInterface,
                    TemperatureFeatureAdapter>(this->device,
            temperatureFeatures, bus, featureFamilies.TEMPERATURE);

	/* Create introspection feature list */
	__create_feature_adapters<IntrospectionFeatureInterface,
		IntrospectionFeatureAdapter>(this->device,
			introspectionFeatures, bus, featureFamilies.INTROSPECTION);

    /* Create revision feature list */
    __create_feature_adapters<RevisionFeatureInterface,
                    RevisionFeatureAdapter>(this->device,
            revisionFeatures, bus, featureFamilies.REVISION);

     /* Create optical bench feature list */
    __create_feature_adapters<OpticalBenchFeatureInterface,
                    OpticalBenchFeatureAdapter>(this->device,
            opticalBenchFeatures, bus, featureFamilies.OPTICAL_BENCH);

     /* Create spectrum processing feature list */
    __create_feature_adapters<SpectrumProcessingFeatureInterface,
                    SpectrumProcessingFeatureAdapter>(this->device,
            spectrumProcessingFeatures, bus, featureFamilies.SPECTRUM_PROCESSING);

    /* Create stray light coefficients feature list */
    __create_feature_adapters<StrayLightCoeffsFeatureInterface,
                    StrayLightCoeffsFeatureAdapter>(this->device,
            strayLightFeatures, bus, featureFamilies.STRAY_LIGHT_COEFFS);

    /* Create pixel binning feature list */
    __create_feature_adapters<PixelBinningFeatureInterface,
                    PixelBinningFeatureAdapter>(this->device,
            pixelBinningFeatures, bus, featureFamilies.PIXEL_BINNING);

	/* Create data buffer feature list */
    __create_feature_adapters<DataBufferFeatureInterface,
                    DataBufferFeatureAdapter>(this->device,
            dataBufferFeatures, bus, featureFamilies.DATA_BUFFER);

	/* Create fast buffer feature list */
	__create_feature_adapters<FastBufferFeatureInterface,
		FastBufferFeatureAdapter>(this->device,
			fastBufferFeatures, bus, featureFamilies.FAST_BUFFER);

	/* Create acquisition feature list */
    __create_feature_adapters<AcquisitionDelayFeatureInterface,
                    AcquisitionDelayFeatureAdapter>(this->device,
            acquisitionDelayFeatures, bus, featureFamilies.ACQUISITION_DELAY);

	/* Create gpio feature list */
	__create_feature_adapters<gpioFeatureInterface,
		gpioFeatureAdapter>(this->device,
			gpioFeatures, bus, featureFamilies.GENERAL_PURPOSE_INPUT_OUTPUT);

	/* Create i2c master feature list */
	__create_feature_adapters<i2cMasterFeatureInterface,
		I2CMasterFeatureAdapter>(this->device,
			i2cMasterFeatures, bus, featureFamilies.I2C_MASTER);


    SET_ERROR_CODE(ERROR_SUCCESS);
    return 0;
}

void DeviceAdapter::close() {
    this->device->close();
}

DeviceLocatorInterface *DeviceAdapter::getLocation() {
    return this->device->getLocation();
}

unsigned long DeviceAdapter::getID() {
    return this->instanceID;
}

int DeviceAdapter::getDeviceType(int *errorCode, char *buffer, unsigned int maxLength) {
    if(0 == maxLength || NULL == buffer) {
        SET_ERROR_CODE(ERROR_BAD_USER_BUFFER);
        /* Nothing to do */
        return 0;
    }

    string name = this->device->getName();
    string::iterator iter;

    memset(buffer, (int)0, maxLength);

    unsigned int i = 0;
    for(iter = name.begin(); iter != name.end() && i < maxLength; iter++) {
        if('+' == *iter) {
            strcat(buffer, "PLUS");
            i += 4;
        } else {
            buffer[i] = toupper(*iter);
            i++;
        }
    }

    buffer[maxLength - 1] = '\0'; /* Guarantee a null terminator */

    SET_ERROR_CODE(ERROR_SUCCESS);
    return i;
}


template <class T> int __getFeatureIDs(vector<T *> features, long *buffer, unsigned int max) {
    unsigned int i;

    for(i = 0; i < max && i < features.size(); i++) {
        buffer[i] = features[i]->getID();
    }

    return i;
}

template <class T> T *__getFeatureByID(vector<T *> features, long id) {
    unsigned int i;

    for(i = 0; i < features.size(); i++) {
        if(features[i]->getID() == id) {
            return features[i];
        }
    }

    return NULL;
}


/* returns a USB endpoint from the device as specified by the enumerator endpointType */
/*  if the endpoint type is not supported, a 0 is returned. */
unsigned char DeviceAdapter::getDeviceEndpoint(int *errorCode, usbEndpointType endpointType)
{
    return this->device->getEndpoint(errorCode, endpointType);
}

/* Raw USB Access  feature wrappers */
int DeviceAdapter::getNumberOfRawUSBBusAccessFeatures() {
    return (int) this->rawUSBBusAccessFeatures.size();
}

int DeviceAdapter::getRawUSBBusAccessFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<RawUSBBusAccessFeatureAdapter>(
                rawUSBBusAccessFeatures, buffer, maxFeatures);
}

RawUSBBusAccessFeatureAdapter *DeviceAdapter::getRawUSBBusAccessFeatureByID(long featureID) {
    return __getFeatureByID<RawUSBBusAccessFeatureAdapter>(
                rawUSBBusAccessFeatures, featureID);
}


int DeviceAdapter::rawUSBBusAccessRead(long featureID,
        int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint) {
    RawUSBBusAccessFeatureAdapter *feature = getRawUSBBusAccessFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readUSB(errorCode, buffer, bufferLength, endpoint);
}

int DeviceAdapter::rawUSBBusAccessWrite(long featureID,
        int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint) {
    RawUSBBusAccessFeatureAdapter *feature = getRawUSBBusAccessFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->writeUSB(errorCode, buffer, bufferLength, endpoint);
}


/* Serial number feature wrappers */
int DeviceAdapter::getNumberOfSerialNumberFeatures() {
    return (int) this->serialNumberFeatures.size();
}

int DeviceAdapter::getSerialNumberFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<SerialNumberFeatureAdapter>(
                serialNumberFeatures, buffer, maxFeatures);
}

SerialNumberFeatureAdapter *DeviceAdapter::getSerialNumberFeatureByID(long featureID) {
    return __getFeatureByID<SerialNumberFeatureAdapter>(
                serialNumberFeatures, featureID);
}

int DeviceAdapter::getSerialNumber(long featureID, int *errorCode,
        char *buffer, int bufferLength) {
    SerialNumberFeatureAdapter *feature = getSerialNumberFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getSerialNumber(errorCode, buffer, bufferLength);
}

unsigned char DeviceAdapter::getSerialNumberMaximumLength(long featureID, int *errorCode) {
    SerialNumberFeatureAdapter *feature = getSerialNumberFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getSerialNumberMaximumLength(errorCode);
}

/* Spectrometer feature wrappers */
int DeviceAdapter::getNumberOfSpectrometerFeatures() {
    return (int) this->spectrometerFeatures.size();
}

int DeviceAdapter::getSpectrometerFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<SpectrometerFeatureAdapter>(
                spectrometerFeatures, buffer, maxFeatures);
}

SpectrometerFeatureAdapter *DeviceAdapter::getSpectrometerFeatureByID(long featureID) {
    return __getFeatureByID<SpectrometerFeatureAdapter>(
                spectrometerFeatures, featureID);
}

void DeviceAdapter::spectrometerSetTriggerMode(long featureID,
            int *errorCode, int mode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setTriggerMode(errorCode, mode);
}

void DeviceAdapter::spectrometerSetIntegrationTimeMicros(
            long featureID, int *errorCode, unsigned long integrationTimeMicros) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setIntegrationTimeMicros(errorCode, integrationTimeMicros);
}

unsigned long DeviceAdapter::spectrometerGetMinimumIntegrationTimeMicros(
            long featureID, int *errorCode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getMinimumIntegrationTimeMicros(errorCode);
}

unsigned long DeviceAdapter::spectrometerGetMaximumIntegrationTimeMicros(
            long featureID, int *errorCode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getMaximumIntegrationTimeMicros(errorCode);
}

double DeviceAdapter::spectrometerGetMaximumIntensity(
            long featureID, int *errorCode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getMaximumIntensity(errorCode);
}

int DeviceAdapter::spectrometerGetUnformattedSpectrumLength(
        long featureID, int *errorCode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getUnformattedSpectrumLength(errorCode);
}

int DeviceAdapter::spectrometerGetUnformattedSpectrum(long featureID,
        int *errorCode, unsigned char *buffer, int bufferLength) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getUnformattedSpectrum(errorCode, buffer, bufferLength);
}

int DeviceAdapter::spectrometerGetFastBufferSpectrum(long featureID,
	int *errorCode, unsigned char *buffer, int bufferLength, unsigned int numberOfSamplesToRetrieve) {
	SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getFastBufferSpectrum(errorCode, buffer, bufferLength, numberOfSamplesToRetrieve);
}

int DeviceAdapter::spectrometerGetFormattedSpectrumLength(
        long featureID, int *errorCode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getFormattedSpectrumLength(errorCode);
}

int DeviceAdapter::spectrometerGetFormattedSpectrum(long featureID, int *errorCode,
        double *buffer, int bufferLength) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getFormattedSpectrum(errorCode, buffer, bufferLength);
}

int DeviceAdapter::spectrometerGetWavelengths(long featureID, int *errorCode,
        double *wavelengths, int length) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getWavelengths(errorCode, wavelengths, length);
}

int DeviceAdapter::spectrometerGetElectricDarkPixelCount(
                    long featureID, int *errorCode) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getElectricDarkPixelCount(errorCode);
}

int DeviceAdapter::spectrometerGetElectricDarkPixelIndices(
                    long featureID, int *errorCode, int *indices, int length) {
    SpectrometerFeatureAdapter *feature = getSpectrometerFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getElectricDarkPixelIndices(errorCode, indices, length);
}



/* Pixel binning feature wrappers */
int DeviceAdapter::getNumberOfPixelBinningFeatures() {
    return (int) pixelBinningFeatures.size();
}

int DeviceAdapter::getPixelBinningFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<PixelBinningFeatureAdapter>(
                pixelBinningFeatures, buffer, maxFeatures);
}

void DeviceAdapter::binningSetPixelBinningFactor(long featureID, int *errorCode, const unsigned char binningFactor) {
    PixelBinningFeatureAdapter *feature = getPixelBinningFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
    }

    feature->setPixelBinningFactor(errorCode, binningFactor);
}

unsigned char DeviceAdapter::binningGetPixelBinningFactor(long featureID, int *errorCode) {
    PixelBinningFeatureAdapter *feature = getPixelBinningFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
    }

    return feature->getPixelBinningFactor(errorCode);
}

void DeviceAdapter::binningSetDefaultPixelBinningFactor(long featureID, int *errorCode, const unsigned char binningFactor) {
    PixelBinningFeatureAdapter *feature = getPixelBinningFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
    }

    feature->setDefaultPixelBinningFactor(errorCode, binningFactor);
}

void DeviceAdapter::binningSetDefaultPixelBinningFactor(long featureID, int *errorCode) {
    PixelBinningFeatureAdapter *feature = getPixelBinningFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
    }

    feature->setDefaultPixelBinningFactor(errorCode);
}

unsigned char DeviceAdapter::binningGetDefaultPixelBinningFactor(long featureID, int *errorCode) {
    PixelBinningFeatureAdapter *feature = getPixelBinningFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
    }

    return feature->getDefaultPixelBinningFactor(errorCode);
}

unsigned char DeviceAdapter::binningGetMaxPixelBinningFactor(long featureID, int *errorCode) {
    PixelBinningFeatureAdapter *feature = getPixelBinningFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
    }

    return feature->getMaxPixelBinningFactor(errorCode);
}

/* TEC feature wrappers */
int DeviceAdapter::getNumberOfThermoElectricFeatures() {
    return (int) this->tecFeatures.size();
}

int DeviceAdapter::getThermoElectricFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<ThermoElectricCoolerFeatureAdapter>(
                tecFeatures, buffer, maxFeatures);
}

ThermoElectricCoolerFeatureAdapter *DeviceAdapter::getTECFeatureByID(long featureID) {
    return __getFeatureByID<ThermoElectricCoolerFeatureAdapter>(
                tecFeatures, featureID);
}

double DeviceAdapter::tecReadTemperatureDegreesC(long featureID, int *errorCode) {
    ThermoElectricCoolerFeatureAdapter *feature = getTECFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readTECTemperature(errorCode);
}

void DeviceAdapter::tecSetTemperatureSetpointDegreesC(long featureID, int *errorCode,
        double temperatureDegreesCelsius) {
    ThermoElectricCoolerFeatureAdapter *feature = getTECFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    return feature->setTECTemperature(errorCode, temperatureDegreesCelsius);
}

void DeviceAdapter::tecSetEnable(long featureID, int *errorCode, bool tecEnable) {
    ThermoElectricCoolerFeatureAdapter *feature = getTECFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    return feature->setTECEnable(errorCode, tecEnable);
}

/* Irradiance calibration feature wrappers */
int DeviceAdapter::getNumberOfIrradCalFeatures() {
    return (int) this->irradCalFeatures.size();
}

int DeviceAdapter::getIrradCalFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<IrradCalFeatureAdapter>(
                irradCalFeatures, buffer, maxFeatures);
}

IrradCalFeatureAdapter *DeviceAdapter::getIrradCalFeatureByID(long featureID) {
    return __getFeatureByID<IrradCalFeatureAdapter>(
                irradCalFeatures, featureID);
}

int DeviceAdapter::irradCalibrationRead(long featureID,
        int *errorCode, float *buffer, int bufferLength) {
    IrradCalFeatureAdapter *feature = getIrradCalFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readIrradCalibration(errorCode, buffer, bufferLength);
}

int DeviceAdapter::irradCalibrationWrite(long featureID,
        int *errorCode, float *buffer, int bufferLength) {
    IrradCalFeatureAdapter *feature = getIrradCalFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->writeIrradCalibration(errorCode, buffer, bufferLength);
}

int DeviceAdapter::irradCalibrationHasCollectionArea(long featureID, int *errorCode) {
    IrradCalFeatureAdapter *feature = getIrradCalFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->hasIrradCollectionArea(errorCode);
}

float DeviceAdapter::irradCalibrationReadCollectionArea(long featureID, int *errorCode) {
    IrradCalFeatureAdapter *feature = getIrradCalFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readIrradCollectionArea(errorCode);
}

void DeviceAdapter::irradCalibrationWriteCollectionArea(long featureID,
        int *errorCode, float area) {
    IrradCalFeatureAdapter *feature = getIrradCalFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    return feature->writeIrradCollectionArea(errorCode, area);
}

/* Ethernet Configuration feature wrappers */
int DeviceAdapter::getNumberOfEthernetConfigurationFeatures()
{
	return (int) this->ethernetConfigurationFeatures.size();
}

int DeviceAdapter::getEthernetConfigurationFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<EthernetConfigurationFeatureAdapter>(ethernetConfigurationFeatures, buffer, maxFeatures);
}

EthernetConfigurationFeatureAdapter *DeviceAdapter::getEthernetConfigurationFeatureByID(long featureID)
{
	return __getFeatureByID<EthernetConfigurationFeatureAdapter>(ethernetConfigurationFeatures, featureID);
}


void DeviceAdapter::ethernetConfiguration_Get_MAC_Address(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6])
{
	EthernetConfigurationFeatureAdapter *feature = getEthernetConfigurationFeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->get_MAC_Address(errorCode, interfaceIndex, macAddress);
}

void DeviceAdapter::ethernetConfiguration_Set_MAC_Address(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6])
{
	EthernetConfigurationFeatureAdapter *feature = getEthernetConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->set_MAC_Address(errorCode, interfaceIndex, macAddress);
}

unsigned char DeviceAdapter::ethernetConfiguration_Get_GbE_Enable_Status(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	EthernetConfigurationFeatureAdapter *feature = getEthernetConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->get_GbE_Enable_Status(errorCode, interfaceIndex);
}

void DeviceAdapter::ethernetConfiguration_Set_GbE_Enable_Status(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	EthernetConfigurationFeatureAdapter *feature = getEthernetConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->set_GbE_Enable_Status(errorCode, interfaceIndex, enableState);
}





/* gpio feature wrappers */
int DeviceAdapter::getNumberOfGPIOFeatures()
{
	return (int) this->gpioFeatures.size();
}

int DeviceAdapter::getGPIOFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<gpioFeatureAdapter>(gpioFeatures, buffer, maxFeatures);
}

gpioFeatureAdapter *DeviceAdapter::getGPIOFeatureByID(long featureID)
{
	return __getFeatureByID<gpioFeatureAdapter>(gpioFeatures, featureID);
}

unsigned char DeviceAdapter::gpioGetNumberOfPins(long featureID, int *errorCode)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getGPIO_NumberOfPins(errorCode);
}

unsigned int DeviceAdapter::gpioGetOutputEnableVector(long featureID, int *errorCode)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getGPIO_OutputEnableVector(errorCode);
}

void DeviceAdapter::gpioSetOutputEnableVector(long featureID, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setGPIO_OutputEnableVector(errorCode, outputEnableVector, bitMask);
}

unsigned int DeviceAdapter::gpioGetValueVector(long featureID, int *errorCode)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getGPIO_ValueVector(errorCode);
}

void DeviceAdapter::gpioSetValueVector(long featureID, int *errorCode, unsigned int valueVector, unsigned int bitMask)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setGPIO_ValueVector(errorCode, valueVector, bitMask);
}

unsigned char DeviceAdapter::gpioExtensionGetNumberOfPins(long featureID, int *errorCode)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getEGPIO_NumberOfPins(errorCode);
}

unsigned char DeviceAdapter::gpioExtensionGetAvailableModes(long featureID, int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maximumModeCount)
{
	unsigned char arraySize = 0;

	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return arraySize;
	}

	return feature->getEGPIO_AvailableModes(errorCode, pinNumber, availableModes, maximumModeCount);
}

unsigned char DeviceAdapter::gpioExtensionGetCurrentMode(long featureID, int *errorCode, unsigned char pinNumber)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getEGPIO_CurrentMode(errorCode, pinNumber);
}

void DeviceAdapter::gpioExtensionSetMode(long featureID, int *errorCode, unsigned char pinNumber, unsigned char mode, float defaultValue)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setEGPIO_Mode(errorCode, pinNumber, mode, defaultValue);
}

unsigned int DeviceAdapter::gpioExtensionGetOutputVector(long featureID, int *errorCode)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getEGPIO_OutputVector(errorCode);
}

void DeviceAdapter::gpioExtensionSetOutputVector(long featureID, int *errorCode, unsigned int outputVector, unsigned int bitMask)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setEGPIO_OutputVector(errorCode, outputVector, bitMask);
}

float DeviceAdapter::gpioExtensionGetValue(long featureID, int *errorCode, unsigned char pinNumber)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getEGPIO_Value(errorCode, pinNumber);
}

void DeviceAdapter::gpioExtensionSetValue(long featureID, int *errorCode, unsigned char pinNumber, float value)
{
	gpioFeatureAdapter *feature = getGPIOFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setEGPIO_Value(errorCode, pinNumber, value);
}




/* Multicast feature wrappers */
int DeviceAdapter::getNumberOfMulticastFeatures()
{
	return (int) this->multicastFeatures.size();
}

int DeviceAdapter::getMulticastFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<MulticastFeatureAdapter>(multicastFeatures, buffer, maxFeatures);
}

MulticastFeatureAdapter *DeviceAdapter::getMulticastFeatureByID(long featureID)
{
	return __getFeatureByID<MulticastFeatureAdapter>(multicastFeatures, featureID);
}

#if 0
void DeviceAdapter::getMulticastGroupAddress(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(&groupAddress)[4])
{
	MulticastFeatureAdapter *feature = getMulticastFeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	}

	feature->getGroupAddress(errorCode, interfaceIndex, groupAddress);
}

void DeviceAdapter::setMulticastGroupAddress(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char groupAddress[4])
{
	MulticastFeatureAdapter *feature = getMulticastFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setGroupAddress(errorCode, interfaceIndex, groupAddress);
}
#endif
unsigned char DeviceAdapter::getMulticastEnableState(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	MulticastFeatureAdapter *feature = getMulticastFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getEnableState(errorCode, interfaceIndex);
}

void DeviceAdapter::setMulticastEnableState(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	MulticastFeatureAdapter *feature = getMulticastFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setEnableState(errorCode, interfaceIndex, enableState);
}









/* IPv4 feature wrappers */
int DeviceAdapter::getNumberOfIPv4Features()
{
	return (int) this->IPv4Features.size();
}

int DeviceAdapter::getIPv4Features(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<IPv4FeatureAdapter>(IPv4Features, buffer, maxFeatures);
}

IPv4FeatureAdapter *DeviceAdapter::getIPv4FeatureByID(long featureID)
{
	return __getFeatureByID<IPv4FeatureAdapter>(IPv4Features, featureID);
}

unsigned char DeviceAdapter::get_IPv4_DHCP_Enable_State(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->get_IPv4_DHCP_Enable_State(errorCode, interfaceIndex);
}

void DeviceAdapter::set_IPv4_DHCP_Enable_State(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->set_IPv4_DHCP_Enable_State(errorCode, interfaceIndex, enableState);
}

unsigned char DeviceAdapter::get_Number_Of_IPv4_Addresses(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->get_Number_Of_IPv4_Addresses(errorCode, interfaceIndex);
}

void DeviceAdapter::get_IPv4_Default_Gateway(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4])
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	}

	feature->get_IPv4_Default_Gateway(errorCode, interfaceIndex, defaultGatewayAddress);
}

void DeviceAdapter::set_IPv4_Default_Gateway(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4])
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->set_IPv4_Default_Gateway(errorCode, interfaceIndex, defaultGatewayAddress);
}

void DeviceAdapter::get_IPv4_Address(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex,  unsigned char(*IPv4_Address)[4], unsigned char *netMask)
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
	}

	feature->get_IPv4_Address(errorCode, interfaceIndex, addressIndex, IPv4_Address, netMask);
}

void DeviceAdapter::add_IPv4_Address(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->add_IPv4_Address(errorCode, interfaceIndex, IPv4_Address, netMask);
}

void DeviceAdapter::delete_IPv4_Address(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex)
{
	IPv4FeatureAdapter *feature = getIPv4FeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->delete_IPv4_Address(errorCode, interfaceIndex, addressIndex);
}













/* Wifi Configuration feature wrappers */
int DeviceAdapter::getNumberOfWifiConfigurationFeatures()
{
	return (int) this->wifiConfigurationFeatures.size();
}

int DeviceAdapter::getWifiConfigurationFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<WifiConfigurationFeatureAdapter>(wifiConfigurationFeatures, buffer, maxFeatures);
}

WifiConfigurationFeatureAdapter *DeviceAdapter::getWifiConfigurationFeatureByID(long featureID)
{
	return __getFeatureByID<WifiConfigurationFeatureAdapter>(wifiConfigurationFeatures, featureID);
}

unsigned char DeviceAdapter::wifiConfigurationGetMode(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getMode(errorCode, interfaceIndex);
}

void DeviceAdapter::wifiConfigurationSetMode(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char mode)
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setMode(errorCode, interfaceIndex, mode);
}

unsigned char DeviceAdapter::wifiConfigurationGetSecurityType(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getSecurityType(errorCode, interfaceIndex);
}

void DeviceAdapter::wifiConfigurationSetSecurityType(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char securityType)
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setMode(errorCode, interfaceIndex, securityType);
}
unsigned char DeviceAdapter::wifiConfigurationGetSSID(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*ssid)[32])
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getSSID(errorCode, interfaceIndex, ssid);
}

void DeviceAdapter::wifiConfigurationSetSSID(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setSSID(errorCode, interfaceIndex, ssid, length);
}

void DeviceAdapter::wifiConfigurationSetPassPhrase(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength)
{
	WifiConfigurationFeatureAdapter *feature = getWifiConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setPassPhrase(errorCode, interfaceIndex, passPhrase, passPhraseLength);
}

/* DHCP Server feature wrappers */
int DeviceAdapter::getNumberOfDHCPServerFeatures()
{
	return (int) this->dhcpServerFeatures.size();
}

int DeviceAdapter::getDHCPServerFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<DHCPServerFeatureAdapter>(dhcpServerFeatures, buffer, maxFeatures);
}

DHCPServerFeatureAdapter *DeviceAdapter::getDHCPServerFeatureByID(long featureID)
{
	return __getFeatureByID<DHCPServerFeatureAdapter>(dhcpServerFeatures, featureID);
}


void DeviceAdapter::dhcpServerGetAddress(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask)
{
	DHCPServerFeatureAdapter *feature = getDHCPServerFeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->getServerAddress(errorCode, interfaceIndex, serverAddress, netMask);
}

void DeviceAdapter::dhcpServerSetAddress(long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask)
{
	DHCPServerFeatureAdapter *feature = getDHCPServerFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setServerAddress(errorCode, interfaceIndex, serverAddress, netMask);
}

unsigned char DeviceAdapter::dhcpServerGetEnableState(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	DHCPServerFeatureAdapter *feature = getDHCPServerFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getServerEnableState(errorCode, interfaceIndex);
}

void DeviceAdapter::dhcpServerSetEnableState(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	DHCPServerFeatureAdapter *feature = getDHCPServerFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setServerEnableState(errorCode, interfaceIndex, enableState);
}





/* Network Configuration feature wrappers */
int DeviceAdapter::getNumberOfNetworkConfigurationFeatures()
{
	return (int) this->networkConfigurationFeatures.size();
}

int DeviceAdapter::getNetworkConfigurationFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<NetworkConfigurationFeatureAdapter>(networkConfigurationFeatures, buffer, maxFeatures);
}

NetworkConfigurationFeatureAdapter *DeviceAdapter::getNetworkConfigurationFeatureByID(long featureID)
{
	return __getFeatureByID<NetworkConfigurationFeatureAdapter>(networkConfigurationFeatures, featureID);
}

unsigned char DeviceAdapter::getNumberOfNetworkInterfaces(long featureID, int *errorCode)
{
	NetworkConfigurationFeatureAdapter *feature = getNetworkConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getNumberOfNetworkInterfaces(errorCode);
}

unsigned char DeviceAdapter::getNetworkInterfaceConnectionType(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	NetworkConfigurationFeatureAdapter *feature = getNetworkConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getNetworkInterfaceConnectionType(errorCode, interfaceIndex);
}

unsigned char DeviceAdapter::getNetworkInterfaceEnableState(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	NetworkConfigurationFeatureAdapter *feature = getNetworkConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getNetworkInterfaceEnableState(errorCode, interfaceIndex);
}

void DeviceAdapter::setNetworkInterfaceEnableState(long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
{
	NetworkConfigurationFeatureAdapter *feature = getNetworkConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setNetworkInterfaceEnableState(errorCode, interfaceIndex, enableState);
}

unsigned char DeviceAdapter::runNetworkInterfaceSelfTest(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	NetworkConfigurationFeatureAdapter *feature = getNetworkConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->runNetworkInterfaceSelfTest(errorCode, interfaceIndex);
}

void DeviceAdapter::saveNetworkInterfaceConnectionSettings(long featureID, int *errorCode, unsigned char interfaceIndex)
{
	NetworkConfigurationFeatureAdapter *feature = getNetworkConfigurationFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->saveNetworkInterfaceConnectionSettings(errorCode, interfaceIndex);
}


/* EEPROM feature wrappers */
int DeviceAdapter::getNumberOfEEPROMFeatures() {
    return (int) this->eepromFeatures.size();
}

int DeviceAdapter::getEEPROMFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<EEPROMFeatureAdapter>(
                eepromFeatures, buffer, maxFeatures);
}

EEPROMFeatureAdapter *DeviceAdapter::getEEPROMFeatureByID(long featureID) {
    return __getFeatureByID<EEPROMFeatureAdapter>(
                eepromFeatures, featureID);
}

int DeviceAdapter::eepromReadSlot(long featureID, int *errorCode, int slotNumber,
                    unsigned char *buffer, int length) {
    EEPROMFeatureAdapter *feature = getEEPROMFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readEEPROMSlot(errorCode, slotNumber, buffer, length);
}

/* Light source feature adapters */
int DeviceAdapter::getNumberOfLightSourceFeatures() {
    return (int) this->lightSourceFeatures.size();
}

int DeviceAdapter::getLightSourceFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<LightSourceFeatureAdapter>(
                lightSourceFeatures, buffer, maxFeatures);
}

LightSourceFeatureAdapter *DeviceAdapter::getLightSourceFeatureByID(long featureID) {
    return __getFeatureByID<LightSourceFeatureAdapter>(
                lightSourceFeatures, featureID);
}

int DeviceAdapter::lightSourceGetCount(long featureID, int *errorCode) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getLightSourceCount(errorCode);
}

bool DeviceAdapter::lightSourceHasEnable(long featureID, int *errorCode,
            int lightSourceIndex) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return false;
    }

    return feature->hasLightSourceEnable(errorCode, lightSourceIndex);
}

bool DeviceAdapter::lightSourceIsEnabled(long featureID, int *errorCode,
            int lightSourceIndex) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return false;
    }

    return feature->isLightSourceEnabled(errorCode, lightSourceIndex);
}

void DeviceAdapter::lightSourceSetEnable(long featureID, int *errorCode,
            int lightSourceIndex, bool enable) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setLightSourceEnable(errorCode, lightSourceIndex, enable);
}

bool DeviceAdapter::lightSourceHasVariableIntensity(long featureID, int *errorCode,
            int lightSourceIndex) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return false;
    }

    return feature->hasVariableIntensity(errorCode, lightSourceIndex);
}

double DeviceAdapter::lightSourceGetIntensity(long featureID, int *errorCode,
            int lightSourceIndex) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0.0;
    }

    return feature->getLightSourceIntensity(errorCode, lightSourceIndex);
}

void DeviceAdapter::lightSourceSetIntensity(long featureID, int *errorCode,
            int lightSourceIndex, double intensity) {

    LightSourceFeatureAdapter *feature = getLightSourceFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setLightSourceIntensity(errorCode, lightSourceIndex, intensity);
}

/* Strobe/lamp feature adapters */
int DeviceAdapter::getNumberOfStrobeLampFeatures() {
    return (int) this->strobeLampFeatures.size();
}

int DeviceAdapter::getStrobeLampFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<StrobeLampFeatureAdapter>(
                strobeLampFeatures, buffer, maxFeatures);
}

StrobeLampFeatureAdapter *DeviceAdapter::getStrobeLampFeatureByID(long featureID) {
    return __getFeatureByID<StrobeLampFeatureAdapter>(
                strobeLampFeatures, featureID);
}

void DeviceAdapter::lampSetStrobeEnable(long featureID,
            int *errorCode, bool strobeEnable) {
    StrobeLampFeatureAdapter *feature = getStrobeLampFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setStrobeLampEnable(errorCode, strobeEnable);
}

/* Continuous strobe feature adapters */
int DeviceAdapter::getNumberOfContinuousStrobeFeatures() {
    return (int) this->continuousStrobeFeatures.size();
}

int DeviceAdapter::getContinuousStrobeFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<ContinuousStrobeFeatureAdapter>(
                continuousStrobeFeatures, buffer, maxFeatures);
}

ContinuousStrobeFeatureAdapter *DeviceAdapter::getContinuousStrobeFeatureByID(long featureID) {
    return __getFeatureByID<ContinuousStrobeFeatureAdapter>(
                continuousStrobeFeatures, featureID);
}

void DeviceAdapter::continuousStrobeSetPeriodMicroseconds(long featureID,
            int *errorCode, unsigned long period_usec) {
    ContinuousStrobeFeatureAdapter *feature = getContinuousStrobeFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setContinuousStrobePeriodMicroseconds(errorCode, period_usec);
}

void DeviceAdapter::continuousStrobeSetEnable(long featureID,
            int *errorCode, bool enable) {
    ContinuousStrobeFeatureAdapter *feature = getContinuousStrobeFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setContinuousStrobeEnable(errorCode, enable);
}

/* Shutter feature wrappers */
int DeviceAdapter::getNumberOfShutterFeatures() {
    return (int) this->shutterFeatures.size();
}

int DeviceAdapter::getShutterFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<ShutterFeatureAdapter>(
                shutterFeatures, buffer, maxFeatures);
}

ShutterFeatureAdapter *DeviceAdapter::getShutterFeatureByID(long featureID) {
    return __getFeatureByID<ShutterFeatureAdapter>(
                shutterFeatures, featureID);
}

void DeviceAdapter::shutterSetShutterOpen(long featureID,
            int *errorCode, bool opened) {
    ShutterFeatureAdapter *feature = getShutterFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setShutterOpen(errorCode, opened);
}


/* Nonlinearity coefficients feature wrappers */
int DeviceAdapter::getNumberOfNonlinearityCoeffsFeatures() {
    return (int) this->nonlinearityFeatures.size();
}

int DeviceAdapter::getNonlinearityCoeffsFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<NonlinearityCoeffsFeatureAdapter>(
                nonlinearityFeatures, buffer, maxFeatures);
}

NonlinearityCoeffsFeatureAdapter *DeviceAdapter::getNonlinearityCoeffsFeatureByID(long featureID) {
    return __getFeatureByID<NonlinearityCoeffsFeatureAdapter>(
                nonlinearityFeatures, featureID);
}

int DeviceAdapter::nonlinearityCoeffsGet(long featureID, int *errorCode,
        double *buffer, int bufferLength) {
    NonlinearityCoeffsFeatureAdapter *feature = getNonlinearityCoeffsFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readNonlinearityCoeffs(errorCode, buffer, bufferLength);
}

/* Temperature feature wrappers */
int DeviceAdapter::getNumberOfTemperatureFeatures() {
    return (int) this->temperatureFeatures.size();
}

int DeviceAdapter::getTemperatureFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<TemperatureFeatureAdapter>(
                temperatureFeatures, buffer, maxFeatures);
}

TemperatureFeatureAdapter *DeviceAdapter::getTemperatureFeatureByID(long temperatureFeatureID) {
    return __getFeatureByID<TemperatureFeatureAdapter>(
                temperatureFeatures, temperatureFeatureID);
}

unsigned char DeviceAdapter::temperatureCountGet(long temperatureFeatureID, int *errorCode) {
    TemperatureFeatureAdapter *feature = getTemperatureFeatureByID(temperatureFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readTemperatureCount(errorCode);
}

double DeviceAdapter::temperatureGet(long temperatureFeatureID, int *errorCode, int index) {
    TemperatureFeatureAdapter *feature = getTemperatureFeatureByID(temperatureFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readTemperature(errorCode, index);
}

int DeviceAdapter::temperatureGetAll(long temperatureFeatureID, int *errorCode,
        double *buffer, int bufferLength) {
    TemperatureFeatureAdapter *feature = getTemperatureFeatureByID(temperatureFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readAllTemperatures(errorCode, buffer, bufferLength);
}

/* Introspection feature wrappers */
int DeviceAdapter::getNumberOfIntrospectionFeatures() {
	return (int) this->introspectionFeatures.size();
}

int DeviceAdapter::getIntrospectionFeatures(long *buffer, int maxFeatures) {
	return __getFeatureIDs<IntrospectionFeatureAdapter>(
		introspectionFeatures, buffer, maxFeatures);
}

IntrospectionFeatureAdapter *DeviceAdapter::getIntrospectionFeatureByID(long featureID) {
	return __getFeatureByID<IntrospectionFeatureAdapter>(
		introspectionFeatures, featureID);
}

unsigned short DeviceAdapter::introspectionNumberOfPixelsGet(long introspectionFeatureID, int *errorCode)
{
	IntrospectionFeatureAdapter *feature = getIntrospectionFeatureByID(introspectionFeatureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getNumberOfPixels(errorCode);
}


int DeviceAdapter::introspectionActivePixelRangesGet(long introspectionFeatureID, int *errorCode, unsigned int *buffer, int bufferLength)
{
	IntrospectionFeatureAdapter *feature = getIntrospectionFeatureByID(introspectionFeatureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getActivePixelRanges(errorCode, buffer, bufferLength);
}

int DeviceAdapter::introspectionElectricDarkPixelRangesGet(long introspectionFeatureID, int *errorCode, unsigned int *buffer, int bufferLength)
{
	IntrospectionFeatureAdapter *feature = getIntrospectionFeatureByID(introspectionFeatureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getElectricDarkPixelRanges(errorCode, buffer, bufferLength);
}

int DeviceAdapter::introspectionOpticalDarkPixelRangesGet(long introspectionFeatureID, int *errorCode, unsigned int *buffer, int bufferLength)
{
	IntrospectionFeatureAdapter *feature = getIntrospectionFeatureByID(introspectionFeatureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getOpticalDarkPixelRanges(errorCode, buffer, bufferLength);
}


/* Revision feature wrappers */
int DeviceAdapter::getNumberOfRevisionFeatures() {
    return (int) this->revisionFeatures.size();
}

int DeviceAdapter::getRevisionFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<RevisionFeatureAdapter>(
                revisionFeatures, buffer, maxFeatures);
}

RevisionFeatureAdapter *DeviceAdapter::getRevisionFeatureByID(long revisionFeatureID) {
    return __getFeatureByID<RevisionFeatureAdapter>(
                revisionFeatures, revisionFeatureID);
}

unsigned char DeviceAdapter::revisionHardwareGet(long revisionFeatureID, int *errorCode) {
    RevisionFeatureAdapter *feature = getRevisionFeatureByID(revisionFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readHardwareRevision(errorCode);
}

unsigned short int DeviceAdapter::revisionFirmwareGet(long revisionFeatureID, int *errorCode) {
    RevisionFeatureAdapter *feature = getRevisionFeatureByID(revisionFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readFirmwareRevision(errorCode);
}

/* Optical Bench feature wrappers */
int DeviceAdapter::getNumberOfOpticalBenchFeatures() {
    return (int) this->opticalBenchFeatures.size();
}

int DeviceAdapter::getOpticalBenchFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<OpticalBenchFeatureAdapter>(
                opticalBenchFeatures, buffer, maxFeatures);
}

OpticalBenchFeatureAdapter *DeviceAdapter::getOpticalBenchFeatureByID(long opticalBenchFeatureID) {
    return __getFeatureByID<OpticalBenchFeatureAdapter>(
               opticalBenchFeatures, opticalBenchFeatureID);
}

unsigned short int DeviceAdapter::opticalBenchGetFiberDiameterMicrons(long opticalBenchFeatureID, int *errorCode) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(opticalBenchFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readOpticalBenchFiberDiameterMicrons(errorCode);
}

unsigned short int DeviceAdapter::opticalBenchGetSlitWidthMicrons(long opticalBenchFeatureID, int *errorCode) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(opticalBenchFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readOpticalBenchSlitWidthMicrons(errorCode);
}

int DeviceAdapter::opticalBenchGetID(long featureID, int *errorCode,
        char *buffer, int bufferLength) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }
    return feature->readOpticalBenchID(errorCode, buffer, bufferLength);
}

int DeviceAdapter::opticalBenchGetSerialNumber(long featureID, int *errorCode,
        char *buffer, int bufferLength) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readOpticalBenchSerialNumber(errorCode, buffer, bufferLength);
}

int DeviceAdapter::opticalBenchGetCoating(long featureID, int *errorCode,
        char *buffer, int bufferLength) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readOpticalBenchCoating(errorCode, buffer, bufferLength);
}

int DeviceAdapter::opticalBenchGetFilter(long featureID, int *errorCode,
        char *buffer, int bufferLength) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readOpticalBenchFilter(errorCode, buffer, bufferLength);
}

int DeviceAdapter::opticalBenchGetGrating(long featureID, int *errorCode,
        char *buffer, int bufferLength) {
    OpticalBenchFeatureAdapter *feature = getOpticalBenchFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readOpticalBenchGrating(errorCode, buffer, bufferLength);
}

/* Spectrum processing feature wrappers */
int DeviceAdapter::getNumberOfSpectrumProcessingFeatures() {
    return (int) this->spectrumProcessingFeatures.size();
}

int DeviceAdapter::getSpectrumProcessingFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<SpectrumProcessingFeatureAdapter>(
                spectrumProcessingFeatures, buffer, maxFeatures);
}

SpectrumProcessingFeatureAdapter *DeviceAdapter::getSpectrumProcessingFeatureByID(long spectrumProcessingFeatureID) {
    return __getFeatureByID<SpectrumProcessingFeatureAdapter>(
               spectrumProcessingFeatures, spectrumProcessingFeatureID);
}

unsigned short int DeviceAdapter::spectrumProcessingScansToAverageGet(long spectrumProcessingFeatureID, int *errorCode) {
    SpectrumProcessingFeatureAdapter *feature = getSpectrumProcessingFeatureByID(spectrumProcessingFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readSpectrumProcessingScansToAverage(errorCode);
}

unsigned char DeviceAdapter::spectrumProcessingBoxcarWidthGet(long spectrumProcessingFeatureID, int *errorCode) {
    SpectrumProcessingFeatureAdapter *feature = getSpectrumProcessingFeatureByID(spectrumProcessingFeatureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readSpectrumProcessingBoxcarWidth(errorCode);
}

void DeviceAdapter::spectrumProcessingBoxcarWidthSet(long featureID, int *errorCode,
            unsigned char boxcarWidth) {

    SpectrumProcessingFeatureAdapter *feature = getSpectrumProcessingFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->writeSpectrumProcessingBoxcarWidth(errorCode, boxcarWidth);
}

void DeviceAdapter::spectrumProcessingScansToAverageSet(long featureID, int *errorCode,
            unsigned short int scansToAverage) {

    SpectrumProcessingFeatureAdapter *feature = getSpectrumProcessingFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->writeSpectrumProcessingScansToAverage(errorCode, scansToAverage);
}

/* Stray light coefficients feature wrappers */
int DeviceAdapter::getNumberOfStrayLightCoeffsFeatures() {
    return (int) this->strayLightFeatures.size();
}

int DeviceAdapter::getStrayLightCoeffsFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<StrayLightCoeffsFeatureAdapter>(
                strayLightFeatures, buffer, maxFeatures);
}


StrayLightCoeffsFeatureAdapter *DeviceAdapter::getStrayLightCoeffsFeatureByID(long featureID) {
    return __getFeatureByID<StrayLightCoeffsFeatureAdapter>(
                strayLightFeatures, featureID);
}

int DeviceAdapter::strayLightCoeffsGet(long featureID, int *errorCode,
        double *buffer, int bufferLength) {
    StrayLightCoeffsFeatureAdapter *feature = getStrayLightCoeffsFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->readStrayLightCoeffs(errorCode, buffer, bufferLength);
}

PixelBinningFeatureAdapter *DeviceAdapter::getPixelBinningFeatureByID(long featureID) {
    return __getFeatureByID<PixelBinningFeatureAdapter>(pixelBinningFeatures, featureID);
}


/* Data buffer feature wrappers */
int DeviceAdapter::getNumberOfDataBufferFeatures() {
    return (int) this->dataBufferFeatures.size();
}

int DeviceAdapter::getDataBufferFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<DataBufferFeatureAdapter>(
                dataBufferFeatures, buffer, maxFeatures);
}

DataBufferFeatureAdapter *DeviceAdapter::getDataBufferFeatureByID(long featureID) {
    return __getFeatureByID<DataBufferFeatureAdapter>(
                dataBufferFeatures, featureID);
}

void DeviceAdapter::dataBufferClear(long featureID, int *errorCode) {
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->clearBuffer(errorCode);
}

void DeviceAdapter::dataBufferRemoveOldestSpectra(long featureID, int *errorCode, unsigned int numberOfSpectra)
{
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->removeOldestSpectraFromBuffer(errorCode, numberOfSpectra);
}

unsigned long DeviceAdapter::dataBufferGetNumberOfElements(long featureID, int *errorCode) {
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getNumberOfElements(errorCode);
}

unsigned long DeviceAdapter::dataBufferGetBufferCapacity(long featureID, int *errorCode) {
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getBufferCapacity(errorCode);
}


unsigned long DeviceAdapter::dataBufferGetBufferCapacityMaximum(long featureID, int *errorCode) {
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getBufferCapacityMaximum(errorCode);
}

unsigned long DeviceAdapter::dataBufferGetBufferCapacityMinimum(long featureID, int *errorCode) {
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getBufferCapacityMinimum(errorCode);
}

void DeviceAdapter::dataBufferSetBufferCapacity(long featureID, int *errorCode, unsigned long capacity) {
    DataBufferFeatureAdapter *feature = getDataBufferFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setBufferCapacity(errorCode, capacity);
}

/* Fast buffer feature wrappers*/

int DeviceAdapter::getNumberOfFastBufferFeatures() {
	return (int) this->fastBufferFeatures.size();
}

int DeviceAdapter::getFastBufferFeatures(long *buffer, int maxFeatures) {
	return __getFeatureIDs<FastBufferFeatureAdapter>(
		fastBufferFeatures, buffer, maxFeatures);
}

FastBufferFeatureAdapter *DeviceAdapter::getFastBufferFeatureByID(long featureID) {
	return __getFeatureByID<FastBufferFeatureAdapter>(
		fastBufferFeatures, featureID);
}


unsigned char DeviceAdapter::fastBufferGetBufferingEnable(long featureID, int *errorCode) {
	FastBufferFeatureAdapter *feature = getFastBufferFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getBufferingEnable(errorCode);
}

void DeviceAdapter::fastBufferSetBufferingEnable(long featureID, int *errorCode, unsigned char isEnabled) {
	FastBufferFeatureAdapter *feature = getFastBufferFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setBufferingEnable(errorCode, isEnabled);
}

unsigned int DeviceAdapter::fastBufferGetConsecutiveSampleCount(long featureID, int *errorCode) {
	FastBufferFeatureAdapter *feature = getFastBufferFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->getConsecutiveSampleCount(errorCode);
}

void DeviceAdapter::fastBufferSetConsecutiveSampleCount(long featureID, int *errorCode, unsigned int consecutiveSampleCount) {
	FastBufferFeatureAdapter *feature = getFastBufferFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return;
	}

	feature->setConsecutiveSampleCount(errorCode, consecutiveSampleCount);
}

/* Acquisition delay feature wrappers */
int DeviceAdapter::getNumberOfAcquisitionDelayFeatures() {
    return (int) this->acquisitionDelayFeatures.size();
}

int DeviceAdapter::getAcquisitionDelayFeatures(long *buffer, int maxFeatures) {
    return __getFeatureIDs<AcquisitionDelayFeatureAdapter>(
                acquisitionDelayFeatures, buffer, maxFeatures);
}

AcquisitionDelayFeatureAdapter *DeviceAdapter::getAcquisitionDelayFeatureByID(long featureID) {
    return __getFeatureByID<AcquisitionDelayFeatureAdapter>(
                acquisitionDelayFeatures, featureID);
}

void DeviceAdapter::acquisitionDelaySetDelayMicroseconds(long featureID, int *errorCode,
        unsigned long delay_usec) {
    AcquisitionDelayFeatureAdapter *feature = getAcquisitionDelayFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return;
    }

    feature->setAcquisitionDelayMicroseconds(errorCode, delay_usec);
}

unsigned long DeviceAdapter::acquisitionDelayGetDelayMicroseconds(long featureID, int *errorCode) {
    AcquisitionDelayFeatureAdapter *feature = getAcquisitionDelayFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getAcquisitionDelayMicroseconds(errorCode);
}

unsigned long DeviceAdapter::acquisitionDelayGetDelayIncrementMicroseconds(long featureID, int *errorCode) {
    AcquisitionDelayFeatureAdapter *feature = getAcquisitionDelayFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getAcquisitionDelayIncrementMicroseconds(errorCode);
}

unsigned long DeviceAdapter::acquisitionDelayGetDelayMaximumMicroseconds(long featureID, int *errorCode) {
    AcquisitionDelayFeatureAdapter *feature = getAcquisitionDelayFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getAcquisitionDelayMaximumMicroseconds(errorCode);
}

unsigned long DeviceAdapter::acquisitionDelayGetDelayMinimumMicroseconds(long featureID, int *errorCode) {
    AcquisitionDelayFeatureAdapter *feature = getAcquisitionDelayFeatureByID(featureID);
    if(NULL == feature) {
        SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
        return 0;
    }

    return feature->getAcquisitionDelayMinimumMicroseconds(errorCode);
}


/* i2c master feature wrappers */
int DeviceAdapter::getNumberOfI2CMasterFeatures()
{
	return (int) this->i2cMasterFeatures.size();
}

int DeviceAdapter::getI2CMasterFeatures(long *buffer, int maxFeatures)
{
	return __getFeatureIDs<I2CMasterFeatureAdapter>(i2cMasterFeatures, buffer, maxFeatures);
}

I2CMasterFeatureAdapter *DeviceAdapter::getI2CMasterFeatureByID(long featureID)
{
	return __getFeatureByID<I2CMasterFeatureAdapter>(i2cMasterFeatures, featureID);
}

unsigned char DeviceAdapter::i2cMasterGetNumberOfBuses(long featureID, int *errorCode)
{
	I2CMasterFeatureAdapter *feature = getI2CMasterFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->i2cMasterGetNumberOfBuses(errorCode);
}

unsigned short DeviceAdapter::i2cMasterReadBus(long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
{
	I2CMasterFeatureAdapter *feature = getI2CMasterFeatureByID(featureID);
	if (NULL == feature)
	{
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->i2cMasterReadBus(errorCode, busIndex, slaveAddress, readData, numberOfBytes);
}

unsigned short DeviceAdapter::i2cMasterWriteBus(long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
{
	I2CMasterFeatureAdapter *feature = getI2CMasterFeatureByID(featureID);
	if (NULL == feature) {
		SET_ERROR_CODE(ERROR_FEATURE_NOT_FOUND);
		return 0;
	}

	return feature->i2cMasterWriteBus(errorCode, busIndex, slaveAddress, writeData, numberOfBytes);
}
