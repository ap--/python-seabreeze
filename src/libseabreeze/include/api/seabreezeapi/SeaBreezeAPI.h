/***************************************************//**
 * @file    SeaBreezeAPI.h
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * This is an interface to SeaBreeze that allows
 * the user to connect to devices over USB and other buses.
 * This is intended as a usable and extensible API.
 *
 * This provides a C interface to help with linkage.
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

#ifndef SEABREEZEAPI_H
#define SEABREEZEAPI_H

// #include "api/DllDecl.h"
#include "api/USBEndpointTypes.h"

/*!
    @brief  This is an interface to SeaBreeze that allows
            the user to connect to devices over USB and
            other buses.  This is intended as a usable and
            extensible API.

    @note   Detailed method documentation is available in
            the analogous C functions in SeaBreezeAPI.h
*/
class SeaBreezeAPI {
public:
    /**
     * No public constructor.  To use this class in C++,
     * you must first call getInstance().  This provides
     * a singleton: it is the same on every call.
     */
    static SeaBreezeAPI *getInstance();

    /**
     * No public destructor.  Call this to force memory cleanup.
     */
    static void shutdown();

    /**
     * Use the probeDevices() method to force the driver to look for any
     * device that can be found automatically.  If this is not called then
     * such devices will not be available for use.  This should be used when
     * attempting to find USB devices.
     */
    virtual int probeDevices() = 0;

    /**
     * Use the addIPv4DeviceLocation() method to specify that a device may be
     * found on a TCP/IPv4 network on a given port.  Once specified,
     * the typical openDevice() function can be used to access it.
     */
    virtual int addTCPIPv4DeviceLocation(char *deviceTypeName, char *ipAddr, int port) = 0;

    /**
     * Use the addRS232DeviceLocation() method to specify that a device may be
     * found on a particular serial bus with a given baud rate.  Once specified,
     * the typical openDevice() function can be used to access it.
     */
    virtual int addRS232DeviceLocation(char *deviceTypeName, char *deviceBusPath, unsigned int baud) = 0;

    /**
     * This provides the number of devices that have either been probed or
     * manually specified.  Devices are not opened automatically, but this can
     * provide a bound for getDeviceIDs().
     */
    virtual int getNumberOfDeviceIDs() = 0;

    /**
     * This provides a unique ID of each device that is detected or specified.
     * The IDs are copied into the user-provided buffer.  These IDs are weak
     * references: attempting to access a device that no longer exists will cause
     * an error value to be returned but should not cause any instability.
     * The IDs may be entirely random, but a given ID will always refer to the
     * same device for as long as the program is running.  This will return the
     * number of device IDs actually copied into the array or 0 on error.
     */
    virtual int getDeviceIDs(long *ids, unsigned long maxLength) = 0;

    // quick and dirty support for returning supported models...
    virtual int getNumberOfSupportedModels() = 0;
    virtual int getSupportedModelName(int index, int *errorCode, char* buffer, int bufferLength) = 0;

    /**
     * This will attempt to open the bus connection to the device with the given ID.
     * Returns 0 on success, other value on error.
     */
    virtual int openDevice(long id, int *errorCode) = 0;

    /**
     * This will attempt to close the bus connection to the device with the given ID.
     */
    virtual void closeDevice(long id, int *errorCode) = 0;

    /* Get a string that describes the type of device */
    virtual int getDeviceType(long id, int *errorCode, char *buffer, unsigned int length) = 0;

    /* Get the usb endpoint address for a specified type of endpoint */
    virtual unsigned char getDeviceEndpoint(long id, int *error_code, usbEndpointType endpointType) = 0;

    /* Get raw usb access capabilities */
    virtual int getNumberOfRawUSBBusAccessFeatures(long deviceID, int *errorCode) = 0;
    virtual int getRawUSBBusAccessFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int rawUSBBusAccessRead(long deviceID, long featureID, int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint) = 0;
    virtual int rawUSBBusAccessWrite(long deviceID, long featureID, int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint) = 0;

