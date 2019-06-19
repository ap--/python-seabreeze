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

// MZ: added "Error" to all error strings to simplify test output analysis
static const char *error_msgs[] = {
    "Success",
    "Error: Undefined error",
    "Error: No device found",
    "Error: Could not close device",
    "Error: Feature not implemented",
    "Error: No such feature on device",
    "Error: Data transfer error",
    "Error: Invalid user buffer provided",
    "Error: Input was out of bounds",
    "Error: Spectrometer was saturated",
    "Error: Value not found",
	"Error: Value not expected",
	"Error: Invalid trigger mode"
};

static int number_error_msgs = sizeof (error_msgs) / sizeof (char *);
#define MESSAGE_BUFFER_SIZE 32
static char __messageBuffer[MESSAGE_BUFFER_SIZE];

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

/**************************************************************************************/
//  C language wrapper for Device control 
/**************************************************************************************/


void sbapi_initialize() {
    /* Force the API to allocate an instance if it has not already. */
    SeaBreezeAPI::getInstance();

    /* Nothing else is required (yet) */
}

void sbapi_shutdown() {
    SeaBreezeAPI::shutdown();
}

int
sbapi_add_TCPIPv4_device_location(char *deviceTypeName, char *ipAddress,
            unsigned int port) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();
    
    return wrapper->addTCPIPv4DeviceLocation(deviceTypeName, ipAddress, port);
}
    
int
sbapi_add_RS232_device_location(char *deviceTypeName, char *deviceBusPath,
            unsigned int baud) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->addRS232DeviceLocation(deviceTypeName, deviceBusPath, baud);
}

int
sbapi_probe_devices() {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->probeDevices();
}

int
sbapi_get_number_of_device_ids() {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfDeviceIDs();
}

int
sbapi_get_device_ids(long *ids, unsigned int max_ids) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceIDs(ids, max_ids);
}

int
sbapi_open_device(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->openDevice(deviceID, error_code);
}

void
sbapi_close_device(long index, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->closeDevice(index, error_code);
}

const char *
sbapi_get_error_string(int error_code) {
	const char *returnMessage;

	if((error_code>-99999) && (error_code<0))
	{ 
	// assume these are system errors, show the error code
		#ifdef _WIN32 // work around Windows not supportings snprintf in c99
		// supported in C++11
		_snprintf(__messageBuffer, MESSAGE_BUFFER_SIZE, "System Error: %d", error_code); 
		#else
		snprintf(__messageBuffer, MESSAGE_BUFFER_SIZE, "System Error: %d", error_code);
		#endif
		
		returnMessage=__messageBuffer;
	}
    else if(error_code >= number_error_msgs)
    {
    	// messages outside of seabreeze and system
        returnMessage=error_msgs[ERROR_INVALID_ERROR];
    }
    else
    	returnMessage=error_msgs[error_code]; // seabreeze errors
    	   
    return(returnMessage);
}

int
sbapi_get_device_type(long deviceID, int *error_code,
            char *buffer, unsigned int length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceType(deviceID, error_code, buffer, length);
}


/**************************************************************************************/
//  C language wrapper to retrieve usb endpoints from the device
/**************************************************************************************/


unsigned char
sbapi_get_device_usb_endpoint_primary_out(long deviceID, int *error_code) 
{
	usbEndpointType ept=kEndpointTypePrimaryOut;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceEndpoint(deviceID, error_code, ept);
}

unsigned char
sbapi_get_device_usb_endpoint_primary_in(long deviceID, int *error_code) 
{
	usbEndpointType ept=kEndpointTypePrimaryIn;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceEndpoint(deviceID, error_code, ept);
}

unsigned char
sbapi_get_device_usb_endpoint_secondary_out(long deviceID, int *error_code) 
{
	usbEndpointType ept=kEndpointTypeSecondaryOut;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceEndpoint(deviceID, error_code, ept);
}

unsigned char
sbapi_get_device_usb_endpoint_secondary_in(long deviceID, int *error_code) 
{
	usbEndpointType ept=kEndpointTypeSecondaryIn;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceEndpoint(deviceID, error_code, ept);
}

unsigned char
sbapi_get_device_usb_endpoint_secondary_in2(long deviceID, int *error_code) 
{
	usbEndpointType ept=kEndpointTypeSecondaryIn2;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDeviceEndpoint(deviceID, error_code, ept);
}


/**************************************************************************************/
//  C language wrapper for raw usb access
/**************************************************************************************/
int
sbapi_get_number_of_raw_usb_bus_access_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfRawUSBBusAccessFeatures(deviceID, error_code);
}

int
sbapi_get_raw_usb_bus_access_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getRawUSBBusAccessFeatures(deviceID, error_code,
            features, max_features);
}

int
sbapi_raw_usb_bus_access_read(long deviceID, long featureID,
        int *error_code, unsigned char *buffer, int buffer_length, unsigned char endpoint) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->rawUSBBusAccessRead(deviceID, featureID, error_code, buffer, buffer_length, endpoint);
}

int
sbapi_raw_usb_bus_access_write(long deviceID, long featureID,
        int *error_code, unsigned char *buffer, int buffer_length, unsigned char endpoint) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->rawUSBBusAccessWrite(deviceID, featureID, error_code, buffer, buffer_length, endpoint);
}


/**************************************************************************************/
//  C language wrapper for Serial Number Features
/**************************************************************************************/

