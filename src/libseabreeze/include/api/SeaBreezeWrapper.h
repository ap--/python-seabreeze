/***************************************************//**
 * @file    SeaBreezeWrapper.h
 * @date    May 2017
 * @author  Ocean Optics, Inc.
 *
 * This is a trivial interface to SeaBreeze that allows
 * the user to connect to devices over USB.
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

#ifndef SEABREEZE_WRAPPER_H
#define SEABREEZE_WRAPPER_H

#define SEABREEZE_API_VERSION "3.0.11"  //!< current version of the SeaBreezeWrapper API
#define SEABREEZE_MAX_DEVICES 32        //!< how many different spectrometer types we support

#include "api/DllDecl.h"

#ifdef __cplusplus

/**
* @brief Encapsulates all SeaBreeze classes
*/
namespace seabreeze {
	class Device;
}

/**
    Original interface to SeaBreeze.  SeaBreezeAPI represents a proposed architecture for
    future development.
*/
class DLL_DECL SeaBreezeWrapper {
public:

    //! get handle to Singleton
    static SeaBreezeWrapper *getInstance();

    //! free some static memory under Microsoft profiler
    static void shutdown();

    // lifecycle
    int    openSpectrometer          (int index, int *errorCode);
    int    closeSpectrometer         (int index, int *errorCode);

	// Wrapper features
	void   setVerbose(bool flag);
	void   setLogfile(char *path, int length);
	int    getAPIVersionString(char *buffer, int length);
	int    getErrorString(int errorCode, char *buffer, int buffer_length);
	int	   getErrorStringMaximumLength(void);

    // metadata
    int    getModel                  (int index, int *errorCode, char *buf, int len);
	int	   getModelStringMaximumLength(void);
    int    getSerialNumber           (int index, int *errorCode, char *buf, int len);
	unsigned char   getSerialNumberMaximumLength(int index, int *errorCode);

		
    // basic acquisitions
    void   setIntegrationTimeMicrosec(int index, int *errorCode, unsigned long integration_time_micros);
    int    getWavelengths(int index, int *errorCode, double *wavelengths, int length);
    int    getFormattedSpectrum(int index, int *errorCode, double* buffer, int buffer_length);
    int    getUnformattedSpectrum(int index, int *errorCode, unsigned char *buffer, int buffer_length);
	int	   getFastBufferSpectrum(int index, int *errorCode, unsigned char *buffer, int buffer_length, unsigned int numberOfSamplesToRetrieve);
    int    getFormattedSpectrumLength(int index, int *errorCode);
    int    getUnformattedSpectrumLength(int index, int *errorCode);
    long   getMinIntegrationTimeMicrosec(int index, int *errorCode);
    long   getMaxIntegrationTimeMicrosec(int index, int *errorCode);
    int    getMaximumIntensity(int index, int *errorCode);

    // advanced features
    int    getElectricDarkPixelIndices(int index, int *errorCode, int *indices, int length);
	int    getOpticalDarkPixelIndices(int index, int *errorCode, int *indices, int length);
	int    getActivePixelIndices(int index, int *errorCode, int *indices, int length);
	unsigned short    getNumberOfPixels(int index, int *errorCode);
    void   setTriggerMode            (int index, int *errorCode, int mode);
    void   setStrobeEnable           (int index, int *errorCode, unsigned char strobe_enable);
    void   setShutterOpen            (int index, int *errorCode, unsigned char opened);
    void   setContinuousStrobePeriodMicrosec(int index, int *errorCode, unsigned short strobe_id, unsigned long period_usec);
    void   setAcquisitionDelayMicrosec(int index, int *errorCode, unsigned long delay_usec);

    // Buffering features
    void   clearBuffer               (int index, int *errorCode);
    void   removeOldestSpectraFromBuffer	(int index, int *errorCode, unsigned int numberOfSpectra);
    unsigned long getBufferElementCount(int index, int *errorCode);
    unsigned long getBufferCapacity  (int index, int *errorCode);
    unsigned long getBufferCapacityMaximum(int index, int *errorCode);
    unsigned long getBufferCapacityMinimum(int index, int *errorCode);
    void   setBufferCapacity         (int index, int *errorCode, unsigned long capacity);

	// fast buffer features
	unsigned char getBufferingEnable(int index, int *errorCode);
    void   setBufferingEnable         (int index, int *errorCode, unsigned char isEnabled);
	unsigned int getConsecutiveSampleCount(int index, int *errorCode);
	void   setConsecutiveSampleCount(int index, int *errorCode, unsigned int consecutiveSampleCount);
	
    // EEPROM access
    int    readEEPROMSlot            (int index, int *errorCode, int slot_number, unsigned char *buffer, int buffer_length);
    int    writeEEPROMSlot           (int index, int *errorCode, int slot_number, unsigned char *buffer, int buffer_length);

    // irradiance calibration
    int    readIrradCalibration      (int index, int *errorCode, float *buffer, int buffer_length);
    int    writeIrradCalibration     (int index, int *errorCode, float *buffer, int buffer_length);
    int    hasIrradCollectionArea    (int index, int *errorCode);
    float  readIrradCollectionArea   (int index, int *errorCode);
    void   writeIrradCollectionArea  (int index, int *errorCode, float area);
    