    /* Serial number capabilities */
    virtual int getNumberOfSerialNumberFeatures(long deviceID, int *errorCode) = 0;
    virtual int getSerialNumberFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int getSerialNumber(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength) = 0;
    virtual unsigned char getSerialNumberMaximumLength(long deviceID, long featureID, int *errorCode) = 0;

    /* Spectrometer capabilities */
    virtual int getNumberOfSpectrometerFeatures(long id, int *errorCode) = 0;
    virtual int getSpectrometerFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void spectrometerSetTriggerMode(long deviceID, long spectrometerFeatureID, int *errorCode, int mode) = 0;
    virtual void spectrometerSetIntegrationTimeMicros(long deviceID, long spectrometerFeatureID, int *errorCode, unsigned long integrationTimeMicros) = 0;
    virtual unsigned long spectrometerGetMinimumIntegrationTimeMicros(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual unsigned long spectrometerGetMaximumIntegrationTimeMicros(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual double spectrometerGetMaximumIntensity(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual int spectrometerGetUnformattedSpectrumLength(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual int spectrometerGetUnformattedSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode, unsigned char *buffer, int bufferLength) = 0;
	virtual int spectrometerGetFastBufferSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode, unsigned char *dataBuffer, int dataMaxLength, unsigned int numberOfSampleToRetrieve) = 0; // currently 15 max
	virtual int spectrometerGetFormattedSpectrumLength(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual int spectrometerGetFormattedSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode, double *buffer, int bufferLength) = 0;
    virtual int spectrometerGetWavelengths(long deviceID, long spectrometerFeatureID, int *errorCode, double *wavelengths, int length) = 0;
    virtual int spectrometerGetElectricDarkPixelCount(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual int spectrometerGetElectricDarkPixelIndices(long deviceID, long spectrometerFeatureID, int *errorCode, int *indices, int length) = 0;

    /* Pixel binning capabilities */
    virtual int getNumberOfPixelBinningFeatures(long id, int *errorCode) = 0;
    virtual int getPixelBinningFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void binningSetPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode, const unsigned char binningFactor) = 0;
    virtual unsigned char binningGetPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual void binningSetDefaultPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode, const unsigned char binningFactor) = 0;
    virtual void binningSetDefaultPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual unsigned char binningGetDefaultPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;
    virtual unsigned char binningGetMaxPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode) = 0;

    /* TEC capabilities */
    virtual int getNumberOfThermoElectricFeatures(long deviceID, int *errorCode) = 0;
    virtual int getThermoElectricFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual double tecReadTemperatureDegreesC(long deviceID, long featureID, int *errorCode) = 0;
    virtual void tecSetTemperatureSetpointDegreesC(long deviceID, long featureID, int *errorCode, double temperatureDegreesCelsius) = 0;
    virtual void tecSetEnable(long deviceID, long featureID, int *errorCode, unsigned char tecEnable) = 0;

    /* Irradiance calibration features */
    virtual int getNumberOfIrradCalFeatures(long deviceID, int *errorCode) = 0;
    virtual int getIrradCalFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int irradCalibrationRead(long deviceID, long featureID, int *errorCode, float *buffer, int bufferLength) = 0;
    virtual int irradCalibrationWrite(long deviceID, long featureID, int *errorCode, float *buffer, int bufferLength) = 0;
    virtual int irradCalibrationHasCollectionArea(long deviceID, long featureID, int *errorCode) = 0;
    virtual float irradCalibrationReadCollectionArea(long deviceID, long featureID, int *errorCode) = 0;
    virtual void irradCalibrationWriteCollectionArea(long deviceID, long featureID, int *errorCode, float area) = 0;