int
sbapi_get_number_of_serial_number_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfSerialNumberFeatures(deviceID, error_code);
}

int
sbapi_get_serial_number_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getSerialNumberFeatures(deviceID, error_code,
            features, max_features);
}

int
sbapi_get_serial_number(long deviceID, long featureID, int *error_code,
        char *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getSerialNumber(deviceID, featureID,
            error_code, buffer, buffer_length);
}

unsigned char
sbapi_get_serial_number_maximum_length(long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getSerialNumberMaximumLength(deviceID, featureID, error_code);
}

/**************************************************************************************/
//  C language wrapper for spectrometer features
/**************************************************************************************/

int
sbapi_get_number_of_spectrometer_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfSpectrometerFeatures(deviceID, error_code);
}

int
sbapi_get_spectrometer_features(long deviceID, int *error_code, long *spectrometerFeatureID,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getSpectrometerFeatures(deviceID, error_code,
            spectrometerFeatureID, max_features);
}

void
sbapi_spectrometer_set_trigger_mode(long deviceID, long spectrometerFeatureID,
        int *error_code, int mode) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->spectrometerSetTriggerMode(deviceID, spectrometerFeatureID, error_code, mode);
}

void
sbapi_spectrometer_set_integration_time_micros(long deviceID, long spectrometerFeatureID,
        int *error_code, unsigned long integration_time_micros) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->spectrometerSetIntegrationTimeMicros(deviceID, spectrometerFeatureID, error_code,
        integration_time_micros);
}

long
sbapi_spectrometer_get_minimum_integration_time_micros(long deviceID,
        long spectrometerFeatureID, int *error_code) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetMinimumIntegrationTimeMicros(deviceID, spectrometerFeatureID,
            error_code);
}

double
sbapi_spectrometer_get_maximum_intensity(long deviceID,
        long spectrometerFeatureID, int *error_code) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetMaximumIntensity(deviceID, spectrometerFeatureID,
            error_code);
}

int sbapi_spectrometer_get_fast_buffer_spectrum(long deviceID,
	long spectrometerFeatureID, int *error_code,
	unsigned char *buffer, int buffer_length, unsigned int numberOfSamplesToRetrieve)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->spectrometerGetFastBufferSpectrum(deviceID,
		spectrometerFeatureID, error_code, buffer, buffer_length, numberOfSamplesToRetrieve);
}

int
sbapi_spectrometer_get_unformatted_spectrum(long deviceID,
        long spectrometerFeatureID, int *error_code,
        unsigned char *buffer, int buffer_length) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetUnformattedSpectrum(deviceID,
            spectrometerFeatureID, error_code, buffer, buffer_length);
}

int
sbapi_spectrometer_get_formatted_spectrum(long deviceID,
        long spectrometerFeatureID, int *error_code,
        double* buffer, int buffer_length) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetFormattedSpectrum(deviceID, spectrometerFeatureID,
            error_code, buffer, buffer_length);
}

int
sbapi_spectrometer_get_unformatted_spectrum_length(long deviceID,
        long spectrometerFeatureID, int *error_code) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetUnformattedSpectrumLength(deviceID,
            spectrometerFeatureID, error_code);
}

int
sbapi_spectrometer_get_formatted_spectrum_length(long deviceID,
        long spectrometerFeatureID, int *error_code) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetFormattedSpectrumLength(deviceID,
            spectrometerFeatureID, error_code);
}

int
sbapi_spectrometer_get_wavelengths(long deviceID,
        long spectrometerFeatureID, int *error_code, double *wavelengths,
        int length) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetWavelengths(deviceID, spectrometerFeatureID,
            error_code, wavelengths, length);
}

int
sbapi_spectrometer_get_electric_dark_pixel_count(long deviceID,
        long spectrometerFeatureID, int *error_code) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetElectricDarkPixelCount(deviceID, spectrometerFeatureID,
            error_code);
}

int
sbapi_spectrometer_get_electric_dark_pixel_indices(long deviceID,
        long spectrometerFeatureID, int *error_code, int *indices, int length) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrometerGetElectricDarkPixelIndices(deviceID, spectrometerFeatureID,
            error_code, indices, length);
}

/**************************************************************************************/
//  C language wrapper for pixel binning features
/**************************************************************************************/

int sbapi_get_number_of_pixel_binning_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfPixelBinningFeatures(deviceID, error_code);
}

int sbapi_get_pixel_binning_features(long deviceID, int *error_code, long *features,
            int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getPixelBinningFeatures(deviceID, error_code, features, max_features);
}

void sbapi_binning_set_pixel_binning_factor(long deviceID, long featureID,
            int *error_code, unsigned char factor) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->binningSetPixelBinningFactor(deviceID, featureID, error_code, factor);
}

unsigned char sbapi_binning_get_pixel_binning_factor(long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->binningGetPixelBinningFactor(deviceID, featureID, error_code);
}

void sbapi_binning_set_default_pixel_binning_factor(long deviceID, long featureID,
            int *error_code, unsigned char factor) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->binningSetDefaultPixelBinningFactor(deviceID, featureID, error_code, factor);
}

void sbapi_binning_reset_default_pixel_binning_factor(long deviceID, long featureID,
            int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->binningSetDefaultPixelBinningFactor(deviceID, featureID, error_code);
}

