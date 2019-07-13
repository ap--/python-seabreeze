"""This is the cython Wrapper for the seabreeze library

Author: Andreas Poehlmann

"""
cimport c_seabreeze as csb
from cpython.mem cimport PyMem_Malloc, PyMem_Free

# from libseabreeze api/SeaBreezeConstants.h
class ErrorCode(object):
    SUCCESS = 0
    INVALID_ERROR = 1
    NO_DEVICE = 2
    FAILED_TO_CLOSE = 3
    NOT_IMPLEMENTED = 4
    FEATURE_NOT_FOUND = 5
    TRANSFER_ERROR = 6
    BAD_USER_BUFFER = 7
    INPUT_OUT_OF_BOUNDS = 8
    SPECTROMETER_SATURATED = 9
    VALUE_NOT_FOUND = 10
    VALUE_NOT_EXPECTED = 11
    INVALID_TRIGGER_MODE = 12


# define max length for some strings
DEF _MAXBUFLEN = 32
DEF _MAXDBUFLEN = 256


class SeaBreezeError(Exception):

    _error_msgs = (
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
    )

    def __init__(self, message=None, error_code=None):
        if error_code is not None:
            if -99999 < error_code < 0:
                # as defined in libseabreeze api/SeaBreezeAPI.cpp
                message = "System Error: {:d}".format(error_code)

            elif error_code >= len(self._error_msgs):
                # not a valid a seabreeze error code
                message = self._error_msgs[ErrorCode.INVALID_ERROR]
            else:
                # return a seabreeze error message
                message = self._error_msgs[error_code]

        elif message is not None:
            pass
        else:
            message = ""
        # Call the base class constructor with the parameters it needs
        super(SeaBreezeError, self).__init__(message)
        self.error_code = error_code


class SeaBreezeNumFeaturesError(SeaBreezeError):

    def __init__(self, feature, received_num, expected_num=1):
        message = ("This should not have happened. Apparently this device has "
                   "{expected_num:d} {feature:s} features. The code expects it "
                   "to have {received_num:d}. Please file a bug report including "
                   "a description of your device.").format(
            expected_num=expected_num,
            received_num=received_num,
            feature=feature
        )
        super(SeaBreezeNumFeaturesError, self).__init__(message)


cdef class SeaBreezeAPI(object):

    cdef csb.SeaBreezeAPI *sbapi

    def __init__(self, initialize=True):
        self.sbapi = NULL
        if initialize:
            self.initialize()

    def initialize(self):
        """initialize the api backend

        normally this function does not have to be called directly by the user
        """
        self.sbapi = csb.SeaBreezeAPI.getInstance()

    def shutdown(self):
        """shutdown the api backend

        normally this function does not have to be called directly by the user
        """
        csb.SeaBreezeAPI.shutdown()
        self.sbapi = NULL

    def list_devices(self):
        """returns available SeaBreezeDevices

        list all connected Ocean Optics devices supported
        by libseabreeze.

        Returns
        -------
        devices: list of SeaBreezeDevice
            connected Spectrometer instances
        """
        # Probe Devices on all Buses
        self.sbapi.probeDevices()

        cdef int num_devices
        cdef long* device_ids
        num_devices = self.sbapi.getNumberOfDeviceIDs()
        device_ids = <long*> PyMem_Malloc(num_devices * sizeof(long))

        self.sbapi.getDeviceIDs(device_ids, num_devices)

        devices = []

        for i in range(num_devices):
            handle = device_ids[i]
            dev = SeaBreezeDevice(handle)
            if dev.is_open:
                was_open_before = True
            else:
                was_open_before = False
                dev.open()
            model = dev.model
            serial = dev.serial_number
            if not was_open_before:
                dev.close()
            devices.append(dev)

        PyMem_Free(device_ids)
        return devices