    /* Ethernet Configuration features */
    virtual int getNumberOfEthernetConfigurationFeatures(long deviceID, int *errorCode) = 0;
    virtual int getEthernetConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void ethernetConfiguration_Get_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6]) = 0;
    virtual void ethernetConfiguration_Set_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6]) = 0;
    virtual unsigned char ethernetConfiguration_Get_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
    virtual void ethernetConfiguration_Set_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState) = 0;

	/* Multicast features */
	virtual int getNumberOfMulticastFeatures(long deviceID, int *errorCode) = 0;
	virtual int getMulticastFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	//virtual void getMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(&macAddress)[6]) = 0;
	//virtual void setMulticstGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6]) = 0;
	virtual unsigned char getMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void setMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState) = 0;

	// IPv4 features
	virtual int getNumberOfIPv4Features(long deviceID, int *errorCode) = 0;
    virtual int getIPv4Features(long deviceID, int *errorCode, long *buffer, int maxLength) = 0;
	virtual unsigned char get_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void   set_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char isEnabled) = 0;
	virtual unsigned char get_Number_Of_IPv4_Addresses(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void   get_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask) = 0;
	virtual void   get_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4]) = 0;
	virtual void   set_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4]) = 0;
	virtual void   add_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask) = 0;
	virtual void   delete_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex) = 0;

	/* DHCP server features */
	virtual int getNumberOfDHCPServerFeatures(long deviceID, int *errorCode) = 0;
	virtual int getDHCPServerFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual void dhcpServerGetAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask) = 0;
	virtual void dhcpServerSetAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask) = 0;
	virtual unsigned char dhcpServerGetEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void dhcpServerSetEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState) = 0;

	/* Network Configuration features */
	virtual int getNumberOfNetworkConfigurationFeatures(long deviceID, int *errorCode) = 0;
	virtual int getNetworkConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual unsigned char getNumberOfNetworkInterfaces(long deviceID, long featureID, int *errorCode) = 0;
	virtual unsigned char getNetworkInterfaceConnectionType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual unsigned char getNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual unsigned char runNetworkInterfaceSelfTest(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void setNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState) = 0;
	virtual void saveNetworkInterfaceConnectionSettings(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;

	// wifi configuration features
	virtual int getNumberOfWifiConfigurationFeatures(long deviceID, int *errorCode) = 0;
    virtual int getWifiConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual unsigned char getWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void   setWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char mode) = 0;
	virtual unsigned char getWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex) = 0;
	virtual void   setWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char securityType) = 0;
	virtual unsigned char   getWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*ssid)[32]) = 0;
	virtual void   setWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length) = 0;
	virtual void   setWifiConfigurationPassPhrase(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength) = 0;

	// gpio features
	virtual int getNumberOfGPIOFeatures(long deviceID, int *errorCode) = 0;
	virtual int getGPIOFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual unsigned char getGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode) = 0;
	virtual unsigned int getGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode) = 0;
	virtual void setGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask) = 0;
	virtual unsigned int getGPIO_ValueVector(long deviceID, long featureID, int *errorCode) = 0;
	virtual void setGPIO_ValueVector(long deviceID, long featureID, int *errorCode, unsigned int valueVector, unsigned int bitMask) = 0;
	virtual unsigned char getEGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode) = 0;
	virtual unsigned char getEGPIO_AvailableModes(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maxModeCount) = 0;
	virtual unsigned char getEGPIO_CurrentMode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber) = 0;
	virtual void setEGPIO_Mode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char mode, float value) = 0;
	virtual unsigned int getEGPIO_OutputVector(long deviceID, long featureID, int *errorCode) = 0;
	virtual void setEGPIO_OutputVector(long deviceID, long featureID, int *errorCode, unsigned int outputVector, unsigned int bitMask) = 0;
	virtual float getEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber) = 0;
	virtual void setEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, float value) = 0;

    /* EEPROM capabilities */
    virtual int getNumberOfEEPROMFeatures(long deviceID, int *errorCode) = 0;
    virtual int getEEPROMFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int eepromReadSlot(long deviceID, long featureID, int *errorCode, int slotNumber, unsigned char *buffer, int bufferLength) = 0;

    /* Light source capabilities */
    virtual int getNumberOfLightSourceFeatures(long deviceID, int *errorCode) = 0;
    virtual int getLightSourceFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int lightSourceGetCount(long deviceID, long featureID, int *errorCode) = 0;
    virtual bool lightSourceHasEnable(long deviceID, long featureID, int *errorCode, int lightSourceIndex) = 0;
    virtual bool lightSourceIsEnabled(long deviceID, long featureID, int *errorCode, int lightSourceIndex) = 0;
    virtual void lightSourceSetEnable(long deviceID, long featureID, int *errorCode, int lightSourceIndex, bool enable) = 0;
    virtual bool lightSourceHasVariableIntensity(long deviceID, long featureID, int *errorCode, int lightSourceIndex) = 0;
    virtual double lightSourceGetIntensity(long deviceID, long featureID, int *errorCode, int lightSourceIndex) = 0;
    virtual void lightSourceSetIntensity(long deviceID, long featureID, int *errorCode, int lightSourceIndex, double intensity) = 0;

    /* Lamp capabilities */
    virtual int getNumberOfLampFeatures(long deviceID, int *errorCode) = 0;
    virtual int getLampFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void lampSetLampEnable(long deviceID, long featureID, int *errorCode, bool strobeEnable) = 0;

    /* Continuous strobe capabilities */
    virtual int getNumberOfContinuousStrobeFeatures(long deviceID, int *errorCode) = 0;
    virtual int getContinuousStrobeFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void continuousStrobeSetContinuousStrobeEnable(long deviceID, long featureID, int *errorCode, bool strobeEnable) = 0;
    virtual void continuousStrobeSetContinuousStrobePeriodMicroseconds(long deviceID, long featureID, int *errorCode, unsigned long strobePeriodMicroseconds) = 0;

    /* Shutter capabilities */
    virtual int getNumberOfShutterFeatures(long deviceID, int *errorCode) = 0;
    virtual int getShutterFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void shutterSetShutterOpen(long deviceID, long featureID, int *errorCode, bool opened) = 0;

    /* Nonlinearity coefficient capabilities */
    virtual int getNumberOfNonlinearityCoeffsFeatures(long deviceID, int *errorCode) = 0;
    virtual int getNonlinearityCoeffsFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int nonlinearityCoeffsGet(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength) = 0;

    /* Temperature capabilities */
    virtual int getNumberOfTemperatureFeatures(long deviceID, int *errorCode) = 0;
    virtual int getTemperatureFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual unsigned char temperatureCountGet(long deviceID, long featureID, int *errorCode) = 0;
    virtual double temperatureGet(long deviceID, long featureID, int *errorCode, int index) = 0;
    virtual int temperatureGetAll(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength) = 0;

	/* Introspection capabilities */
	virtual int getNumberOfIntrospectionFeatures(long deviceID, int *errorCode) = 0;
	virtual int getIntrospectionFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual unsigned short int introspectionNumberOfPixelsGet(long deviceID, long featureID, int *errorCode) = 0;
	virtual int introspectionActivePixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength) = 0;
	virtual int introspectionOpticalDarkPixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength) = 0;
	virtual int introspectionElectricDarkPixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength) = 0;


    /* Spectrum processing capabilities */
    virtual int getNumberOfSpectrumProcessingFeatures(long deviceID, int *errorCode) = 0;
    virtual int getSpectrumProcessingFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual unsigned char spectrumProcessingBoxcarWidthGet(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned short int spectrumProcessingScansToAverageGet(long deviceID, long featureID, int *errorCode) = 0;
    virtual void spectrumProcessingBoxcarWidthSet(long deviceID, long featureID, int *errorCode, unsigned char boxcarWidth) = 0;
    virtual void spectrumProcessingScansToAverageSet(long deviceID, long featureID, int *errorCode, unsigned short int scansToAverage) = 0;

    /* Revision capabilities */
    virtual int getNumberOfRevisionFeatures(long deviceID, int *errorCode) = 0;
    virtual int getRevisionFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual unsigned char revisionHardwareGet(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned short int revisionFirmwareGet(long deviceID, long featureID, int *errorCode) = 0;

    /* Optical Bench capabilities */
    virtual int getNumberOfOpticalBenchFeatures(long deviceID, int *errorCode) = 0;
    virtual int getOpticalBenchFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual unsigned short int opticalBenchGetFiberDiameterMicrons(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned short int opticalBenchGetSlitWidthMicrons(long deviceID, long featureID, int *errorCode) = 0;
    virtual int opticalBenchGetID(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength) = 0;
    virtual int opticalBenchGetSerialNumber(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength) = 0;
    virtual int opticalBenchGetCoating(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength) = 0;
    virtual int opticalBenchGetFilter(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength) = 0;
    virtual int opticalBenchGetGrating(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength) = 0;

    /* Stray light coefficient capabilities */
    virtual int getNumberOfStrayLightCoeffsFeatures(long deviceID, int *errorCode) = 0;
    virtual int getStrayLightCoeffsFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual int strayLightCoeffsGet(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength) = 0;

    /* Data buffer capabilities */
    virtual int getNumberOfDataBufferFeatures(long deviceID, int *errorCode) = 0;
    virtual int getDataBufferFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void dataBufferClear(long deviceID, long featureID, int *errorCode) = 0;
    virtual void dataBufferRemoveOldestSpectra(long deviceID, long featureID, int *errorCode, unsigned int numberOfSpectra) = 0;
    virtual unsigned long dataBufferGetNumberOfElements(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned long dataBufferGetBufferCapacity(long deviceID, long featureID, int *errorCode) = 0;
	virtual unsigned long dataBufferGetBufferCapacityMaximum(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned long dataBufferGetBufferCapacityMinimum(long deviceID, long featureID, int *errorCode) = 0;
    virtual void dataBufferSetBufferCapacity(long deviceID, long featureID, int *errorCode, unsigned long capacity) = 0;

	/* Fast Buffer capabilities*/
	virtual int getNumberOfFastBufferFeatures(long deviceID, int *errorCode) = 0;
	virtual int getFastBufferFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual unsigned char fastBufferGetBufferingEnable(long deviceID, long featureID, int *errorCode) = 0;
	virtual void fastBufferSetBufferingEnable(long deviceID, long featureID, int *errorCode, unsigned char isEnabled) = 0;
	virtual unsigned int fastBufferGetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode) = 0;
	virtual void fastBufferSetConsecutiveSampleCount (long deviceID, long featureID, int *errorCode, unsigned int consecutiveSampleCount) = 0;

    /* Acquisition delay capabilities */
    virtual int getNumberOfAcquisitionDelayFeatures(long deviceID, int *errorCode) = 0;
    virtual int getAcquisitionDelayFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
    virtual void acquisitionDelaySetDelayMicroseconds(long deviceID, long featureID, int *errorCode, unsigned long delay_usec) = 0;
    virtual unsigned long acquisitionDelayGetDelayMicroseconds(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned long acquisitionDelayGetDelayIncrementMicroseconds(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned long acquisitionDelayGetDelayMaximumMicroseconds(long deviceID, long featureID, int *errorCode) = 0;
    virtual unsigned long acquisitionDelayGetDelayMinimumMicroseconds(long deviceID, long featureID, int *errorCode) = 0;

	// i2c master features
	virtual int getNumberOfI2CMasterFeatures(long deviceID, int *errorCode) = 0;
	virtual int getI2CMasterFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength) = 0;
	virtual unsigned char i2cMasterGetNumberOfBuses(long deviceID, long featureID, int *errorCode) = 0;
	virtual unsigned short i2cMasterReadBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes) = 0;
	virtual unsigned short i2cMasterWriteBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes) = 0;

protected:
    SeaBreezeAPI();
    virtual ~SeaBreezeAPI();

private:
    static SeaBreezeAPI *instance;
};

#endif /* SEABREEZEAPI_H */