unsigned char sbapi_binning_get_default_pixel_binning_factor(long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->binningGetDefaultPixelBinningFactor(deviceID, featureID, error_code);
}

unsigned char sbapi_binning_get_max_pixel_binning_factor(long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->binningGetMaxPixelBinningFactor(deviceID, featureID, error_code);
}

/**************************************************************************************/
//  C language wrapper for shutter features
/**************************************************************************************/

int
sbapi_get_number_of_shutter_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfShutterFeatures(deviceID, error_code);
}

int
sbapi_get_shutter_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getShutterFeatures(deviceID, error_code,
            features, max_features);
}

void
sbapi_shutter_set_shutter_open(long deviceID, long featureID,
        int *error_code, unsigned char opened) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->shutterSetShutterOpen(deviceID, featureID, error_code, opened != 0);
}

/**************************************************************************************/
//  C language wrapper for light source features
/**************************************************************************************/

int
sbapi_get_number_of_light_source_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfLightSourceFeatures(deviceID, error_code);
}

int
sbapi_get_light_source_features(long deviceID, int *error_code,
        long *features, int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getLightSourceFeatures(deviceID, error_code,
            features, max_features);
}

int
sbapi_light_source_get_count(long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->lightSourceGetCount(deviceID, featureID, error_code);
}

unsigned char
sbapi_light_source_has_enable(long deviceID, long featureID, int *error_code,
        int light_source_index) {
    bool retval;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    retval = wrapper->lightSourceHasEnable(deviceID, featureID, error_code,
            light_source_index);
    return (retval == false ? 0 : 1);
}

unsigned char
sbapi_light_source_is_enabled(long deviceID, long featureID, int *error_code,
        int light_source_index) {
    bool retval;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    retval = wrapper->lightSourceIsEnabled(deviceID, featureID, error_code,
            light_source_index);
    return (false == retval ? 0 : 1);
}

void
sbapi_light_source_set_enable(long deviceID, long featureID, int *error_code,
        int light_source_index, unsigned char enable) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->lightSourceSetEnable(deviceID, featureID, error_code,
            light_source_index, (0 == enable ? false : true));
}

unsigned char
sbapi_light_source_has_variable_intensity(long deviceID, long featureID,
        int *error_code, int light_source_index) {
    bool retval;
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    retval = wrapper->lightSourceHasVariableIntensity(deviceID, featureID,
            error_code, light_source_index);
    return (retval == false ? 0 : 1);
}

double
sbapi_light_source_get_intensity(long deviceID, long featureID,
        int *error_code, int light_source_index) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->lightSourceGetIntensity(deviceID, featureID, error_code,
        light_source_index);
}

void
sbapi_light_source_set_intensity(long deviceID, long featureID,
        int *error_code, int light_source_index, double intensity) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->lightSourceSetIntensity(deviceID, featureID, error_code,
        light_source_index, intensity);
}

/**************************************************************************************/
//  C language wrapper for lamp features
/**************************************************************************************/

int
sbapi_get_number_of_lamp_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfLampFeatures(deviceID, error_code);
}

int
sbapi_get_lamp_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getLampFeatures(deviceID, error_code,
            features, max_features);
}

void
sbapi_lamp_set_lamp_enable(long deviceID, long featureID,
        int *error_code, unsigned char lamp_enable) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->lampSetLampEnable(deviceID, featureID, error_code, lamp_enable != 0);
}

/**************************************************************************************/
//  C language wrapper for continuous strobe features
/**************************************************************************************/

int
sbapi_get_number_of_continuous_strobe_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfContinuousStrobeFeatures(deviceID, error_code);
}

int
sbapi_get_continuous_strobe_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getContinuousStrobeFeatures(deviceID, error_code,
            features, max_features);
}

void
sbapi_continuous_strobe_set_continuous_strobe_enable(long deviceID, long featureID,
        int *error_code, unsigned char lamp_enable) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->continuousStrobeSetContinuousStrobeEnable(deviceID, featureID, error_code,
        lamp_enable != 0);
}

void
sbapi_continuous_strobe_set_continuous_strobe_period_micros(long deviceID, long featureID,
        int *error_code, unsigned long period_usec) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->continuousStrobeSetContinuousStrobePeriodMicroseconds(deviceID, featureID,
        error_code, period_usec);
}

/**************************************************************************************/
//  C language wrapper for eeprom features
/**************************************************************************************/

int
sbapi_get_number_of_eeprom_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfEEPROMFeatures(deviceID, error_code);
}

int
sbapi_get_eeprom_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getEEPROMFeatures(deviceID, error_code,
            features, max_features);
}

int
sbapi_eeprom_read_slot(long deviceID, long featureID,
        int *error_code, int slot_number,
        unsigned char *buffer, int buffer_length) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->eepromReadSlot(deviceID, featureID, error_code, slot_number,
            buffer, buffer_length);
}

/**************************************************************************************/
//  C language wrapper for irradCal features
/**************************************************************************************/

int
sbapi_get_number_of_irrad_cal_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfIrradCalFeatures(deviceID, error_code);
}

int
sbapi_get_irrad_cal_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getIrradCalFeatures(deviceID, error_code,
            features, max_features);
}

int
sbapi_irrad_calibration_read(long deviceID, long featureID,
        int *error_code, float *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->irradCalibrationRead(deviceID, featureID, error_code, buffer,
        buffer_length);
}