cdef class SeaBreezeDevice(object):

    cdef public long handle
    cdef public str model, serial_number
    cdef csb.SeaBreezeAPI *sbapi

    def __init__(self, handle=None):
        if handle is None:
            raise SeaBreezeError("Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`.")
        self.sbapi = csb.SeaBreezeAPI.getInstance()
        self.handle = handle
        if self.is_open:
            self._get_info()
        else:
            self.model = "?"
            self.serial_number = "?"

    cdef _get_info(self):
        """populate model and serial_number attributes (internal)"""
        model = self.get_model()
        try:
            self.model = model
        except TypeError:
            self.model = model.encode("utf-8")
        serial_number = self.get_serial_number()
        try:
            self.serial_number = serial_number
        except TypeError:
            self.serial_number = serial_number.encode("utf-8")

    def __repr__(self):
        return "<SeaBreezeDevice %s:%s>" % (self.model, self.serial_number)

    def open(self):
        """open the spectrometer usb connection

        Returns
        -------
        None
        """
        cdef int error_code
        cdef int ret
        ret = self.sbapi.openDevice(self.handle, &error_code)
        if int(ret) > 0:
            raise SeaBreezeError(error_code=error_code)
        self._get_info()

    def close(self):
        """close the spectrometer usb connection

        Returns
        -------
        None
        """
        cdef int error_code
        # always returns 1
        self.sbapi.closeDevice(self.handle, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    @property
    def is_open(self):
        """returns if the spectrometer device usb connection is opened

        Returns
        -------
        bool
        """
        try:
            # this is a hack to figure out if the spectrometer is connected
            self.get_serial_number()
        except SeaBreezeError:
            return False
        else:
            return True

    def get_serial_number(self):
        """return the serial number string of the spectrometer

        Returns
        -------
        serial_number: str
        """
        cdef int error_code
        cdef int num_serial_number_features
        num_serial_number_features = self.sbapi.getNumberOfSerialNumberFeatures(self.handle, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_serial_number_features != 1:
            raise SeaBreezeNumFeaturesError("serial number", received_num=num_serial_number_features)

        cdef long feature_id
        self.sbapi.getSerialNumberFeatures(self.handle, &error_code, &feature_id, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        cdef unsigned char max_length
        max_length = self.sbapi.getSerialNumberMaximumLength(self.handle, feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        cdef char cbuf[_MAXBUFLEN]
        cdef int bytes_written
        bytes_written = self.sbapi.getSerialNumber(self.handle, feature_id, &error_code, cbuf, max_length)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        serial = cbuf[:bytes_written]
        return serial.decode("utf-8").rstrip('\x00')

    def get_model(self):
        """return the model string of the spectrometer

        Returns
        -------
        model: str
        """
        cdef int error_code
        cdef char cbuf[_MAXBUFLEN]
        cdef int bytes_written
        bytes_written = self.sbapi.getDeviceType(self.handle, &error_code, cbuf, _MAXBUFLEN)
        model = cbuf[:bytes_written]
        if model == "NONE":
            raise SeaBreezeError(error_code=error_code)
        return model.decode("utf-8")

    @property
    def features(self):
        features = {}
        # noinspection PyProtectedMember
        feature_registry = SeaBreezeFeature.get_feature_class_registry()
        for identifier, feature_class in feature_registry.items():
            feature_ids = feature_class.get_feature_ids_from_device(device_id=self.handle)
            features[identifier] = [feature_class(self.handle, feature_id) for feature_id in feature_ids]
        return features


cdef class SeaBreezeFeature(object):

    cdef public long device_id
    cdef public long feature_id
    cdef csb.SeaBreezeAPI *sbapi

    identifier = "base_feature"
    required = False

    def __cinit__(self, int device_id, int feature_id):
        self.sbapi = csb.SeaBreezeAPI.getInstance()

    def __init__(self, int device_id, int feature_id):
        if self.identifier == "base_feature":
            raise SeaBreezeError("Don't instantiate SeaBreezeFeature directly. Use derived feature classes.")
        self.device_id = device_id
        self.feature_id = feature_id

    def __repr__(self):
        return "<{} id={}>".format(self.__class__.__name__, self.feature_id)

    @classmethod
    def get_feature_class_registry(cls):
        # noinspection PyUnresolvedReferences
        return {feature_class.identifier: feature_class for feature_class in SeaBreezeFeature.__subclasses__()}

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        return []


class SeaBreezeRawUSBBusAccessFeature(SeaBreezeFeature):

    identifier = "raw_usb_bus_access"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfRawUSBBusAccessFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getRawUSBBusAccessFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeSpectrometerFeature(SeaBreezeFeature):

    identifier = "spectrometer"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfSpectrometerFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getSpectrometerFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezePixelBinningFeature(SeaBreezeFeature):

    identifier = "pixel_binning"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfPixelBinningFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getPixelBinningFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeThermoElectricFeature(SeaBreezeFeature):

    identifier = "thermo_electric"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfThermoElectricFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getThermoElectricFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeIrradCalFeature(SeaBreezeFeature):

    identifier = "irrad_cal"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIrradCalFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getIrradCalFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeEthernetConfigurationFeature(SeaBreezeFeature):

    identifier = "ethernet_configuration"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfEthernetConfigurationFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getEthernetConfigurationFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeMulticastFeature(SeaBreezeFeature):

    identifier = "multicast"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfMulticastFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getMulticastFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeIPv4Feature(SeaBreezeFeature):

    identifier = "ipv4"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIPv4Features(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getIPv4Features(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeDHCPServerFeature(SeaBreezeFeature):

    identifier = "dhcp_server"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfDHCPServerFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getDHCPServerFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeNetworkConfigurationFeature(SeaBreezeFeature):

    identifier = "network_configuration"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfNetworkConfigurationFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getNetworkConfigurationFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeWifiConfigurationFeature(SeaBreezeFeature):

    identifier = "wifi_configuration"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfWifiConfigurationFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getWifiConfigurationFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeGPIOFeature(SeaBreezeFeature):

    identifier = "gpio"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfGPIOFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getGPIOFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeEEPROMFeature(SeaBreezeFeature):

    identifier = "eeprom"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfEEPROMFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getEEPROMFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeLightSourceFeature(SeaBreezeFeature):

    identifier = "light_source"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfLightSourceFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getLightSourceFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeStrobeLampFeature(SeaBreezeFeature):

    identifier = "strobe_lamp"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfLampFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getLampFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids

    def enable_lamp(self, state):
        """sets the strobe enable on the spectrometer

        Parameters
        ----------
        state : bool
            on or off

        Returns
        -------
        None
        """
        cdef int error_code
        self.sbapi.lampSetLampEnable(self.device_id, self.feature_id, &error_code, bool(state))
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


class SeaBreezeContinuousStrobeFeature(SeaBreezeFeature):

    identifier = "continuous_strobe"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfContinuousStrobeFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getContinuousStrobeFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeShutterFeature(SeaBreezeFeature):

    identifier = "shutter"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfShutterFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getShutterFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids

    def open_shutter(self, state):
        """sets the shutter state on the device

        Parameters
        ----------
        state : bool
            open or close

        Returns
        -------
        None
        """
        cdef int error_code
        self.sbapi.shutterSetShutterOpen(self.device_id, self.feature_id, &error_code, bool(state))
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


class SeaBreezeNonlinearityCoeffsFeature(SeaBreezeFeature):

    identifier = "nonlinearity_coeffs"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfNonlinearityCoeffsFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getNonlinearityCoeffsFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids

    def get_nonlinearity_coefficients(self):
        """reads out nonlinearity coefficients from the device's internal memory

        Returns
        -------
        nonlinearity_coefficients: list of float
            nonlinearity coefficients
        """
        cdef int error_code
        cdef int values_written
        cdef double[_MAXBUFLEN] ccoeffs
        values_written = self.sbapi.nonlinearityCoeffsGet(self.device_id, self.feature_id, &error_code,
                                                          &ccoeffs[0], _MAXBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        coeffs = [float(ccoeffs[i]) for i in range(values_written)]
        return coeffs


class SeaBreezeTemperatureFeature(SeaBreezeFeature):

    identifier = "temperature"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfTemperatureFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getTemperatureFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeIntrospectionFeature(SeaBreezeFeature):

    identifier = "introspection"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIntrospectionFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getIntrospectionFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeSpectrumProcessingFeature(SeaBreezeFeature):

    identifier = "spectrum_processing"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfSpectrumProcessingFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getSpectrumProcessingFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeRevisionFeature(SeaBreezeFeature):

    identifier = "revision"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfRevisionFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getRevisionFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeOpticalBenchFeature(SeaBreezeFeature):

    identifier = "optical_bench"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfOpticalBenchFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getOpticalBenchFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeStrayLightCoeffsFeature(SeaBreezeFeature):

    identifier = "stray_light_coeffs"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfStrayLightCoeffsFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getStrayLightCoeffsFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids

    def get_stray_light_coefficients(self):
        """reads out stray light coefficients from the device's internal memory

        Returns
        -------
        stray_light_coefficients: list of float
            the stray light coefficients stored on the device
        """
        cdef int error_code
        cdef int values_written
        cdef double[_MAXDBUFLEN] ccoeffs
        values_written = self.sbapi.strayLightCoeffsGet(self.device_id, self.feature_id, &error_code,
                                                        &ccoeffs[0], _MAXDBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        coeffs = [float(ccoeffs[i]) for i in range(values_written)]
        return coeffs


class SeaBreezeDataBufferFeature(SeaBreezeFeature):

    identifier = "data_buffer"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfDataBufferFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getDataBufferFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeFastBufferFeature(SeaBreezeFeature):

    identifier = "fast_buffer"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfFastBufferFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getFastBufferFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeAcquisitionDelayFeature(SeaBreezeFeature):

    identifier = "acquisition_delay"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfAcquisitionDelayFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getAcquisitionDelayFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


class SeaBreezeI2CMasterFeature(SeaBreezeFeature):

    identifier = "i2c_master"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, long device_id):
        cdef int num_features
        cdef int error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfI2CMasterFeatures(device_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)
            return []
        feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
        sbapi.getI2CMasterFeatures(device_id, &error_code, feature_ids, num_features)
        py_feature_ids = []
        for i in range(num_features):
            feature_id = feature_ids[i]
            py_feature_ids.append(feature_id)
        PyMem_Free(feature_ids)
        return py_feature_ids


'''
def spectrometer_set_trigger_mode(SeaBreezeDevice device not None, long featureID, int mode):
    cdef int error_code
    with nogil:
        csb.sbapi_spectrometer_set_trigger_mode(device.handle, featureID, &error_code, mode)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def spectrometer_set_integration_time_micros(SeaBreezeDevice device not None, long featureID, unsigned long integration_time_micros):
    cdef int error_code
    with nogil:
        csb.sbapi_spectrometer_set_integration_time_micros(device.handle, featureID, &error_code, integration_time_micros)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def spectrometer_get_minimum_integration_time_micros(SeaBreezeDevice device not None, long featureID):
    cdef long min_integration_time
    cdef int error_code
    with nogil:
        min_integration_time = csb.sbapi_spectrometer_get_minimum_integration_time_micros(device.handle, featureID, &error_code)
    if min_integration_time < 0:
        raise SeaBreezeError(error_code=error_code)
    return min_integration_time

def spectrometer_get_formatted_spectrum_length(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef int spec_length
    with nogil:
        spec_length = csb.sbapi_spectrometer_get_formatted_spectrum_length(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return spec_length

@cython.boundscheck(False)
def spectrometer_get_formatted_spectrum(SeaBreezeDevice device not None, long featureID, double[::1] out):
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = out.size
    with nogil:
        bytes_written = csb.sbapi_spectrometer_get_formatted_spectrum(device.handle, featureID, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bytes_written

def spectrometer_get_unformatted_spectrum_length(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef int spec_length
    with nogil:
        spec_length = csb.sbapi_spectrometer_get_unformatted_spectrum_length(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return spec_length

@cython.boundscheck(False)
def spectrometer_get_unformatted_spectrum(SeaBreezeDevice device not None, long featureID, unsigned char[::1] out):
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = out.size
    with nogil:
        bytes_written = csb.sbapi_spectrometer_get_unformatted_spectrum(device.handle, featureID, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bytes_written

@cython.boundscheck(False)
def spectrometer_get_wavelengths(SeaBreezeDevice device not None, long featureID, double[::1] out):
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = out.size
    with nogil:
        bytes_written = csb.sbapi_spectrometer_get_wavelengths(device.handle, featureID, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bytes_written

def spectrometer_get_electric_dark_pixel_indices(SeaBreezeDevice device not None, long featureID):
    assert device.handle >= 0
    cdef int error_code
    cdef int dp_count
    with nogil:
        dp_count = csb.sbapi_spectrometer_get_electric_dark_pixel_count(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if dp_count == 0:
        return []
    cindices = <int*> PyMem_Malloc(dp_count * sizeof(int))
    with nogil:
        csb.sbapi_spectrometer_get_electric_dark_pixel_indices(device.handle, featureID, &error_code, &cindices[0], dp_count)
    indices = [int(cindices[i]) for i in range(dp_count)]
    PyMem_Free(cindices)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return indices


def light_source_get_count(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef int ls_count
    ls_count = csb.sbapi_light_source_get_count(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return int(ls_count)

def light_source_has_enable(SeaBreezeDevice device not None, long featureID, int light_source_index):
    cdef int error_code
    cdef unsigned char has_enable
    has_enable = csb.sbapi_light_source_has_enable(device.handle, featureID, &error_code, light_source_index)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bool(has_enable)

def light_source_is_enabled(SeaBreezeDevice device not None, long featureID, int light_source_index):
    cdef int error_code
    cdef unsigned char is_enabled
    is_enabled = csb.sbapi_light_source_is_enabled(device.handle, featureID, &error_code, light_source_index)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bool(is_enabled)

def light_source_set_enable(SeaBreezeDevice device not None, long featureID, int light_source_index, unsigned char enable):
    cdef int error_code
    csb.sbapi_light_source_set_enable(device.handle, featureID, &error_code, light_source_index, enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def light_source_has_variable_intensity(SeaBreezeDevice device not None, long featureID, int light_source_index):
    cdef int error_code
    cdef unsigned char has_vi
    has_vi = csb.sbapi_light_source_has_variable_intensity(device.handle, featureID, &error_code, light_source_index)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bool(has_vi)

def light_source_get_intensity(SeaBreezeDevice device not None, long featureID, int light_source_index):
    cdef int error_code
    cdef double intensity
    intensity = csb.sbapi_light_source_get_intensity(device.handle, featureID, &error_code, light_source_index)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return float(intensity)

def light_source_set_intensity(SeaBreezeDevice device not None, long featureID, int light_source_index, double intensity):
    cdef int error_code
    csb.sbapi_light_source_set_intensity(device.handle, featureID, &error_code, light_source_index, intensity)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def continuous_strobe_set_enable(SeaBreezeDevice device not None, long featureID, unsigned char strobe_enable):
    cdef int error_code
    csb.sbapi_continuous_strobe_set_continuous_strobe_enable(device.handle, featureID, &error_code, strobe_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def continuous_strobe_set_period_micros(SeaBreezeDevice device not None, long featureID, unsigned long period_micros):
    cdef int error_code
    csb.sbapi_continuous_strobe_set_continuous_strobe_period_micros(device.handle, featureID, &error_code, period_micros)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)


def eeprom_read_slot(SeaBreezeDevice device not None, long featureID, int slot_number):
    cdef int error_code
    cdef unsigned char cbuf[_MAXBUFLEN]
    cdef int bytes_written
    try:
        bytes_written = csb.sbapi_eeprom_read_slot(device.handle, featureID, &error_code, slot_number, cbuf, _MAXBUFLEN)
    except ValueError:
        raise SeaBreezeError("EEProm slot out of bounds.")
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return cbuf[:bytes_written]


def irrad_calibration_read(SeaBreezeDevice device not None, long featureID, float[::1] out):
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = out.size
    bytes_written = csb.sbapi_irrad_calibration_read(device.handle, featureID, &error_code, &out[0], out_length)
    if error_code != 0:
         raise SeaBreezeError(error_code=error_code)
    return bytes_written

def irrad_calibration_write(SeaBreezeDevice device not None, long featureID, float[::1] out):
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = out.size
    bytes_written = csb.sbapi_irrad_calibration_write(device.handle, featureID, &error_code, &out[0], out_length)
    if error_code != 0:
         raise SeaBreezeError(error_code=error_code)
    return bytes_written

def irrad_calibration_has_collection_area(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef int has_ca
    has_ca = csb.sbapi_irrad_calibration_has_collection_area(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bool(has_ca)

def irrad_calibration_read_collection_area(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef float area
    area = csb.sbapi_irrad_calibration_read_collection_area(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return area

def irrad_calibration_write_collection_area(SeaBreezeDevice device not None, long featureID, float area):
    cdef int error_code
    csb.sbapi_irrad_calibration_write_collection_area(device.handle, featureID, &error_code, area)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)


def tec_read_temperature_degrees_C(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef double temperature
    temperature = csb.sbapi_tec_read_temperature_degrees_C(device.handle, featureID, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return temperature

def tec_set_temperature_setpoint_degrees_C(SeaBreezeDevice device not None, long featureID, double temperature_degrees_celsius):
    cdef int error_code
    csb.sbapi_tec_set_temperature_setpoint_degrees_C(device.handle, featureID, &error_code, temperature_degrees_celsius)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def tec_set_enable(SeaBreezeDevice device not None, long featureID, unsigned char tec_enable):
    cdef int error_code
    csb.sbapi_tec_set_enable(device.handle, featureID, &error_code, tec_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)


def spectrum_processing_set_boxcar_width(SeaBreezeDevice device not None, long featureID, unsigned char boxcar_width):
    cdef int error_code
    with nogil:
        csb.sbapi_spectrum_processing_boxcar_width_set(device.handle, featureID, &error_code, boxcar_width)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def spectrum_processing_set_scans_to_average(SeaBreezeDevice device not None, long featureID, unsigned short int scans_to_average):
    cdef int error_code
    with nogil:
        csb.sbapi_spectrum_processing_scans_to_average_set(device.handle, featureID, &error_code, scans_to_average)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def spectrum_processing_get_boxcar_width(SeaBreezeDevice device not None, long featureID):
    cdef unsigned char boxcar_width
    cdef int error_code
    with nogil:
        boxcar_width = csb.sbapi_spectrum_processing_boxcar_width_get(device.handle, featureID, &error_code)
    if boxcar_width < 0:
        raise SeaBreezeError(error_code=error_code)
    return boxcar_width

def spectrum_processing_get_scans_to_average(SeaBreezeDevice device not None, long featureID):
    cdef unsigned short int scans_to_average
    cdef int error_code
    with nogil:
        scans_to_average = csb.sbapi_spectrum_processing_scans_to_average_get(device.handle, featureID, &error_code)
    if scans_to_average < 1:
        raise SeaBreezeError(error_code=error_code)
    return scans_to_average
    
'''