    // ethernet configuration features
	unsigned char get_GbE_Enable_Status	(int index, int *errorCode, unsigned char interfaceIndex);
    void   set_GbE_Enable_Status      	(int index, int *errorCode, unsigned char interfaceIndex, unsigned char isEnabled);
	void   get_MAC_Address				(int index, int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6]);
	void   set_MAC_Address				(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6]);

	// multicast features
	unsigned char getMutlicastEnableState(int index, int *errorCode, unsigned char interfaceIndex);
	void   setMulticastEnableState(int index, int *errorCode, unsigned char interfaceIndex, unsigned char isEnabled);

	// IPv4 features
	unsigned char get_IPv4_DHCP_Enable_State(int index, int *errorCode, unsigned char interfaceIndex);
	void   set_IPv4_DHCP_Enable_State(int index, int *errorCode, unsigned char interfaceIndex, unsigned char isEnabled);
	unsigned char get_Number_Of_IPv4_Addresses(int index, int *errorCode, unsigned char interfaceIndex);
	void   get_IPv4_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask);
	void   get_IPv4_Default_Gateway(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4]);
	void   set_IPv4_Default_Gateway(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4]);
	void   add_IPv4_Address(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask);
	void   delete_IPv4_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex);

	/* DHCP server features */
	void get_DHCP_Server_Address(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask);
	void set_DHCP_Server_Address(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask);
	unsigned char get_DHCP_Server_Enable_State(int index, int *errorCode, unsigned char interfaceIndex);
	void set_DHCP_Server_Enable_State(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState);

	// wifi configuration features
	unsigned char getWifiConfigurationMode(int index, int *errorCode, unsigned char interfaceIndex);
	void   setWifiConfigurationMode(int index, int *errorCode, unsigned char interfaceIndex, unsigned char mode);
	unsigned char getWifiConfigurationSecurityType(int index, int *errorCode, unsigned char interfaceIndex);
	void   setWifiConfigurationSecurityType(int index, int *errorCode, unsigned char interfaceIndex, unsigned char securityType);
	unsigned char   getWifiConfigurationSSID(int index, int *errorCode, unsigned char interfaceIndex, unsigned char(*ssid)[32]);
	void   setWifiConfigurationSSID(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length);
	void   setWifiConfigurationPassPhrase(int index, int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength);


	// network configuration features
	unsigned char getNumberOfNetworkInterfaces(int index, int *errorCode);
	unsigned char getNetworkInterfaceConnectionType(int index, int *errorCode, unsigned char interfaceIndex);
	unsigned char getNetworkInterfaceEnableState(int index, int *errorCode, unsigned char interfaceIndex);
	void		  setNetworkInterfaceEnableState(int index, int *errorCode, unsigned char interfaceIndex, unsigned char enableState);
	unsigned char runNetworkInterfaceSelfTest(int index, int *errorCode, unsigned char interfaceIndex);
	void		  saveNetworkInterfaceConnectionSettings(int index, int *errorCode, unsigned char interfaceIndex);

	// gpio features
	unsigned char getGPIO_NumberOfPins(int index, int *errorCode);
	unsigned int getGPIO_OutputEnableVector(int index, int *errorCode);
	void setGPIO_OutputEnableVector(int index, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask);
	unsigned int getGPIO_ValueVector(int index, int *errorCode);
	void setGPIO_ValueVector(int index, int *errorCode, unsigned int valueVector, unsigned int bitMask);
	unsigned char getEGPIO_NumberOfPins(int index, int *errorCode);
	unsigned char getEGPIO_AvailableModes(int index, int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maxModeCount);
	unsigned char getEGPIO_CurrentMode(int index, int *errorCode, unsigned char pinNumber);
	void setEGPIO_Mode(int index, int *errorCode, unsigned char pinNumber, unsigned char mode, float value);
	unsigned int getEGPIO_OutputVector(int index, int *errorCode);
	void setEGPIO_OutputVector(int index, int *errorCode, unsigned int outputVector, unsigned int bitMask);
	float getEGPIO_Value(int index, int *errorCode, unsigned char pinNumber);
	void setEGPIO_Value(int index, int *errorCode, unsigned char pinNumber, float value);

    // thermal-electric cooler
    double readTECTemperature        (int index, int *errorCode);
    void   setTECTemperature         (int index, int *errorCode, double temperature_degrees_celsius);
    void   setTECEnable              (int index, int *errorCode, unsigned char tec_enable);
    void   setTECFanEnable           (int index, int *errorCode, unsigned char tec_fan_enable);

    // raw USB access
    int    writeUSB                  (int index, int *errorCode, unsigned char endpoint, unsigned char *buffer, unsigned int length);
    int    readUSB                   (int index, int *errorCode, unsigned char endpoint, unsigned char *buffer, unsigned int length);
    int    getUSBDescriptorString    (int index, int *errorCode, int id, unsigned char *buffer, int length);

    // light sources (JAZ?)
    int    getLightSourceCount       (int index, int *errorCode);
    void   setLightSourceEnable      (int index, int *errorCode, int lightIndex, unsigned char enable);
    void   setLightSourceIntensity   (int index, int *errorCode, int lightIndex, double intensity);

	// i2c master features
	unsigned char getI2CMasterNumberOfBuses(int index, int *errorCode);
	unsigned short readI2CMasterBus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes);
	unsigned short writeI2CMasterBus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short dataLength);

private:
    SeaBreezeWrapper();
    virtual ~SeaBreezeWrapper();

    //! Singleton
    static SeaBreezeWrapper *instance;

    //! types of supported Ocean Optics devices (not actual devices found enumerated on the USB bus)
    seabreeze::Device *devices[SEABREEZE_MAX_DEVICES];
};