int
sbapi_irrad_calibration_write(long deviceID, long featureID,
        int *error_code, float *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->irradCalibrationWrite(deviceID, featureID, error_code, buffer,
                buffer_length);
}

int
sbapi_irrad_calibration_has_collection_area(long deviceID, long featureID,
        int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->irradCalibrationHasCollectionArea(deviceID, featureID, error_code);
}

float
sbapi_irrad_calibration_read_collection_area(long deviceID, long featureID,
        int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->irradCalibrationReadCollectionArea(deviceID, featureID, error_code);
}

void
sbapi_irrad_calibration_write_collection_area(long deviceID, long featureID,
        int *error_code, float area) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->irradCalibrationWriteCollectionArea(deviceID, featureID, error_code, area);
}


/**************************************************************************************/
//  C language wrapper for Ethernet configuration features
/**************************************************************************************/

int sbapi_get_number_of_ethernet_configuration_features(long deviceID, int *error_code) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfEthernetConfigurationFeatures(deviceID, error_code);
}

int sbapi_get_ethernet_configuration_features(long deviceID, int *error_code, long *features, int max_features) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getEthernetConfigurationFeatures(deviceID, error_code, features, max_features);
}

void sbapi_ethernet_configuration_get_mac_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char (*macAddress)[6])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->ethernetConfiguration_Get_MAC_Address(deviceID, featureID, error_code, interfaceIndex, macAddress);
}

void sbapi_ethernet_configuration_set_mac_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char macAddress[6])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->ethernetConfiguration_Set_MAC_Address(deviceID, featureID, error_code, interfaceIndex, macAddress);
}

unsigned char sbapi_ethernet_configuration_get_gbe_enable_status(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->ethernetConfiguration_Get_GbE_Enable_Status(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_ethernet_configuration_set_gbe_enable_status(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char enableStatus)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->ethernetConfiguration_Set_GbE_Enable_Status(deviceID, featureID, error_code, interfaceIndex, enableStatus);
}



/**************************************************************************************/
//  C language wrapper for multicast features
/**************************************************************************************/

int sbapi_get_number_of_multicast_features(long deviceID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfMulticastFeatures(deviceID, error_code);
}

int sbapi_get_multicast_features(long deviceID, int *error_code, long *features, int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getMulticastFeatures(deviceID, error_code, features, max_features);
}

#if 0
void sbapi_multicast_get_group_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char(&groupAddress)[4])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getMulticastGroupAddress(deviceID, featureID, error_code, interfaceIndex, groupAddress);
}

void sbapi_multicast_set_group_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char groupAddress[4])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->setMulticastGroupAddress(deviceID, featureID, error_code, interfaceIndex, groupAddress);
}
#endif
unsigned char sbapi_multicast_get_enable_state(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getMulticastEnableState(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_multicast_set_enable_state(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char enableState)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->setMulticastEnableState(deviceID, featureID, error_code, interfaceIndex, enableState);
}


// currently the multicast group address and port are not accessible. However, these functions return a hard coded value for convenience
unsigned short sbapi_multicast_get_group_port(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	//SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	//return wrapper->getMulticastGroupPort(deviceID, featureID, error_code, interfaceIndex);
	return 57357;
}

void sbapi_multicast_get_group_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char(*groupAddress)[4])
{
	//SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	//wrapper->getMulticastGroupAddress(deviceID, featureID, error_code, interfaceIndex, groupAddress);
	for (int i = 0; i < 4; i++)
		(*groupAddress)[i] = 239;
}


/**************************************************************************************/
//  C language wrapper for IPv4 features
/**************************************************************************************/

int sbapi_get_number_of_ipv4_features(long deviceID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfIPv4Features(deviceID, error_code);
}

int sbapi_get_ipv4_features(long deviceID, int *error_code, long *features, int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getIPv4Features(deviceID, error_code, features, max_features);
}

unsigned char sbapi_ipv4_get_dhcp_enable_state(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->get_IPv4_DHCP_Enable_State(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_ipv4_set_dhcp_enable_state(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char enableStatus)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->set_IPv4_DHCP_Enable_State(deviceID, featureID, error_code, interfaceIndex, enableStatus);
}

unsigned char sbapi_ipv4_get_number_of_addresses(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->get_Number_Of_IPv4_Addresses(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_ipv4_get_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->get_IPv4_Address(deviceID, featureID, error_code, interfaceIndex, addressIndex, IPv4_Address, netMask);
}

void sbapi_ipv4_get_default_gateway_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->get_IPv4_Default_Gateway(deviceID, featureID, error_code, interfaceIndex, defaultGatewayAddress);
}
void sbapi_ipv4_set_default_gateway_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->set_IPv4_Default_Gateway(deviceID, featureID, error_code, interfaceIndex, defaultGatewayAddress);
}

void sbapi_ipv4_add_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->add_IPv4_Address(deviceID, featureID, error_code, interfaceIndex, IPv4_Address, netMask);
}

void sbapi_ipv4_delete_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char addressIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->delete_IPv4_Address(deviceID, featureID, error_code, interfaceIndex, addressIndex);
}


/**************************************************************************************/
//  C language wrapper for dhcp server features
/**************************************************************************************/

int sbapi_get_number_of_dhcp_server_features(long deviceID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfDHCPServerFeatures(deviceID, error_code);
}

int sbapi_get_dhcp_server_features(long deviceID, int *error_code, long *features, int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getDHCPServerFeatures(deviceID, error_code, features, max_features);
}

