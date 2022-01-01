# cseabreeze cython headers
# =========================
#
# wrapping the cpp class interface directly
#
# Author: Andreas Poehlmann (andreas@poehlmann.io)
# License: MIT
# seabreeze source: https://github.com/ap--/libseabreeze/tree/libseabreeze-3.0.11.11
#
#
from libcpp cimport bool


cdef extern from "api/USBEndpointTypes.h":
    cpdef enum usbEndpointType:
        kEndpointTypePrimaryOut, # slow speed
        kEndpointTypePrimaryIn,  # slow speed
        kEndpointTypeSecondaryOut, # could be high speed
        kEndpointTypeSecondaryIn,  # could be high speed
        kEndpointTypeSecondaryIn2  # generally high speed


cdef extern from "api/seabreezeapi/SeaBreezeAPI.h":
    # noinspection PyPep8Naming,PyShadowingBuiltins
    cdef cppclass SeaBreezeAPI:

        @staticmethod
        SeaBreezeAPI* getInstance() except +
        @staticmethod
        void shutdown() except +

        int probeDevices()
        int addTCPIPv4DeviceLocation(char *deviceTypeName, char *ipAddr, int port)
        int addRS232DeviceLocation(char *deviceTypeName, char *deviceBusPath, unsigned int baud)
        int getNumberOfDeviceIDs()
        int getDeviceIDs(long *ids, unsigned long maxLength)
        int getDeviceType(long id, int *errorCode, char *buffer, unsigned int length)

        int getNumberOfSupportedModels()
        int getSupportedModelName(int index, int *errorCode, char *buffer, int bufferLength)

        int openDevice(long id, int *errorCode)
        void closeDevice(long id, int *errorCode)

        # Serial number capabilities
        int getNumberOfSerialNumberFeatures(long deviceID, int *errorCode)
        int getSerialNumberFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        int getSerialNumber(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
        unsigned char getSerialNumberMaximumLength(long deviceID, long featureID, int *errorCode)

        # Get raw usb access capabilities
        int getNumberOfRawUSBBusAccessFeatures(long deviceID, int *errorCode)
        int getRawUSBBusAccessFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char getDeviceEndpoint(long id, int *error_code, usbEndpointType endpointType)
        int rawUSBBusAccessRead(long deviceID, long featureID, int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint)
        int rawUSBBusAccessWrite(long deviceID, long featureID, int *errorCode, unsigned char *buffer, unsigned int bufferLength, unsigned char endpoint)


        # Spectrometer capabilities
        int getNumberOfSpectrometerFeatures(long id, int *errorCode)
        int getSpectrometerFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void spectrometerSetTriggerMode(long deviceID, long spectrometerFeatureID, int *errorCode, int mode) nogil
        void spectrometerSetIntegrationTimeMicros(long deviceID, long spectrometerFeatureID, int *errorCode, unsigned long integrationTimeMicros) nogil
        unsigned long spectrometerGetMinimumIntegrationTimeMicros(long deviceID, long spectrometerFeatureID, int *errorCode)
        unsigned long spectrometerGetMaximumIntegrationTimeMicros(long deviceID, long spectrometerFeatureID, int *errorCode)
        double spectrometerGetMaximumIntensity(long deviceID, long spectrometerFeatureID, int *errorCode)
        # int spectrometerGetUnformattedSpectrumLength(long deviceID, long spectrometerFeatureID, int *errorCode)
        # int spectrometerGetUnformattedSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode, unsigned char *buffer, int bufferLength)
        # int spectrometerGetFastBufferSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode, unsigned char *dataBuffer, int dataMaxLength, unsigned int numberOfSampleToRetrieve) # currently 15 max
        int spectrometerGetFormattedSpectrumLength(long deviceID, long spectrometerFeatureID, int *errorCode)
        int spectrometerGetFormattedSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode, double *buffer, int bufferLength) nogil
        int spectrometerGetWavelengths(long deviceID, long spectrometerFeatureID, int *errorCode, double *wavelengths, int length) nogil
        int spectrometerGetElectricDarkPixelCount(long deviceID, long spectrometerFeatureID, int *errorCode)
        int spectrometerGetElectricDarkPixelIndices(long deviceID, long spectrometerFeatureID, int *errorCode, int *indices, int length)

        # Pixel binning capabilities
        int getNumberOfPixelBinningFeatures(long id, int *errorCode)
        int getPixelBinningFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void binningSetPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode, const unsigned char binningFactor)
        unsigned char binningGetPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode)
        void binningSetDefaultPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode, const unsigned char binningFactor)
        void binningSetDefaultPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode)
        unsigned char binningGetDefaultPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode)
        unsigned char binningGetMaxPixelBinningFactor(long deviceID, long spectrometerFeatureID, int *errorCode)

        # TEC capabilities
        int getNumberOfThermoElectricFeatures(long deviceID, int *errorCode)
        int getThermoElectricFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        double tecReadTemperatureDegreesC(long deviceID, long featureID, int *errorCode)
        void tecSetTemperatureSetpointDegreesC(long deviceID, long featureID, int *errorCode, double temperatureDegreesCelsius)
        void tecSetEnable(long deviceID, long featureID, int *errorCode, unsigned char tecEnable)

        # Irradiance calibration features
        int getNumberOfIrradCalFeatures(long deviceID, int *errorCode)
        int getIrradCalFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        int irradCalibrationRead(long deviceID, long featureID, int *errorCode, float *buffer, int bufferLength)
        int irradCalibrationWrite(long deviceID, long featureID, int *errorCode, float *buffer, int bufferLength)
        int irradCalibrationHasCollectionArea(long deviceID, long featureID, int *errorCode)
        float irradCalibrationReadCollectionArea(long deviceID, long featureID, int *errorCode)
        void irradCalibrationWriteCollectionArea(long deviceID, long featureID, int *errorCode, float area)

        # Ethernet Configuration features
        int getNumberOfEthernetConfigurationFeatures(long deviceID, int *errorCode)
        int getEthernetConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void ethernetConfiguration_Get_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char (*macAddress)[6])
        void ethernetConfiguration_Set_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6])
        unsigned char ethernetConfiguration_Get_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void ethernetConfiguration_Set_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)

        # Multicast features
        int getNumberOfMulticastFeatures(long deviceID, int *errorCode)
        int getMulticastFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        # void getMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(&macAddress)[6])
        # void setMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char macAddress[6])
        unsigned char getMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void setMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)

        # IPv4 features
        int getNumberOfIPv4Features(long deviceID, int *errorCode)
        int getIPv4Features(long deviceID, int *errorCode, long *buffer, int maxLength)
        unsigned char get_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void set_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char isEnabled)
        unsigned char get_Number_Of_IPv4_Addresses(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void get_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask)
        void get_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*defaultGatewayAddress)[4])
        void set_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char defaultGatewayAddress[4])
        void add_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char IPv4_Address[4], unsigned char netMask)
        void delete_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char addressIndex)

        # DHCP server features
        int getNumberOfDHCPServerFeatures(long deviceID, int *errorCode)
        int getDHCPServerFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void dhcpServerGetAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*serverAddress)[4], unsigned char *netMask)
        void dhcpServerSetAddress(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char serverAddress[4], unsigned char netMask)
        unsigned char dhcpServerGetEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void dhcpServerSetEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)

        # Network Configuration features
        int getNumberOfNetworkConfigurationFeatures(long deviceID, int *errorCode)
        int getNetworkConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char getNumberOfNetworkInterfaces(long deviceID, long featureID, int *errorCode)
        unsigned char getNetworkInterfaceConnectionType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        unsigned char getNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        unsigned char runNetworkInterfaceSelfTest(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void setNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char enableState)
        void saveNetworkInterfaceConnectionSettings(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)

        # wifi configuration features
        int getNumberOfWifiConfigurationFeatures(long deviceID, int *errorCode)
        int getWifiConfigurationFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char getWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void setWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char mode)
        unsigned char getWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex)
        void setWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char securityType)
        unsigned char getWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, unsigned char(*ssid)[32])
        void setWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char ssid[32], unsigned char length)
        void setWifiConfigurationPassPhrase(long deviceID, long featureID, int *errorCode, unsigned char interfaceIndex, const unsigned char *passPhrase, unsigned char passPhraseLength)

        # gpio features
        int getNumberOfGPIOFeatures(long deviceID, int *errorCode)
        int getGPIOFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char getGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode)
        unsigned int getGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode)
        void setGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask)
        unsigned int getGPIO_ValueVector(long deviceID, long featureID, int *errorCode)
        void setGPIO_ValueVector(long deviceID, long featureID, int *errorCode, unsigned int valueVector, unsigned int bitMask)
        unsigned char getEGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode)
        unsigned char getEGPIO_AvailableModes(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char *availableModes, unsigned char maxModeCount)
        unsigned char getEGPIO_CurrentMode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber)
        void setEGPIO_Mode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char mode, float value)
        unsigned int getEGPIO_OutputVector(long deviceID, long featureID, int *errorCode)
        void setEGPIO_OutputVector(long deviceID, long featureID, int *errorCode, unsigned int outputVector, unsigned int bitMask)
        float getEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber)
        void setEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, float value)

        # EEPROM capabilities
        int getNumberOfEEPROMFeatures(long deviceID, int *errorCode)
        int getEEPROMFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        int eepromReadSlot(long deviceID, long featureID, int *errorCode, int slotNumber, unsigned char *buffer, int bufferLength)

        # Light source capabilities
        int getNumberOfLightSourceFeatures(long deviceID, int *errorCode)
        int getLightSourceFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        int lightSourceGetCount(long deviceID, long featureID, int *errorCode)
        bool lightSourceHasEnable(long deviceID, long featureID, int *errorCode, int lightSourceIndex)
        bool lightSourceIsEnabled(long deviceID, long featureID, int *errorCode, int lightSourceIndex)
        void lightSourceSetEnable(long deviceID, long featureID, int *errorCode, int lightSourceIndex, bool enable)
        bool lightSourceHasVariableIntensity(long deviceID, long featureID, int *errorCode, int lightSourceIndex)
        double lightSourceGetIntensity(long deviceID, long featureID, int *errorCode, int lightSourceIndex)
        void lightSourceSetIntensity(long deviceID, long featureID, int *errorCode, int lightSourceIndex, double intensity)

        # Lamp capabilities
        int getNumberOfLampFeatures(long deviceID, int *errorCode)
        int getLampFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void lampSetLampEnable(long deviceID, long featureID, int *errorCode, bool strobeEnable)

        # Continuous strobe capabilities
        int getNumberOfContinuousStrobeFeatures(long deviceID, int *errorCode)
        int getContinuousStrobeFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void continuousStrobeSetContinuousStrobeEnable(long deviceID, long featureID, int *errorCode, bool strobeEnable)
        void continuousStrobeSetContinuousStrobePeriodMicroseconds(long deviceID, long featureID, int *errorCode, unsigned long strobePeriodMicroseconds)

        # Shutter capabilities
        int getNumberOfShutterFeatures(long deviceID, int *errorCode)
        int getShutterFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void shutterSetShutterOpen(long deviceID, long featureID, int *errorCode, bool opened)

        # Nonlinearity coefficient capabilities
        int getNumberOfNonlinearityCoeffsFeatures(long deviceID, int *errorCode)
        int getNonlinearityCoeffsFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        int nonlinearityCoeffsGet(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength)

        # Temperature capabilities
        int getNumberOfTemperatureFeatures(long deviceID, int *errorCode)
        int getTemperatureFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char temperatureCountGet(long deviceID, long featureID, int *errorCode)
        double temperatureGet(long deviceID, long featureID, int *errorCode, int index)
        int temperatureGetAll(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength)

        # Introspection capabilities
        int getNumberOfIntrospectionFeatures(long deviceID, int *errorCode)
        int getIntrospectionFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned short int introspectionNumberOfPixelsGet(long deviceID, long featureID, int *errorCode)
        int introspectionActivePixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength)
        int introspectionOpticalDarkPixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength)
        int introspectionElectricDarkPixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength)


        # Spectrum processing capabilities
        int getNumberOfSpectrumProcessingFeatures(long deviceID, int *errorCode)
        int getSpectrumProcessingFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char spectrumProcessingBoxcarWidthGet(long deviceID, long featureID, int *errorCode)
        unsigned short int spectrumProcessingScansToAverageGet(long deviceID, long featureID, int *errorCode)
        void spectrumProcessingBoxcarWidthSet(long deviceID, long featureID, int *errorCode, unsigned char boxcarWidth)
        void spectrumProcessingScansToAverageSet(long deviceID, long featureID, int *errorCode, unsigned short int scansToAverage)

        # Revision capabilities
        int getNumberOfRevisionFeatures(long deviceID, int *errorCode)
        int getRevisionFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char revisionHardwareGet(long deviceID, long featureID, int *errorCode)
        unsigned short int revisionFirmwareGet(long deviceID, long featureID, int *errorCode)

        # Optical Bench capabilities
        int getNumberOfOpticalBenchFeatures(long deviceID, int *errorCode)
        int getOpticalBenchFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned short int opticalBenchGetFiberDiameterMicrons(long deviceID, long featureID, int *errorCode)
        unsigned short int opticalBenchGetSlitWidthMicrons(long deviceID, long featureID, int *errorCode)
        int opticalBenchGetID(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
        int opticalBenchGetSerialNumber(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
        int opticalBenchGetCoating(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
        int opticalBenchGetFilter(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
        int opticalBenchGetGrating(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)

        # Stray light coefficient capabilities
        int getNumberOfStrayLightCoeffsFeatures(long deviceID, int *errorCode)
        int getStrayLightCoeffsFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        int strayLightCoeffsGet(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength)

        # Data buffer capabilities
        int getNumberOfDataBufferFeatures(long deviceID, int *errorCode)
        int getDataBufferFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void dataBufferClear(long deviceID, long featureID, int *errorCode)
        void dataBufferRemoveOldestSpectra(long deviceID, long featureID, int *errorCode, unsigned int numberOfSpectra)
        unsigned long dataBufferGetNumberOfElements(long deviceID, long featureID, int *errorCode)
        unsigned long dataBufferGetBufferCapacity(long deviceID, long featureID, int *errorCode)
        unsigned long dataBufferGetBufferCapacityMaximum(long deviceID, long featureID, int *errorCode)
        unsigned long dataBufferGetBufferCapacityMinimum(long deviceID, long featureID, int *errorCode)
        void dataBufferSetBufferCapacity(long deviceID, long featureID, int *errorCode, unsigned long capacity)

        # Fast Buffer capabilities
        int getNumberOfFastBufferFeatures(long deviceID, int *errorCode)
        int getFastBufferFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char fastBufferGetBufferingEnable(long deviceID, long featureID, int *errorCode)
        void fastBufferSetBufferingEnable(long deviceID, long featureID, int *errorCode, unsigned char isEnabled)
        unsigned int fastBufferGetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode)
        void fastBufferSetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode, unsigned int consecutiveSampleCount)

        # Acquisition delay capabilities
        int getNumberOfAcquisitionDelayFeatures(long deviceID, int *errorCode)
        int getAcquisitionDelayFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        void acquisitionDelaySetDelayMicroseconds(long deviceID, long featureID, int *errorCode, unsigned long delay_usec)
        unsigned long acquisitionDelayGetDelayMicroseconds(long deviceID, long featureID, int *errorCode)
        unsigned long acquisitionDelayGetDelayIncrementMicroseconds(long deviceID, long featureID, int *errorCode)
        unsigned long acquisitionDelayGetDelayMaximumMicroseconds(long deviceID, long featureID, int *errorCode)
        unsigned long acquisitionDelayGetDelayMinimumMicroseconds(long deviceID, long featureID, int *errorCode)

        # i2c master features
        int getNumberOfI2CMasterFeatures(long deviceID, int *errorCode)
        int getI2CMasterFeatures(long deviceID, int *errorCode, long *buffer, unsigned int maxLength)
        unsigned char i2cMasterGetNumberOfBuses(long deviceID, long featureID, int *errorCode)
        unsigned short i2cMasterReadBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
        unsigned short i2cMasterWriteBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