extern "C" {
#endif /* __cplusplus */

    /* All of these C functions start with seabreeze_ to prevent namespace
     * collisions.
     */

    /**
     * @brief This function opens a device attached to the system.
     * @param index (Input) The index of a USB device to try to open.
     *        Valid values will range from 0 to N-1 for N connected devices.
     * @param error_code (Output) A pointer to an integer that can be used
     *        for storing error codes.
     * @return int: The function will return an integer of 0 if it opened a
     *      device successfully, or 1 if no device was opened (in which
     *      case the error_code variable will be set).
     *
     * This can be called repeatedly with incrementing index values (until
     * it returns 1) to open all connected devices.
     *
     * Note that the index used to open a device with this function should also
     * be used to communicate with that same device in the other functions
     * provided here.
     */
    DLL_DECL int seabreeze_open_spectrometer(int index, int *error_code);

    /**
     * @brief This function closes the spectrometer attached to the system.
     * @param index (Input) The index of a device previously opened with
     *        open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used
     *        for storing error codes.
     * @return int: This function will return 1 no matter what!  (MZ)
     */
    DLL_DECL int seabreeze_close_spectrometer(int index, int *error_code);

	/**
	* @brief This function returns a the maximum length for an error string.
	* @return int: Maximum size of an error string.
	*/
	DLL_DECL int seabreeze_get_error_string_maximum_length(void);

    /**
     * @brief This function returns a description of the error denoted by
     *        error_code.
     * @param error_code (Input) The integer error code to look up.  Error
     *        codes not be zero, but can be any non-zero integer (positive or
     *        negative).
     * @param buffer (Output) A character buffer allocated to contain at least
     *        'buffer_length' bytes, which will be populated with the string
     *        description of the given error code.
     * @param buffer_length (Input) allocated size of the output buffer
     * @return int: Number of bytes written to buffer.
     */
    DLL_DECL int seabreeze_get_error_string(int error_code, char *buffer, int buffer_length);

    /**
     * @brief This function returns a string denoting the type of the device.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *      error codes.  This may be NULL.
     * @param buffer (Output)  A character buffer allocated to contain at least
     *      'buffer_length' bytes, which will be populated with the
     *      spectrometer type.
     * @param buffer_length (Input) allocated size of the buffer
     * @return int: Number of bytes written to the buffer.
     *
     * The populated buffer will hold one of the following strings:
     *
     * \verbatim
     *      NONE:        Used if no spectrometer is found (error_code will also be set)
	 *      FLAMEX:		 Represents a Flame X Spectrometer
     *      HR2000:      Represents an HR2000 Spectrometer
     *      HR2000PLUS:  Represents an HR2000+ Spectrometer
     *      HR4000:      Represents an HR4000 Spectrometer
     *      JAZ:         Represents a Jaz Spectrometer
     *      MAYA2000:    Represents a Maya2000 Spectrometer
     *      MAYALSL:     Represents a Maya-LSL Spectrometer
     *      MAYA2000PRO: Represents a Maya2000 Pro Spectrometer
     *      NIRQUEST256: Represents an NIRQuest256 Spectrometer
     *      NIRQUEST512: Represents an NIRQuest512 Spectrometer
     *      QE65000:     Represents a QE65000 Spectrometer
     *      QE-PRO:      Represents a QE-Pro Spectrometer
     *      STS:         Represents an STS Spectrometer
     *      TORUS:       Represents a Torus Spectrometer
     *      USB2000:     Represents a USB2000 Spectrometer
     *      USB2000PLUS: Represents a USB2000+ Spectrometer
     *      USB4000:     Represents a USB4000 Spectrometer
     * \endverbatim
     */
    DLL_DECL int seabreeze_get_model(int index, int *error_code, char *buffer, int buffer_length);

	/**
	* @brief This function returns a the maximum length for a model string.
	* @return int: Maximum size of a model string.
	*/
	DLL_DECL int seabreeze_get_model_string_maximum_length(void);

    /**
     * @brief This function sets the trigger mode for the specified device.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *      error codes.
     * @param mode (Input) a trigger mode (0 = normal, 1 = software, 2 = synchronization,
     *        3 = external hardware, etc.)
     *
     * Note that requesting an unsupported mode will result in an error.
     */

    DLL_DECL void seabreeze_set_trigger_mode(int index, int *error_code, int mode);

    /**
     * @brief This function sets the integration time for the specified device.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *      error codes.
     * @param integration_time_micros (Input) The new integration time in units of
     *      microseconds
     *
     * This function does not automatically perform a stability scan.
     * If your application requires a stability scan following a change
     * in integration time, you need to command that yourself.
     */
    DLL_DECL void seabreeze_set_integration_time_microsec(int index, int *error_code, unsigned long integration_time_micros);

    /**
     * @brief This function returns the smallest integration time setting,
     *        in microseconds, that is valid for the spectrometer.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @return Returns minimum legal integration time in microseconds if > 0.
     *        On error, returns -1 and error_code will be set accordingly.
     */
    DLL_DECL long seabreeze_get_min_integration_time_microsec(int index, int *error_code);

    /**
     * @brief This function sets the shutter state on the spectrometer.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param opened (Input) A logical boolean used for denoting the desired
     *      state (opened/closed) of the shutter.   If the value of opened is
     *      non-zero, then the shutter will open.  If the value of opened is
     *      zero, then the shutter will close.
     */
    DLL_DECL void seabreeze_set_shutter_open(int index, int *error_code, unsigned char opened);

    /**
     * @brief This function sets the strobe enable on the spectrometer.  Note that
     *        this refers to a particular set of one or more digital pins on the
     *        device: lamp enable, single strobe, and continuous strobe may all
     *        be affected by this setting, and these generally control lamps
     *        that are external to the device.  Note that this is related to, but
     *        different from, the light source interface which allows the intensity
     *        and/or enable status of individual light sources (e.g. lamp bulbs,
     *        LEDs, or lasers) in attached modules to be controlled.  Refer to
     *        the seabreeze_xxx_light_source_ functions for finer control of
     *        certain light source modules that are more closely integrated with
     *        the spectrometer.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output)  A pointer to an integer that can be used for storing
     *        error codes.
     * @param strobe_enable (Input) A logical boolean used for denoting the
     *      desired value (high/low) of the strobe-enable pin.   If the value of
     *      strobe_enable is zero, then the pin should be set low.  If the value
     *      of strobe_enable is non-zero, then the pin should be set high.
     */
    DLL_DECL void seabreeze_set_strobe_enable(int index, int *error_code, unsigned char strobe_enable);

    /**
     * @brief This function gets the number of attached light sources that can
     *        be programmatically controlled.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output)  A pointer to an integer that can be used for storing
     *        error codes.
     * @return The number of light sources that can be controlled
     */
    DLL_DECL int seabreeze_get_light_source_count(int index, int *error_code);

    /**
     * @brief This function sets the enable status on a connected light source.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output)  A pointer to an integer that can be used for storing
     *        error codes.
     * @param light_index (Input)  The index of the light source.  This will
     *      usually be zero, but if the light module contains multiple LEDs,
     *      bulbs, lasers, etc. then this may be higher.  Use
     *      seabreeze_get_light_source_count() to bound the maximum value .
     * @param enable (Input) A logical boolean used for denoting whether to enable
     *      the indicated light source.   If the value of
     *      enable is zero, then the light source should be disabled.  If the value
     *      of enable is non-zero, then the light source should be enabled.
     */
    DLL_DECL void seabreeze_set_light_source_enable(int index, int *error_code, int light_index, unsigned char enable);

    /**
     * @brief This function sets the intensity of a connected light source.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output)  A pointer to an integer that can be used for storing
     *        error codes.
     * @param light_index (Input)  The index of the light source.  This will
     *      usually be zero, but if the light module contains multiple LEDs,
     *      bulbs, lasers, etc. then this may be higher.  Use
     *      seabreeze_get_light_source_count() to bound the maximum value.
     * @param intensity (Input) The desired intensity of the light source.  The
     *      range of intensities is normalized over [0, 1], where 0 is the
     *      minimum controllable intensity of the light source, and 1 is the
     *      maximum.
     *
     * @warning SETTING THE INTENSITY TO ZERO MAY NOT CAUSE THE LIGHT SOURCE
     *      TO TURN OFF COMPLETELY.  The light source will go to the
     *      dimmest level it can reach without changing its enable status.
     *      To switch the light source off, try using the
     *      seabreeze_set_light_source_enable() function or provide the
     *      operator with another way to disable or block the light source.
     */
    DLL_DECL void seabreeze_set_light_source_intensity(int index, int *error_code, int light_index, double intensity);

    /**
     * @brief This function reads a string out of the spectrometer's EEPROM slot
     *        and returns the result.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param slot_number (Input) The number of the slot to read out.  Possible
     *        values are 0 through 16*.
     * @param buffer (Output)  A buffer (with memory already allocated) to hold the
     *        value read out of the EEPROM slot
     * @param buffer_length (Input) The length of the allocated output buffer (typically 16)
     * @return int: The number of bytes read from the EEPROM slot into the buffer
     *
     * (*) Actual maximum slot count varies by spectrometer model.  For the number of supported
     *     slots on your spectrometer, see EEPROMSlotFeature instantiation in appropriate
     *     device file under src/vendors/OceanOptics/devices.
     */
    DLL_DECL int seabreeze_read_eeprom_slot(int index, int *error_code, int slot_number, unsigned char *buffer, int buffer_length);

    /**
     * @brief This function writes a string to a spectrometer's EEPROM slot
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer for storing error codes.
     * @param slot_number (Input) The number of the slot being written (e.g. 0-16*).
     * @param buffer (Input) A buffer containing the text to write to the slot.
     * @param buffer_length (Input) The length of the text to write (typically 15)
     * @return number of bytes written
     *
     * @warning CARELESS USE OF THIS FUNCTION CAN RENDER YOUR SPECTROMETER INOPERABLE,
     *          POSSIBLY REQUIRING R.M.A. OR REFLASHING TO RESTORE FUNCTIONALITY.
     *          PLEASE READ YOUR SPECTROMETER'S DATA SHEET CAREFULLY BEFORE USE.
     *
     * (*) See note in seabreeze_read_eeprom_slot() regarding per-device slot limits.
     */
    DLL_DECL int seabreeze_write_eeprom_slot(int index, int *error_code, int slot_number, unsigned char *buffer, int buffer_length);

    /**
     * @brief This function reads out an irradiance calibration from the spectrometer's
     *        internal memory if that feature is supported.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param buffer (Output) array of floating point values into which calibration values are stored
     * @param buffer_length (Input) maximum number of values to copy from the device into buffer
     * @return int: the number of floats read from the device into the buffer
     */
    DLL_DECL int seabreeze_read_irrad_calibration(int index, int *error_code, float *buffer, int buffer_length);

    /**
     * @brief This function writes an irradiance calibration to the spectrometer's
     *        internal memory if that feature is supported.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param buffer (Output) array of floating point values to store into the device
     * @param buffer_length (Input) number of calibration factors to write
     * @return int: the number of floats written from the buffer to the device
     */
    DLL_DECL int seabreeze_write_irrad_calibration(int index, int *error_code, float *buffer, int buffer_length);

    /**
     * @brief This function checks for an irradiance collection area in the spectrometer's
     *        internal memory if that feature is supported.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @return int: 0 if no collection area available, 1 if available.
     */
    DLL_DECL int seabreeze_has_irrad_collection_area(int index, int *error_code);

    /**
     * @brief This function reads an irradiance collection area from the spectrometer's
     *        internal memory if that feature is supported.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @return float: collection area (typically in units of cm^2) read from device
     */
    DLL_DECL float seabreeze_read_irrad_collection_area(int index, int *error_code);

    /**
     * @brief This function writes an irradiance collection area to the spectrometer's
     *        internal memory if that feature is supported.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param area (Input) collection area to save to the EEPROM (presumably cm^2)
     */
    DLL_DECL void seabreeze_write_irrad_collection_area(int index, int *error_code, float area);

    /**
     * @brief This function reads the value of the TEC and returns the value in
     *        degrees celsius.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @return int: The TEC temperature in degrees Celsius.
     */
    DLL_DECL double seabreeze_read_tec_temperature(int index, int *error_code);

    /**
     * @brief This function sets the TEC temperature.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param temperature_degrees_celsius (Input) The desired temperature, in degrees
     *        Celsius.
     */
    DLL_DECL void seabreeze_set_tec_temperature(int index, int *error_code, double temperature_degrees_celsius);

    /**
     * @brief This function enables the TEC feature on the spectrometer.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param tec_enable (Input) A logical boolean that denotes the desired TEC enable
     *        state.  If the value of tec_enable is zero, the TEC should be disabled.
     *        If the value of tec_enable is non-zero, the TEC should be enabled.
     */
    DLL_DECL void seabreeze_set_tec_enable(int index, int *error_code, unsigned char tec_enable);

    /**
     * @brief This function enables the TEC Fan on the spectrometer.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param tec_fan_enable (Input) A logical boolean that denotes the desired TEC fan enable
     *        state.  If the value of tec_fan_enable is zero, the TEC fan should be disabled.
     *        If the value of tec_fan_enable is non-zero, the TEC fan should be enabled.
     */
    DLL_DECL void seabreeze_set_tec_fan_enable(int index, int *error_code, unsigned char tec_fan_enable);

    /**
     * @brief This acquires a spectrum and returns the answer in raw, unformatted bytes.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param buffer (Output) A buffer (with memory already allocated) to hold the
     *        spectral data
     * @param buffer_length (Input) The length of the buffer in bytes (not pixels)
     * @return int: The number of bytes read into the buffer
     * @see   sample-code/c/demo-unformatted-spectrum.c
     *
     * An unformatted spectrum is the raw sequence of bytes returned by the spectrometer to 
     * the PC over USB.  The bytes have not been broken down into pixels, they're in the 
     * original endianness, absolutely nothing has been done to them.  The caller is expected 
     * to know how many bytes are returned by each spectrometer model, which bytes indicate 
     * synchronization points or whatever, etc.  
     */
    DLL_DECL int seabreeze_get_unformatted_spectrum(int index, int *error_code, unsigned char *buffer, int buffer_length);

	/**
	* @brief This acquires the number of spectrum samples defined by seabreeze_set_consecutive_sample_count
	*        and returns the number of samples defined by numberOfSamplesToRetrieve with meta data
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) A pointer to an integer that can be used for storing
	*        error codes.
	* @param buffer (Output) A buffer (with memory already allocated) to hold the
	*        spectral data
	* @param buffer_length (Input) The length of the buffer in bytes (not pixels)
	* @param numberOfSamplesToRetrieve, currently a maximum of 15 samples can be retrieve by any get fast buffer spectrum
	* @return int: The number of bytes read into the buffer
	*
	*/
	DLL_DECL int seabreeze_get_fast_buffer_spectrum(int index, int *error_code, unsigned char *buffer, int buffer_length, unsigned int numberOfSamplesToRetrieve);

    /**
     * @brief This acquires a spectrum and returns the answer in formatted
     *        floats.  In this mode, auto-nulling should be automatically
     *        performed for devices that support it.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param buffer (Output) A buffer (with memory already allocated) to hold the
     *        spectral data
     * @param buffer_length (Input) The length of the buffer in floats (not bytes)
     * @return int: The number of floats read into the buffer
     *
     * A formatted spectrum returns exactly one double-precision floating-point IEEE value 
     * per pixel, as opposed to a raw byte stream.  It has also had autonulling (gain control) 
     * applied, meaning it has been scaled up to the spectrometer's full dynamic range using 
     * the gain setting recorded in that spectrometer’s EEPROM.
     */
    DLL_DECL int seabreeze_get_formatted_spectrum(int index, int *error_code, double* buffer, int buffer_length);

    /**
     * @brief This returns an integer denoting the length of a raw spectrum
     *        (as returned by get_unformatted_spectrum(...)).
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *      error codes.
     * @return int: An integer denoting the length of an unformatted spectrum in bytes
     *
     * The caller is expected to know the number of bytes per pixel and the endian
     * ordering, but it will normally be 2 bytes per pixel, LSB-MSB order.
     */
    DLL_DECL int seabreeze_get_unformatted_spectrum_length(int index, int *error_code);

    /**
     * @brief This returns an integer denoting the number of pixels in a
     *        formatted spectrum (as returned by get_formatted_spectrum(...)).
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @return int: An integer denoting the length of a formatted spectrum (in pixels)
     */
    DLL_DECL int
    seabreeze_get_formatted_spectrum_length(int index, int *error_code);

    /**
     * @brief This computes the wavelengths for the spectrometer and fills in the
     *        provided array (up to the given length) with those values.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param wavelengths (Output) A pre-allocated array of doubles into which the
     *        wavelengths will be copied
     * @param length (Input) The number of values to copy into the wavelength array (this should
     *        be no larger than the number of doubles allocated in the wavelengths
     *        array)
     * @return int: An integer denoting the number of wavelengths written to the buffer
     */
    DLL_DECL int seabreeze_get_wavelengths(int index, int *error_code, double *wavelengths, int length);

    /**
     * @brief This reads the device's serial number and fills the
     *        provided array (up to the given length) with it.
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param buffer (Output) A pre-allocated array of characters into which the serial number
     *        will be copied
     * @param buffer_length (Input) The number of values to copy into the buffer (this should
     *        be no larger than the number of chars allocated in the buffer) (typically 16)
     * @return int: An integer denoting the number of bytes written into the buffer
     *
     * Note that "serial numbers" may include both digits and letters
     */
    DLL_DECL int seabreeze_get_serial_number(int index, int *error_code, char *buffer, int buffer_length);

	/**
	* @brief This reads the device's maximum serial number length
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) A pointer to an integer that can be used for storing
	*        error codes.
	* @return int: An integer denoting the maximum size of the device serial number
	*
	*/
	DLL_DECL unsigned char seabreeze_get_serial_number_max_length(int index, int *error_code);


	/**
	* @brief This returns the number of pixels provided by the detector
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) A pointer to an integer that can be used for storing
	*        error codes.
	* @return unsigned int: An integer denoting the number of pixels provided by the detector
	*
	* Note that not all spectrometers provide the number of detector pixels; in that case,
	* this function will return zero.
	*/
	DLL_DECL unsigned short seabreeze_get_number_of_pixels(int index, int *error_code);

    /**
     * @brief This fills in the provided array (up to the given length) with the indices
     *        of the pixels that are electrically active but optically masked
     *        (a.k.a. electric dark pixels).
     * @param index (Input) The index of a device previously opened with open_spectrometer().
     * @param error_code (Output) A pointer to an integer that can be used for storing
     *        error codes.
     * @param indices (Output) A pre-allocated array of ints into which the pixel indices
     *        will be copied
     * @param length (Input) The number of values to copy into the indices array (this should
     *        be no larger than the number of ints allocated in the indices array)
     * @return int: An integer denoting the number of indices written into the indices buffer
     *
     * Note that not all detectors have electrically dark pixels; in that case,
     * this function will return zero.
     */
    DLL_DECL int seabreeze_get_electric_dark_pixel_indices(int index, int *error_code, int *indices, int length);

	/**
	* @brief This fills in the provided array (up to the given length) with the indices
	*        of the pixels that are optically active and  masked
	*        (a.k.a. optical dark pixels).
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) A pointer to an integer that can be used for storing
	*        error codes.
	* @param indices (Output) A pre-allocated array of ints into which the pixel indices
	*        will be copied
	* @param length (Input) The number of values to copy into the indices array (this should
	*        be no larger than the number of ints allocated in the indices array)
	* @return int: An integer denoting the number of indices written into the indices buffer
	*
	* Note that not all detectors have optically dark pixels; in that case,
	* this function will return zero.
	*/
	DLL_DECL int seabreeze_get_optical_dark_pixel_indices(int index, int *error_code, int *indices, int length);

	/**
	* @brief This fills in the provided array (up to the given length) with the indices
	*        of the active pixels
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) A pointer to an integer that can be used for storing
	*        error codes.
	* @param indices (Output) A pre-allocated array of ints into which the pixel indices
	*        will be copied
	* @param length (Input) The number of values to copy into the indices array (this should
	*        be no larger than the number of ints allocated in the indices array)
	* @return int: An integer denoting the number of indices written into the indices buffer
	*
	* Note that not all detectors provide indicies of active pixels.
	*/
	DLL_DECL int seabreeze_get_active_pixel_indices(int index, int *error_code, int *indices, int length);

    /**
     * @brief Shutdown SeaBreeze completely, releasing all resources and destroying
     *        any cached device handles.
     *
     * This function is not normally needed (Singletons are destroyed automatically
     * at process end), but calling this explicitly can resolve some spurious warnings
     * in highly paranoid memory leak profilers.
     */
    DLL_DECL void seabreeze_shutdown();

    /**
    * @brief Write a raw array of bytes to a USB spectrometer.
    * @param index (Input) index of an opened spectrometer
    * @param errorCode (Output) pointer to an allocated integer field for receiving error codes
    * @param endpoint (Input) USB endpoint for write operation
    *        (see src/vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeature.cpp)
    * @param buffer (Input) array of bytes to send to USB spectrometer
    * @param length (Input) number of bytes to write from buffer to spectrometer
    * @return number of bytes written
    *
    * Write the bytes in the buffer according to the two command bytes at the
    * start of the buffer.
    *
    * \section Endpoints
    *
    * You may wonder why the caller needs to specify an explicit endpoint for
    * raw read and write operations; does not SeaBreeze already know the appro-
    * priate endpoints for any open spectrometer, for example.  Yes it does,
    * but currently there is no easy way to extract endpoints from a generic
    * spectrometer, due to the fact that OOIUSBCypressEndpointMap and
    * OOIUSBSTSEndpointMap do not share a common base class (see
    * OOIUSBEndpointMaps.h).
    *
    * We could attempt to extract them from the TransferHelpers held by
    * OOIUSBInterface, but without a Hint to use as a key, we would likely
    * obtain unmatched or inappropriate versions for control exchanges.
    * As ControlHint and OBPControlHint share no common ancestor or type data
    * to associate them while distinguishing from other hint types, we cannot
    * automatically infer appropriate control endpoints.
    *
    * And finally, we have no way of knowing that the user really wants to
    * use control endpoints anyway: they may actually desire high-speed
    * spectral retrieval.  So really, this needs to remain a manual external
    * operation until such time as we provide a much richer, generic query
    * interface over our internal endpoint lookup tables.
    *
    * \section Example
    *
    * For instance, for most USB spectrometers, the hex values <tt>05 00</tt> in
    * the buffer means 'read EEPROM slot 0'. Writing this command, and following
    * it with the \c seabreeze_read_usb command and the same hex <tt>05 00</tt>
    * in the buffer, will result in the spectrometer's serial number, in ASCII
    * characters, being delivered to the subsequent bytes in the buffer array.
    * This would appear in the output buffer as:
    *
    * \code
    *   05 00 55 53 42 32 2B 48 30 31 34 31 36 00 00 00 00 (hex)
    * \endcode
    *
    * (<tt>USB2+H01416</tt> ASCII, skipping the two command bytes)
    *
    * A C or C++ program could use the data as-is, but a C# program could
    * append to a string, each byte, cast as char, stopping on the first null
    * character:
    *
    * \code
    *   string serial = "";
    *   for (int i = 0; i < length && buffer[i] != '\0'; i++)
    *     serial += (char)buffer[i];
    * \endcode
    *
    * This is equivalent to:
    *
    * \code
    *   string serial = System.Text.ASCIIEncoding.ASCII.GetString(buffer).Trim('\0');
    * \endcode
    *
    * For C# and VB.NET, the trailing zero-fill must be removed. Those null
    * characters would not print or display on a screen but would make a file
    * name invalid.
    */
    DLL_DECL int seabreeze_write_usb(int index, int *errorCode, unsigned char endpoint, unsigned char* buffer, unsigned int length);

    /**
    * @brief Read a raw array of bytes from a USB spectrometer.
    * @param index (Input) index of a previously opened spectrometer
    * @param errorCode (Output) pointer to an allocated integer field for receiving error codes
    * @param endpoint (Input) USB endpoint for read operation
    *        (see src/vendors/OceanOptics/features/raw_bus_access/RawUSBBusAccessFeature.cpp)
    * @param buffer (Input) array of allocated bytes at which to recieve USB data
    * @param length (Input) maximum number of bytes to read from spectrometer
    * @return number of received bytes written to buffer
    *
    * Read bytes from the spectrometer into the buffer. The buffer starts with
    * two bytes of command information that will be followed by the transferred
    * bytes. The read and write USB functions allow low-level control of the
    * spectrometer via USB commands. The higher-level SeaBreeze functions issue
    * USB commands internally.
    *
    * Reading USB data takes two steps. First a seabreeze_write_usb call requests
    * the transfer, then a seabreeze_read_usb call delivers the data. Writing a buffer
    * to USB takes one step. The operating system handle for the spectrometer is
    * managed by SeaBreeze. USB has a concept of "end points". These are implicitly
    * addressed by the seabreeze_read_usb and seabreeze_write_usb functions.
    */
    DLL_DECL int seabreeze_read_usb(int index, int *errorCode, unsigned char endpoint, unsigned char* buffer, unsigned int length);

    /**
    * @brief Get the SeaBreeze library's internal version identifier.
    * @param buffer (Output) pointer to an allocated character array
    *                        to hold the returned version string
    * @param len    (Input)  size of the allocated buffer
    * @return number of bytes written to buffer
    */
    DLL_DECL int seabreeze_get_api_version_string(char *buffer, int len);

    /**
    * @brief Get a USB descriptor string by number
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param errorCode (Output) pointer to allocated integer to receive error code
    * @param id        (Input)  numeric ID of the desired USB descriptor string
    * @param buffer    (Output) pointer to an allocated character array to hold
    *                           the returned descriptor string
    * @param len       (Input)  size of the allocated buffer
    * @return number of bytes written to buffer
    */
    DLL_DECL int seabreeze_get_usb_descriptor_string(int index, int *errorCode, int id, unsigned char *buffer, int len);

    /**
    * @brief Set the continuous strobe period in microseconds
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param errorCode (Output) pointer to allocated integer to receive error code
    * @param strobe_id (Input) index of the strobe generator (currently always zero)
    * @param period_usec (Input) total period of the strobe, in microseconds
    * @return no return
    *
    * The resolution is 0.1 milliseconds (100 microseconds).
    */
    DLL_DECL void seabreeze_set_continuous_strobe_period_microsec(int index, int *errorCode, unsigned short strobe_id, unsigned long period_usec);

    /**
    * @brief Set the acquisition delay (trigger delay) in microseconds.  This
    *        controls the amount of time between a particular event
    *        (usually a request for spectrum or an external trigger pulse)
    *        and the start of acquisition.
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param errorCode (Output) pointer to allocated integer to receive error code
	* @param delay_usec (Input) total delay before acquisition, in microseconds
	* @return no return
	*
    */
    DLL_DECL void seabreeze_set_acquisition_delay_microsec(int index, int *errorCode, unsigned long delay_usec);

    /**
    * @brief Clear the spectrum buffer (if equipped)
    * @param index (Input) Which spectrometer should have its buffer cleared
    * @param error_code (Output) Pointer to allocated integer to receive error code
	* @return no return
    */
    DLL_DECL void seabreeze_clear_buffer(int index, int *error_code);

    /**
    * @brief remove the oldest spectrum from the buffer (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	+ @param numberOfSpectra (Input) how many of the oldest spectra to be removed
    * @param error_code (Output) Pointer to allocated integer to receive error code
    */
    DLL_DECL void seabreeze_remove_oldest_spectra_from_buffer(int index, int *error_code, unsigned int numberOfSpectra);

    /**
    * @brief Get the number of spectra presently in the buffer (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @return Number of spectra in the buffer
    */
    DLL_DECL unsigned long seabreeze_get_buffer_element_count(int index, int *error_code);

    /**
    * @brief Get the currently configured size of the data buffer (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @return The present limit on the number of spectra that will be retained.
    */
    DLL_DECL unsigned long seabreeze_get_buffer_capacity(int index, int *error_code);

    /**
    * @brief Get buffer enable value (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @return The value of the buffering enable bit
    */
    DLL_DECL unsigned char seabreeze_get_buffering_enable(int index, int *error_code);
    
    /**
    * @brief Get the maximum possible configurable size for the data buffer (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @return Maximum allowed value for the buffer size
    */
    DLL_DECL unsigned long seabreeze_get_buffer_capacity_maximum(int index, int *error_code);

    /**
    * @brief Get the minimum possible configurable size for the data buffer (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @return Minimum allowed value for the buffer size
    */
    DLL_DECL unsigned long seabreeze_get_buffer_capacity_minimum(int index, int *error_code);

    /**
    * @brief Set the number of spectra that the buffer should keep
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @param capacity (Input) Limit on number of spectra to store.
    *        Note that this must be within the range defined by the capacity minimum
    *        and maximum values.
    */
    DLL_DECL void seabreeze_set_buffer_capacity(int index, int *error_code, unsigned long capacity);

    /**
    * @brief Set the buffering enable bit
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @param isEnabled (Input) The state of the buffering enable bit
    */
    DLL_DECL void seabreeze_set_buffering_enable(int index, int *error_code, unsigned char isEnabled);

    /**
    * @brief Get GbE enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @return The GbE enable state
    */
    DLL_DECL unsigned char seabreeze_get_gbe_enable(int index, int *error_code, unsigned char interfaceIndex);
    
        /**
    * @brief Set GbE enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param GbE_Enable (Input) The GbE enable state from the indicated interface
    * @param error_code (Output) Pointer to allocated integer to receive error code
    */
    DLL_DECL void seabreeze_set_gbe_enable(int index, int *error_code, unsigned char interfaceIndex, unsigned char GbE_Enable);
    
        /**
    * @brief Get the MAC address (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
    * @param macAddress (Output) Pointer to allocated six byte unsigned char array to receive the MAC Address
    */
    	// no c++11 yet 
    DLL_DECL void seabreeze_get_mac_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char (*macAddress)[6]);

    //DLL_DECL void seabreeze_get_mac_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char (&macAddress)[6]);
    
        /**
    * @brief Set the MAC address (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param macAddress (Input) A pointer to a six byte unsigned char array to set the MAC address for the indicated interface
    * @param error_code (Output) Pointer to allocated integer to receive error code
    */
    DLL_DECL void seabreeze_set_mac_address(int index, int *error_code, unsigned char interfaceIndex, const unsigned char macAddress[6]);




	/**
	* @brief Get the wifi access mode, 0: client, 1: access point
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return The wifi access mode
	*/
	DLL_DECL unsigned char seabreeze_get_wifi_mode(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Set the wifi access mode,  0: client, 1: access point
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param wifi Mode (Input) The wifi mode for the indicated interface
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_wifi_mode(int index, int *error_code, unsigned char interfaceIndex, unsigned char wifiMode);

	/**
	* @brief Get the wifi security type, 0: open, 1: WPA2
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return The wifi security type
	*/
	DLL_DECL unsigned char seabreeze_get_wifi_security_type(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Set the wifi security type,  0: open, 1: WPA2
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param secuirty type  (Input) The wifi mode for the indicated interface
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_wifi_security_type(int index, int *error_code, unsigned char interfaceIndex, unsigned char securityType);

	/**
	* @brief Get the wifi ssid (up to 32 bytes)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param ssid (Output) Pointer to allocated 32 byte unsigned char array to receive the ssid
	* @return the number of bytes used in the 32 byte array
	*/
	
	// no c++11 yet 
	DLL_DECL unsigned char seabreeze_get_wifi_ssid(int index, int *error_code, unsigned char interfaceIndex, unsigned char(*ssid)[32]);
	//DLL_DECL unsigned char seabreeze_get_wifi_ssid(int index, int *error_code, unsigned char interfaceIndex, unsigned char(&ssid)[32]);

	/**
	* @brief Set the wifi ssid 
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param ssid (Input) A pointer to a 32 byte unsigned char array to set the ssid for the indicated interface
	* @param length (Input) the actual size of the SSID
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_wifi_ssid(int index, int *error_code, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length);

	/**
	* @brief Set the wifi passphrase
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param pass phrase (Input) A pointer to a char array to set the pass phrase for the indicated interface
	* @param length (Input) length of the pass phrase
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_wifi_pass_phrase(int index, int *error_code, unsigned char interfaceIndex, const unsigned char *passphrase, unsigned char length);






	/**
	* @brief Get multicast enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return The multicast enable state
	*/
	DLL_DECL unsigned char seabreeze_get_multicast_enable(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Set multicat enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param GbE_Enable (Input) The GbE enable state from the indicated interface
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_multicast_enable(int index, int *error_code, unsigned char interfaceIndex, unsigned char multicastEnable);

	/**
	* @brief Get the hard coded multicast group address (239.239.239.239). In the future this can be modified
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param macAddress (Output) Pointer to allocated six byte unsigned char array to receive the multicast group Address
	*/
	// no c++11 yet
	//DLL_DECL void seabreeze_get_multicast_group_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char(&groupAddress)[4]);
	DLL_DECL void seabreeze_get_multicast_group_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char(*groupAddress)[4]);


	/**
	* @brief Get the hard coded multicast group port (57357) In the future this will can be modified
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return The multicast enable state
	*/
	DLL_DECL unsigned short seabreeze_get_multicast_group_port(int index, int *error_code, unsigned char interfaceIndex);



	/**
	* @brief Get the number of network interfaces (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return The number of network interfaces available
	*/
	DLL_DECL unsigned char seabreeze_get_number_of_network_interfaces(int index, int *error_code);

	/**
	* @brief Get the network interface type, 0: loopback, 1: wired ethernet, 2: wifi, 3: cdc ethernet (usb) 
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param interfaceIndex (Input) The index number for the network interface of interest
	* @return The network interface type
	*/
	DLL_DECL unsigned char seabreeze_get_network_interface_connection_type(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Get the network interface type (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param interfaceIndex (Input) The index number for the network interface of interest
	* @return Network interface type, 0: loopback, 1: wired ethernet, 2: wifi, 3: cdc ethernet (usb) 
	*/
	DLL_DECL unsigned char seabreeze_get_network_interface_enable_state(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Set the network interface enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param interfaceIndex (Input) The index number for the network interface of interest
	* @return no return
	*/
	DLL_DECL void seabreeze_set_network_interface_enable_state(int index, int *error_code, unsigned char interfaceIndex, unsigned char enableState);

	/**
	* @brief Run a self test on the indicated network interface (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param interfaceIndex (Input) The index number for the network interface of interest
	* @return SElf test result, pass = 1, fail = 0
	*/
	DLL_DECL unsigned char seabreeze_run_network_interface_self_test(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Save all of the network interface settings
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param interfaceIndex (Input) The index number for the network interface of interest
	* @return no return
	*/
	DLL_DECL void seabreeze_save_network_interface_connection_settings(int index, int *error_code, unsigned char interfaceIndex);



	/**
	* @brief Get DHCP Server enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return The dhcp server enable state
	*/
	DLL_DECL unsigned char seabreeze_get_dhcp_server_enable_state(int index, int *error_code, unsigned char interfaceIndex);

	/**
	* @brief Set dhcp server enable state (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param serverEnable (Input) The GbE enable state from the indicated interface
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_dhcp_server_enable_state(int index, int *error_code, unsigned char interfaceIndex, unsigned char serverEnable);

	/**
	* @brief Get the dhcp server address and net mask(if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param serverAddress (Output) Pointer to allocated four byte unsigned char array to receive the server Address
	* @param netMask (Output) Pointer to an unsigned char to receive the network mask
	*/
	// no c++11 yet
	//DLL_DECL void seabreeze_get_dhcp_server_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char(&serverAddress)[4], unsigned char &netMask);
	DLL_DECL void seabreeze_get_dhcp_server_address(int index, int *error_code, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask);

	/**
	* @brief Set the dhcp server address (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param serverAddress (Input) a four byte unsigned char array to set the server address for the indicated interface
	* @param netMask (Input) an unsigned char to set the network mask for the indicated interface
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_dhcp_server_address(int index, int *error_code, unsigned char interfaceIndex, const unsigned char serverAddress[4], const unsigned char netMask);






	/**
	* @brief get number of gpio pins (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the number of gpio pins
	*/
	DLL_DECL unsigned char seabreeze_get_gpio_number_of_pins(int index, int *error_code);


	/**
	* @brief get bits that represent gpio output enable (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the output enable vector
	*/
	DLL_DECL unsigned int seabreeze_get_gpio_output_enable_vector(int index, int *error_code);

	/**
	* @brief Set bits that control  gpio output enable (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param outputEnableVector (Input) The GbE enable state from the indicated interface
	* @param bit Mask (input) for the output enable vector.
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_gpio_output_enable_vector(int index, int *error_code, unsigned int outputEnableVector, unsigned bitMask);


	/**
	* @brief get bits that represent the gpio bit values (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the output enable vector
	*/
	DLL_DECL unsigned int seabreeze_get_gpio_value_vector(int index, int *error_code);

	/**
	* @brief Set bits that control  the gpio bit values (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param value (Input) The GbE enable state from the indicated interface
	* @param bit Mask (input) for the output enable vector.
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_gpio_value_vector(int index, int *error_code, unsigned int value, unsigned bitMask);

	/**
	* @brief get number of extended gpio pins (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the number of gpio pins
	*/
	DLL_DECL unsigned char seabreeze_get_egpio_number_of_pins(int index, int *error_code);

	/**
	* @brief Returns the available gpio modes for the pin of interest
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param buffer (Output) A character buffer allocated to contain at least 'maxModeCount' items, which will be populated with the mode list
	* @param maxModeCount (Input) number of Modes that can be stored
	* @return int: Number of modes written to buffer.
	*/
	DLL_DECL unsigned char seabreeze_get_egpio_available_modes(int index, int error_code, unsigned char pinNumber, unsigned char *availableModes, unsigned char maxModeCount);

	/**
	* @brief get a mode type for the pin of interest (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the mode identifier, 0x00: GPIO Output (Push/Pull), 0x01: GPIO Open Drain Output
	*     0x02: DAC output, 0x80: GPIO Input (High Z), 0x81: GPIO Input w/Pull Down, 0x82: ADC Input
	*/
	DLL_DECL unsigned int seabreeze_get_egpio_current_mode(int index, int *error_code, unsigned char pinNumber);

	/**
	* @brief Set bits that control  the gpio bit values (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @param pinNumber (Input) the pin of interest
	* @param value (Input) optional value from 0.0 to 1.0
	* @param mode (Input)  the mode identifier, 0x00: GPIO Output (Push/Pull), 0x01: GPIO Open Drain Output
	*     0x02: DAC output, 0x80: GPIO Input (High Z), 0x81: GPIO Input w/Pull Down, 0x82: ADC Input
	*/
	DLL_DECL void seabreeze_set_egpio_mode(int index, int *error_code, unsigned char pinNumber, unsigned char mode, float value);


	/**
	* @brief get bits that represent the egpio bit values (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the output  vector
	*/
	DLL_DECL unsigned int seabreeze_get_egpio_output_vector(int index, int *error_code);

	/**
	* @brief Set bits that control  the egpio bit values (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param value (Input) The GbE enable state from the indicated interface
	* @param bit Mask (input) for the output enable vector.
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_egpio_output_vector(int index, int *error_code, unsigned int value, unsigned bitMask);



	/**
	* @brief get value for the given pin, 0.0 to 1.0 (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param pinNumber (Input) the pin of interest
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the output value
	*/
	DLL_DECL float seabreeze_get_egpio_value(int index, int *error_code, unsigned pinNumber);


	/**
	* @brief Set value for the bit of interest, 0.0 to 1.0 (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param value (Input) a normalized value
	* @param pinNumber (input) the pin of interest
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL void seabreeze_set_egpio_value(int index, int *error_code, unsigned pinNumber, float value);


	/**
	* @brief get the number of i2c buses (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param error_code (Output) Pointer to allocated integer to receive error code
	* @return the number of i2c buses
	*/
	DLL_DECL unsigned char getI2CMasterNumberOfBuses(int index, int *errorCode);


	/**
	* @brief receive  i2c data (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param busIndex (Input) the index for the i2c bus of interest
	* @param slaveAddress (Input) the i2c address of the device to be queried
	* @param readData (Input) pointer to the data that should be received
	* @param numberOfBytes (Input) the number of bytes to retrieve
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL unsigned short readI2CMasterBus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes);
	
	/**
	* @brief send  i2c data (if equipped)
	* @param index (Input) The index of a device previously opened with open_spectrometer().
	* @param busIndex (Input) the index for the i2c bus of interest
	* @param slaveAddress (Input) the i2c address of the device to be written to
	* @param writeData (Input) pointer to the data that should be sent
	* @param numberOfBytes (Input) the number of bytes to send
	* @param error_code (Output) Pointer to allocated integer to receive error code
	*/
	DLL_DECL unsigned short writeI2CMasterBus(int index, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes);





    /**
    * @brief Programmatically enable debug outputs to stderr
    * @param flag (Input) zero to disable (default), non-zero to enable
    */
    DLL_DECL void seabreeze_set_verbose(int flag);

    /**
    * @brief redirect verbose logging to named file
    * @param flag (Input) NULL for default behavior (stderr), non-null for valid OS path
    */
    DLL_DECL void seabreeze_set_logfile(char* pathname, int len);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* SEABREEZE_WRAPPER_H */