void sbapi_dhcp_server_get_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->dhcpServerGetAddress(deviceID, featureID, error_code, interfaceIndex, serverAddress, netMask);
}

void sbapi_dhcp_server_set_address(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->dhcpServerSetAddress(deviceID, featureID, error_code, interfaceIndex, serverAddress, netMask);
}

unsigned char sbapi_dhcp_server_get_enable_state(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->dhcpServerGetEnableState(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_dhcp_server_set_enable_state(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char enableStatus)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->dhcpServerSetEnableState(deviceID, featureID, error_code, interfaceIndex, enableStatus);
}


/**************************************************************************************/
//  C language wrapper for Network configuration features
/**************************************************************************************/

int sbapi_get_number_of_network_configuration_features(long deviceID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfNetworkConfigurationFeatures(deviceID, error_code);
}

int sbapi_get_network_configuration_features(long deviceID, int *error_code, long *features, int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNetworkConfigurationFeatures(deviceID, error_code, features, max_features);
}

unsigned char sbapi_network_configuration_get_interface_count(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfNetworkInterfaces(deviceID, featureID, error_code);
}

unsigned char sbapi_network_configuration_get_interface_connection_type(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNetworkInterfaceConnectionType(deviceID, featureID, error_code, interfaceIndex);
}

unsigned char sbapi_network_configuration_run_interface_self_test(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->runNetworkInterfaceSelfTest(deviceID, featureID, error_code, interfaceIndex);
}

unsigned char sbapi_network_configuration_get_interface_enable_status(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNetworkInterfaceEnableState(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_network_configuration_set_interface_enable_status(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char enableStatus)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setNetworkInterfaceEnableState(deviceID, featureID, error_code, interfaceIndex, enableStatus);
}

void sbapi_network_configuration_save_interface_settings(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->saveNetworkInterfaceConnectionSettings(deviceID, featureID, error_code, interfaceIndex);
}


/**************************************************************************************/
//  C language wrapper for Wifi configuration features
/**************************************************************************************/
int sbapi_get_number_of_wifi_configuration_features(long deviceID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfWifiConfigurationFeatures(deviceID, error_code);
}

int sbapi_get_wifi_configuration_features(long deviceID, int *error_code, long *features, int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getWifiConfigurationFeatures(deviceID, error_code, features, max_features);
}

unsigned char sbapi_wifi_configuration_get_mode(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getWifiConfigurationMode(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_wifi_configuration_set_mode(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char wifiMode)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setWifiConfigurationMode(deviceID, featureID, error_code, interfaceIndex, wifiMode);
}

unsigned char sbapi_wifi_configuration_get_security_type(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getWifiConfigurationSecurityType(deviceID, featureID, error_code, interfaceIndex);
}

void sbapi_wifi_configuration_set_security_type(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char securityType)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setWifiConfigurationSecurityType(deviceID, featureID, error_code, interfaceIndex, securityType);
}

unsigned char sbapi_wifi_configuration_get_ssid(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, unsigned char(*ssid)[32])
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getWifiConfigurationSSID(deviceID, featureID, error_code, interfaceIndex, ssid);
}

void sbapi_wifi_configuration_set_ssid(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setWifiConfigurationSSID(deviceID, featureID, error_code, interfaceIndex, ssid, length);
}

void sbapi_wifi_configuration_set_pass_phrase(long deviceID, long featureID, int *error_code, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setWifiConfigurationPassPhrase(deviceID, featureID, error_code, interfaceIndex, passPhrase, passPhraseLength);
}




/**************************************************************************************/
//  C language wrapper for gpio features
/**************************************************************************************/

int sbapi_get_number_of_gpio_features(long deviceID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfGPIOFeatures(deviceID, error_code);
}

int sbapi_get_gpio_features(long deviceID, int *error_code, long *features, int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getGPIOFeatures(deviceID, error_code, features, max_features);
}

unsigned char sbapi_gpio_get_number_of_pins(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getGPIO_NumberOfPins(deviceID, featureID, error_code);
}

unsigned int sbapi_gpio_get_output_enable_vector(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getGPIO_OutputEnableVector(deviceID, featureID, error_code);
}


void sbapi_gpio_set_output_enable_vector(long deviceID, long featureID, int *error_code, unsigned int outputEnableVector, unsigned int bitMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setGPIO_OutputEnableVector(deviceID, featureID, error_code, outputEnableVector, bitMask);
}

unsigned int sbapi_gpio_get_value_vector(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getGPIO_ValueVector(deviceID, featureID, error_code);
}


void sbapi_gpio_set_value_vector(long deviceID, long featureID, int *error_code, unsigned int valueVector, unsigned int bitMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setGPIO_ValueVector(deviceID, featureID, error_code, valueVector, bitMask);
}

unsigned char sbapi_gpio_extension_get_number_of_pins(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getEGPIO_NumberOfPins(deviceID, featureID, error_code);
}

unsigned char sbapi_gpio_extension_get_available_modes(long deviceID, long featureID, int *error_code, unsigned char pinNumber, unsigned char *availableModes, unsigned char maximumModeCount)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getEGPIO_AvailableModes(deviceID, featureID, error_code, pinNumber, availableModes, maximumModeCount);
}

unsigned char sbapi_gpio_extension_get_current_mode(long deviceID, long featureID, int *error_code, unsigned char pinNumber)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getEGPIO_CurrentMode(deviceID, featureID, error_code, pinNumber);
}


void sbapi_gpio_extension_set_mode(long deviceID, long featureID, int *error_code, unsigned char pinNumber, unsigned char mode, float value)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setEGPIO_Mode(deviceID, featureID, error_code, pinNumber, mode, value);
}

unsigned int sbapi_gpio_extension_get_output_vector(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getEGPIO_OutputVector(deviceID, featureID, error_code);
}


void sbapi_gpio_extension_set_output_vector(long deviceID, long featureID, int *error_code, unsigned int outputVector, unsigned int bitMask)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setEGPIO_OutputVector(deviceID, featureID, error_code, outputVector, bitMask);
}

float sbapi_gpio_extension_get_value(long deviceID, long featureID, int *error_code, unsigned char pinNumber)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getEGPIO_Value(deviceID, featureID, error_code, pinNumber);
}


void sbapi_gpio_extension_set_value(long deviceID, long featureID, int *error_code, unsigned char pinNumber, float value)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->setEGPIO_Value(deviceID, featureID, error_code, pinNumber, value);
}

/**************************************************************************************/
//  C language wrapper for thermo-electric features
/**************************************************************************************/

int
sbapi_get_number_of_thermo_electric_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfThermoElectricFeatures(deviceID, error_code);
}

int
sbapi_get_thermo_electric_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getThermoElectricFeatures(deviceID, error_code,
            features, max_features);
}

double
sbapi_tec_read_temperature_degrees_C(long deviceID, long featureID,
        int *error_code) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->tecReadTemperatureDegreesC(deviceID, featureID, error_code);
}

void
sbapi_tec_set_temperature_setpoint_degrees_C(long deviceID, long featureID,
        int *error_code, double temperature_degrees_celsius) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->tecSetTemperatureSetpointDegreesC(deviceID, featureID, error_code,
            temperature_degrees_celsius);
}

void
sbapi_tec_set_enable(long deviceID, long featureID,
        int *error_code, unsigned char tec_enable) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->tecSetEnable(deviceID, featureID, error_code, tec_enable);
}


int
sbapi_get_number_of_nonlinearity_coeffs_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfNonlinearityCoeffsFeatures(deviceID, error_code);
}

/**************************************************************************************/
//  C language wrapper for nonlinearity coeffs features
/**************************************************************************************/

int
sbapi_get_nonlinearity_coeffs_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNonlinearityCoeffsFeatures(deviceID, error_code,
            features, max_features);
}

int sbapi_nonlinearity_coeffs_get(long deviceID, long featureID, int *error_code,
        double *buffer, int max_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->nonlinearityCoeffsGet(deviceID, featureID, error_code,
            buffer, max_length);
}

/**************************************************************************************/
//  C language wrapper for temperature features
/**************************************************************************************/

int sbapi_get_number_of_temperature_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfTemperatureFeatures(deviceID, error_code);
}

// FIXME: Even though it may seem obvious to the experienced SeaBreeze developer, it might
//  be helpful for those just starting to use SeaBreeze to see not just the word Features 
//  for the feature ID arguments, but type of features, such as temperatureFeatureID. 

int sbapi_get_temperature_features(long deviceID, int *error_code, long *temperatureFeatures, int max_features) 
{
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getTemperatureFeatures(deviceID, error_code,
            temperatureFeatures, max_features);
}

unsigned char sbapi_temperature_count_get(long deviceID, long temperatureFeatureID, int *error_code) 
{
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->temperatureCountGet(deviceID, temperatureFeatureID, error_code);
}

double sbapi_temperature_get(long deviceID, long temperatureFeatureID, int *error_code,
        int index) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->temperatureGet(deviceID, temperatureFeatureID, error_code, index);
}

int sbapi_temperature_get_all(long deviceID, long temperatureFeatureID, int *error_code,double *buffer, int max_length) 
{
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->temperatureGetAll(deviceID, temperatureFeatureID, error_code,
            buffer, max_length);
}

/**************************************************************************************/
//  C language wrapper for introspection features
/**************************************************************************************/

int sbapi_get_number_of_introspection_features(long deviceID, int *error_code) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfIntrospectionFeatures(deviceID, error_code);
}

int sbapi_get_introspection_features(long deviceID, int *error_code, long *features, unsigned int max_features)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getIntrospectionFeatures(deviceID, error_code, features, max_features);
}

unsigned short sbapi_introspection_number_of_pixels_get(long deviceID, long introspectionFeatureID, int *error_code) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->introspectionNumberOfPixelsGet(deviceID, introspectionFeatureID, error_code);
}

int sbapi_introspection_active_pixel_ranges_get(long deviceID, long introspectionFeatureID, int *error_code, unsigned int *pixelIndexPairs, int max_length) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->introspectionActivePixelRangesGet(deviceID, introspectionFeatureID, error_code, pixelIndexPairs, max_length);
}

int sbapi_introspection_electric_dark_pixel_ranges_get(long deviceID, long introspectionFeatureID, int *error_code, unsigned int *pixelIndexPairs, int max_length)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->introspectionElectricDarkPixelRangesGet(deviceID, introspectionFeatureID, error_code, pixelIndexPairs, max_length);
}

int sbapi_introspection_optical_dark_pixel_ranges_get(long deviceID, long introspectionFeatureID, int *error_code, unsigned int *pixelIndexPairs, int max_length)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->introspectionOpticalDarkPixelRangesGet(deviceID, introspectionFeatureID, error_code, pixelIndexPairs, max_length);
}

/**************************************************************************************/
//  C language wrapper for revision features
/**************************************************************************************/

int sbapi_get_number_of_revision_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfRevisionFeatures(deviceID, error_code);
}

int sbapi_get_revision_features(long deviceID, int *error_code, long *revisionFeatures,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getRevisionFeatures(deviceID, error_code,
            revisionFeatures, max_features);
}

unsigned char sbapi_revision_hardware_get(long deviceID, long revisionFeatureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->revisionHardwareGet(deviceID, revisionFeatureID, error_code);
}

unsigned short int sbapi_revision_firmware_get(long deviceID, long revisionFeatureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->revisionFirmwareGet(deviceID, revisionFeatureID, error_code);
}


/**************************************************************************************/
//  C language wrapper for optical bench features
/**************************************************************************************/

int sbapi_get_number_of_optical_bench_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfOpticalBenchFeatures(deviceID, error_code);
}

int sbapi_get_optical_bench_features(long deviceID, int *error_code, long *opticalBenchFeatures, int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getOpticalBenchFeatures(deviceID, error_code, opticalBenchFeatures, max_features);
}

unsigned short int sbapi_optical_bench_get_fiber_diameter_microns(long deviceID, long opticalBenchFeatureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetFiberDiameterMicrons(deviceID, opticalBenchFeatureID, error_code);
}

unsigned short int sbapi_optical_bench_get_slit_width_microns(long deviceID, long opticalBenchFeatureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetSlitWidthMicrons(deviceID, opticalBenchFeatureID, error_code);
}

int sbapi_optical_bench_get_id(long deviceID, long opticalBenchFeatureID, int *error_code, char *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetID(deviceID, opticalBenchFeatureID, error_code, buffer, buffer_length);
}

int sbapi_optical_bench_get_serial_number(long deviceID, long opticalBenchFeatureID, int *error_code, char *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetSerialNumber(deviceID, opticalBenchFeatureID, error_code, buffer, buffer_length);
}

int sbapi_optical_bench_get_coating(long deviceID, long opticalBenchFeatureID, int *error_code, char *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetCoating(deviceID, opticalBenchFeatureID, error_code, buffer, buffer_length);
}

int sbapi_optical_bench_get_filter(long deviceID, long opticalBenchFeatureID, int *error_code, char *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetFilter(deviceID, opticalBenchFeatureID, error_code, buffer, buffer_length);
}

int sbapi_optical_bench_get_grating(long deviceID, long opticalBenchFeatureID, int *error_code, char *buffer, int buffer_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->opticalBenchGetGrating(deviceID, opticalBenchFeatureID, error_code, buffer, buffer_length);
}


/**************************************************************************************/
//  C language wrapper for spectrum processing features
/**************************************************************************************/

int sbapi_get_number_of_spectrum_processing_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfSpectrumProcessingFeatures(deviceID, error_code);
}

int sbapi_get_spectrum_processing_features(long deviceID, int *error_code, long *spectrumProcessingFeatures, int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getSpectrumProcessingFeatures(deviceID, error_code, spectrumProcessingFeatures, max_features);
}

unsigned short int sbapi_spectrum_processing_scans_to_average_get(long deviceID, long spectrumProcessingFeatureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrumProcessingScansToAverageGet(deviceID, spectrumProcessingFeatureID, error_code);
}

unsigned char sbapi_spectrum_processing_boxcar_width_get(long deviceID, long spectrumProcessingFeatureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->spectrumProcessingBoxcarWidthGet(deviceID, spectrumProcessingFeatureID, error_code);
}

void sbapi_spectrum_processing_scans_to_average_set(long deviceID, long featureID,
        int *error_code, unsigned short int scansToAverage) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->spectrumProcessingScansToAverageSet(deviceID, featureID, error_code,
            scansToAverage);
}

void sbapi_spectrum_processing_boxcar_width_set(long deviceID, long featureID,
        int *error_code, unsigned char boxcarWidth) {

    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->spectrumProcessingBoxcarWidthSet(deviceID, featureID, error_code,
            boxcarWidth);
}


/**************************************************************************************/
//  C language wrapper for stray light features
/**************************************************************************************/

int
sbapi_get_number_of_stray_light_coeffs_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfStrayLightCoeffsFeatures(deviceID, error_code);
}

int
sbapi_get_stray_light_coeffs_features(long deviceID, int *error_code, long *features,
        int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getStrayLightCoeffsFeatures(deviceID, error_code,
            features, max_features);
}

int sbapi_stray_light_coeffs_get(long deviceID, long featureID, int *error_code,
        double *buffer, int max_length) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->strayLightCoeffsGet(deviceID, featureID, error_code,
            buffer, max_length);
}


/**************************************************************************************/
//  C language wrapper for data buffer features
/**************************************************************************************/

int sbapi_get_number_of_data_buffer_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfDataBufferFeatures(deviceID, error_code);
}

int sbapi_get_data_buffer_features(long deviceID, int *error_code,
            long *features, unsigned int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getDataBufferFeatures(deviceID, error_code,
            features, max_features);
}

void sbapi_data_buffer_clear(long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->dataBufferClear(deviceID, featureID, error_code);
}

void sbapi_data_buffer_remove_oldest_spectra(long deviceID, long featureID, int *error_code, unsigned int numberOfSpectra) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->dataBufferRemoveOldestSpectra(deviceID, featureID, error_code, numberOfSpectra);
}

unsigned long sbapi_data_buffer_get_number_of_elements(long deviceID,
            long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->dataBufferGetNumberOfElements(deviceID, featureID, error_code);
}

unsigned long sbapi_data_buffer_get_buffer_capacity(long deviceID, long featureID, int *error_code) 
{
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->dataBufferGetBufferCapacity(deviceID, featureID, error_code);
}

unsigned long sbapi_data_buffer_get_buffer_capacity_maximum(
            long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->dataBufferGetBufferCapacityMaximum(deviceID, featureID, error_code);
}

unsigned long sbapi_data_buffer_get_buffer_capacity_minimum(
            long deviceID, long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->dataBufferGetBufferCapacityMinimum(deviceID, featureID, error_code);
}

void sbapi_data_buffer_set_buffer_capacity(long deviceID, long featureID, int *error_code, unsigned long capacity) 
{
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->dataBufferSetBufferCapacity(deviceID, featureID, error_code, capacity);
}


/**************************************************************************************/
//  C language wrapper for fast buffer features
/**************************************************************************************/

int sbapi_get_number_of_fast_buffer_features(long deviceID, int *error_code) {
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfFastBufferFeatures(deviceID, error_code);
}

int sbapi_get_fast_buffer_features(long deviceID, int *error_code,
	long *features, unsigned int max_features) {
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getFastBufferFeatures(deviceID, error_code,
		features, max_features);
}

unsigned char sbapi_fast_buffer_get_buffering_enable(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->fastBufferGetBufferingEnable(deviceID, featureID, error_code);
}

void sbapi_fast_buffer_set_buffering_enable(long deviceID, long featureID, int *error_code, unsigned char isEnabled)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->fastBufferSetBufferingEnable(deviceID, featureID, error_code, isEnabled);
}

unsigned int sbapi_fast_buffer_get_consecutive_sample_count(long deviceID, long featureID, int *error_code)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->fastBufferGetConsecutiveSampleCount(deviceID, featureID, error_code);
}

void sbapi_fast_buffer_set_consecutive_sample_count(long deviceID, long featureID, int *error_code, unsigned int consecutiveSampleCount)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	wrapper->fastBufferSetConsecutiveSampleCount(deviceID, featureID, error_code, consecutiveSampleCount);
}

/**************************************************************************************/
//  C language wrapper for acquisition delay features
/**************************************************************************************/

int sbapi_get_number_of_acquisition_delay_features(long deviceID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getNumberOfAcquisitionDelayFeatures(deviceID, error_code);
}

int sbapi_get_acquisition_delay_features(long deviceID, int *error_code,
            long *features, unsigned int max_features) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->getAcquisitionDelayFeatures(deviceID, error_code,
            features, max_features);
}

void sbapi_acquisition_delay_set_delay_microseconds(long deviceID, long featureID,
            int *error_code, unsigned long delay_usec) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    wrapper->acquisitionDelaySetDelayMicroseconds(deviceID, featureID,
            error_code, delay_usec);
}

unsigned long sbapi_acquisition_delay_get_delay_microseconds(long deviceID,
            long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->acquisitionDelayGetDelayMicroseconds(deviceID, featureID,
            error_code);
}

unsigned long sbapi_acquisition_delay_get_delay_increment_microseconds(long deviceID,
            long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->acquisitionDelayGetDelayIncrementMicroseconds(deviceID, featureID,
            error_code);
}

unsigned long sbapi_acquisition_delay_get_delay_maximum_microseconds(long deviceID,
            long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->acquisitionDelayGetDelayMaximumMicroseconds(deviceID, featureID,
            error_code);
}

unsigned long sbapi_acquisition_delay_get_delay_minimum_microseconds(long deviceID,
            long featureID, int *error_code) {
    SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

    return wrapper->acquisitionDelayGetDelayMinimumMicroseconds(deviceID, featureID,
            error_code);
}


/**************************************************************************************/
//  C language wrapper for i2c_master features
/**************************************************************************************/

int sbapi_get_number_of_i2c_master_features(long deviceID, int *error_code) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getNumberOfI2CMasterFeatures(deviceID, error_code);
}

int sbapi_get_i2c_master_features(long deviceID, int *error_code, long *features, unsigned int max_features) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->getI2CMasterFeatures(deviceID, error_code, features, max_features);
}


unsigned char sbapi_i2c_master_get_number_of_buses(long deviceID, long featureID, int *error_code) 
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->i2cMasterGetNumberOfBuses(deviceID, featureID, error_code);
}

unsigned short sbapi_i2c_master_read_bus(long deviceID, long featureID, int *error_code, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->i2cMasterReadBus(deviceID, featureID, error_code, busIndex, slaveAddress, readData, numberOfBytes);
}

unsigned short sbapi_i2c_master_write_bus(long deviceID, long featureID, int *error_code, unsigned char busIndex, unsigned char slaveAddress, unsigned char *writeData, unsigned short numberOfBytes)
{
	SeaBreezeAPI *wrapper = SeaBreezeAPI::getInstance();

	return wrapper->i2cMasterWriteBus(deviceID, featureID, error_code, busIndex, slaveAddress, writeData, numberOfBytes);
}

