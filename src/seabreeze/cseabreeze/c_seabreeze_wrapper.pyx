"""This is the cython Wrapper for the seabreeze library

Author: Andreas Poehlmann

"""
cimport c_seabreeze as csb
import numpy as np
cimport cython
from cpython.mem cimport PyMem_Malloc, PyMem_Free
from libcpp cimport bool as bool_t



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
                   "{received_num:d} {feature:s} features. The code expects it "
                   "to have {expected_num:d}. Please file a bug report including "
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

    def _list_device_ids(self):
        cdef int num_devices
        cdef long* device_ids
        cdef int found_devices
        self.sbapi.probeDevices()
        num_devices = self.sbapi.getNumberOfDeviceIDs()
        device_ids = <long*> PyMem_Malloc(num_devices * sizeof(long))
        found_devices = self.sbapi.getDeviceIDs(device_ids, num_devices)
        devices = []
        for i in range(found_devices):
            devices.append(int(device_ids[i]))
        PyMem_Free(device_ids)
        return devices

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
        device_ids = self._list_device_ids()
        devices = []
        for handle in device_ids:
            dev = SeaBreezeDevice(handle)
            if dev.is_open:
                was_open_before = True
            else:
                was_open_before = False
                try:
                    dev.open()
                except SeaBreezeError as err:
                    if err.error_code == ErrorCode.NO_DEVICE:
                        # device used by another thread?
                        continue
            model = dev.model
            serial = dev.serial_number
            if not was_open_before:
                dev.close()
            devices.append(dev)
        return devices


cdef class SeaBreezeDevice(object):

    cdef public long handle
    cdef public str model, serial_number
    cdef csb.SeaBreezeAPI *sbapi

    def __cinit__(self, handle):
        self.sbapi = csb.SeaBreezeAPI.getInstance()

    def __init__(self, handle=None):
        if handle is None:
            raise SeaBreezeError("Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`.")
        self.handle = handle
        if self.is_open:
            self._get_info()
        else:
            self.model = "?"
            self.serial_number = "?"

    def __dealloc__(self):
        cdef int error_code
        # always returns 1
        self.sbapi.closeDevice(self.handle, &error_code)

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
        if int(ret) > 0 or error_code != 0:
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
        except SeaBreezeError as err:

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
            feature_ids = feature_class.get_feature_ids_from_device(self)
            features[identifier] = [feature_class(self, feature_id) for feature_id in feature_ids]
        return features

    @property
    def f(self):
        class FeatureAccessHandler(object):
            def __init__(self, features):
                for identifier, features in features.items():
                    setattr(self, identifier, features[0] if features else None)
        return FeatureAccessHandler(self.features)


cdef class SeaBreezeFeature(object):

    cdef SeaBreezeDevice device
    cdef long device_id
    cdef public long feature_id
    cdef csb.SeaBreezeAPI *sbapi

    identifier = "base_feature"
    required = False

    def __cinit__(self, SeaBreezeDevice device, int feature_id):
        self.sbapi = csb.SeaBreezeAPI.getInstance()

    def __init__(self, SeaBreezeDevice device, int feature_id):
        if self.identifier == "base_feature":
            raise SeaBreezeError("Don't instantiate SeaBreezeFeature directly. Use derived feature classes.")
        self.device = device
        self.device_id = device.handle
        self.feature_id = feature_id

    def __repr__(self):
        return "<{}:{}:{} id={}>".format(self.__class__.__name__,
                                         self.device.model, self.device.serial_number, self.feature_id)

    @classmethod
    def get_feature_class_registry(cls):
        # noinspection PyUnresolvedReferences
        return {feature_class.identifier: feature_class for feature_class in SeaBreezeFeature.__subclasses__()}

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):
        return []

    @classmethod
    def _raise_if_error(cls, error_code, num_features):
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            if cls.required:
                raise SeaBreezeNumFeaturesError(cls.identifier, num_features)


cdef class SeaBreezeRawUSBBusAccessFeature(SeaBreezeFeature):

    identifier = "raw_usb_bus_access"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfRawUSBBusAccessFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getRawUSBBusAccessFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids

    cdef unsigned char _get_device_endpoint(self, endpoint):
        cdef csb.usbEndpointType ep
        cdef int error_code
        cdef unsigned char out
        ep_map = {
            'primary_out': csb.kEndpointTypePrimaryOut, # slow speed
            'primary_in': csb.kEndpointTypePrimaryIn,  # slow speed
            'secondary_out': csb.kEndpointTypeSecondaryOut, # could be high speed
            'secondary_in': csb.kEndpointTypeSecondaryIn,  # could be high speed
            'secondary_in2': csb.kEndpointTypeSecondaryIn2  # generally high speed
        }
        if endpoint not in ep_map.keys():
            raise ValueError("endpoint not in %s" % str(ep_map.keys()))
        ep = ep_map[endpoint]
        out = self.sbapi.getDeviceEndpoint(self.device_id, &error_code, ep)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return out

    def raw_usb_read(self, endpoint, buffer_length=1024):
        cdef unsigned char* buffer = NULL
        cdef unsigned int buflen = int(buffer_length)
        cdef int error_code
        cdef int bytes_written
        cdef unsigned char ep = self._get_device_endpoint(endpoint)

        try:
            buffer = <unsigned char*> PyMem_Malloc(buffer_length * sizeof(unsigned char))
            bytes_written = self.sbapi.rawUSBBusAccessRead(self.device_id, self.feature_id, &error_code,
                                                           &buffer[0], buflen, ep)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            data = buffer[:bytes_written]
        finally:
            if buffer is not NULL:
                PyMem_Free(buffer)
        return data

    def raw_usb_write(self, data, endpoint):
        cdef unsigned char* buffer
        cdef unsigned int buffer_length
        cdef int error_code
        cdef int bytes_written
        cdef unsigned char ep = self._get_device_endpoint(endpoint)
        bdata = bytes(data)
        buffer = bdata
        buffer_length = len(data)
        bytes_written = self.sbapi.rawUSBBusAccessWrite(self.device_id, self.feature_id, &error_code,
                                                        &buffer[0], buffer_length, ep)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bytes_written


cdef class SeaBreezeSpectrometerFeature(SeaBreezeFeature):

    identifier = "spectrometer"
    required = False

    cdef public int _cached_spectrum_length

    def __cinit__(self, SeaBreezeDevice device, int feature_id):
        self._cached_spectrum_length = -1

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfSpectrometerFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getSpectrometerFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids

    def set_trigger_mode(self, mode):
        """sets the trigger mode for the spectrometer

        Parameters
        ----------
        mode : int
            trigger mode for spectrometer. Note that requesting an
            unsupported mode will result in an error.

        Returns
        -------
        None
        """
        cdef int error_code
        cdef int cmode
        cmode = int(mode)
        cdef unsigned long device_id, feature_id
        with nogil:
            self.sbapi.spectrometerSetTriggerMode(self.device_id, self.feature_id, &error_code, cmode)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def set_integration_time_micros(self, unsigned long integration_time_micros):
        """sets the integration time for the specified device

        Parameters
        ----------
        integration_time_micros : int
            the integration time in micro seconds

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned long cinttime
        cinttime = int(integration_time_micros)
        with nogil:
            self.sbapi.spectrometerSetIntegrationTimeMicros(self.device_id, self.feature_id, &error_code, cinttime)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def get_integration_time_micros_limits(self):
        """returns the smallest and largest valid integration time setting, in microseconds

        Returns
        -------
        micros_low: int
            smallest supported integration time
        micros_high: int
            largest supported integration time
        """
        cdef int error_code
        cdef unsigned long int_low, int_high
        int_low = self.sbapi.spectrometerGetMinimumIntegrationTimeMicros(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        int_high = self.sbapi.spectrometerGetMaximumIntegrationTimeMicros(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(int_low), int(int_high)

    def get_maximum_intensity(self):
        """returns the maximum pixel intensity for the spectrometer

        Returns
        -------
        max_intensity: float
        """
        cdef int error_code
        cdef double max_intensity
        max_intensity = self.sbapi.spectrometerGetMaximumIntensity(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return float(max_intensity)

    def get_electric_dark_pixel_indices(self):
        """returns the electric dark pixel indices for the spectrometer

        This returns a list of indices of the pixels that are electrically active
        but optically masked (a.k.a. electric dark pixels). Note that not all
        detectors have optically masked pixels; in that case, an empty list is returned

        Returns
        -------
        dark_pixel_idxs: list of int
        """
        cdef int error_code
        cdef int dp_count, written
        dp_count = self.sbapi.spectrometerGetElectricDarkPixelCount(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if dp_count == 0:
            return []
        cindices = <int*> PyMem_Malloc(dp_count * sizeof(int))
        written = self.sbapi.spectrometerGetElectricDarkPixelIndices(self.device_id, self.feature_id, &error_code,
                                                                     &cindices[0], dp_count)
        assert int(written) == int(dp_count)
        indices = [int(cindices[i]) for i in range(dp_count)]
        PyMem_Free(cindices)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return indices

    @property
    def _spectrum_length(self):
        """cached spectrum length

        Returns
        -------
        spectrum_length: int
        """
        cdef int error_code
        cdef int spec_length
        if self._cached_spectrum_length < 0:
            spec_length = self.sbapi.spectrometerGetFormattedSpectrumLength(self.device_id, self.feature_id, &error_code)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            self._cached_spectrum_length = int(spec_length)
        return self._cached_spectrum_length

    @cython.boundscheck(False)
    def get_wavelengths(self):
        """computes the wavelengths for the spectrometer

        Returns
        -------
        wavelengths: `np.ndarray`
        """
        cdef int error_code
        cdef int bytes_written
        cdef double[::1] out
        cdef int out_length

        wavelengths = np.zeros((self._spectrum_length, ), dtype=np.double)
        out = wavelengths
        out_length = wavelengths.size
        with nogil:
            bytes_written = self.sbapi.spectrometerGetWavelengths(self.device_id, self.feature_id, &error_code,
                                                                  &out[0], out_length)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return wavelengths

    @cython.boundscheck(False)
    def get_intensities(self):
        """acquires a spectrum and returns the measured intensities

        In this mode, auto-nulling should be automatically performed
        for devices that support it.

        Returns
        -------
        intensities: `np.ndarray`
        """
        cdef int error_code
        cdef int bytes_written
        cdef double[::1] out
        cdef int out_length

        wavelengths = np.zeros((self._spectrum_length, ), dtype=np.double)
        out = wavelengths
        out_length = wavelengths.size
        with nogil:
            bytes_written = self.sbapi.spectrometerGetFormattedSpectrum(self.device_id, self.feature_id, &error_code,
                                                                        &out[0], out_length)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return wavelengths

    def _get_spectrum_raw(self):
        # int spectrometerGetUnformattedSpectrumLength(long deviceID, long spectrometerFeatureID, int *errorCode)
        # int spectrometerGetUnformattedSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode,
        #                                        unsigned char *buffer, int bufferLength)
        raise NotImplementedError("unformatted spectrum")

    def _get_fast_buffer_spectrum(self):
        # int spectrometerGetFastBufferSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode,
        #                                       unsigned char *dataBuffer, int dataMaxLength,
        #                                       unsigned int numberOfSampleToRetrieve) # currently 15 max
        raise NotImplementedError("fast buffer spectrum")


cdef class SeaBreezePixelBinningFeature(SeaBreezeFeature):

    identifier = "pixel_binning"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfPixelBinningFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getPixelBinningFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeThermoElectricFeature(SeaBreezeFeature):

    identifier = "thermo_electric"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfThermoElectricFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getThermoElectricFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids

    def read_temperature_degrees_C(self):
        """reads the actual temperature of the TEC in degrees Celsius

        Returns
        -------
        temperature: float
            tec temperature in degrees Celcius
        """
        cdef int error_code
        cdef double temperature
        temperature = self.sbapi.tecReadTemperatureDegreesC(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return float(temperature)

    def set_temperature_setpoint_degrees_C(self, temperature):
        """sets the target (setpoint) TEC temperature

        Returns
        -------
        None
        """
        cdef int error_code
        cdef double temperature_degrees_celsius
        temperature_degrees_celsius = float(temperature)
        self.sbapi.tecSetTemperatureSetpointDegreesC(self.device_id, self.feature_id, &error_code,
                                                     temperature_degrees_celsius)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def enable_tec(self, state):
        """enables the TEC feature on the device

        Parameters
        ----------
        state : bool
            on or off

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char enable = 1 if bool(state) else 0
        self.sbapi.tecSetEnable(self.device_id, self.feature_id, &error_code, enable)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeIrradCalFeature(SeaBreezeFeature):

    identifier = "irrad_cal"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIrradCalFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getIrradCalFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids

    def has_collection_area(self):
        """checks for an irradiance collection area

        Returns
        -------
        has_area: bool
        """
        cdef int error_code
        cdef int has_ca
        has_ca = self.sbapi.irradCalibrationHasCollectionArea(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(has_ca)

    def read_collection_area(self):
        """reads an irradiance collection area from the spectrometer

        Returns
        -------
        area: float
        """
        cdef int error_code
        cdef float area
        area = self.sbapi.irradCalibrationReadCollectionArea(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return float(area)

    def write_collection_area(self, area):
        """writes an irradiance collection area to the spectrometer

        Returns
        -------
        None
        """
        cdef int error_code
        cdef float c_area = float(area)
        self.sbapi.irradCalibrationWriteCollectionArea(self.device_id, self.feature_id, &error_code, c_area)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def read_calibration(self):
        """

        Returns
        -------
        irrad_calibration: `np.ndarray`
        """
        cdef int error_code
        cdef int bytes_written
        cdef float[::1] out
        cdef int out_length
        irrad_calibration = np.zeros((4096,), dtype=np.float32)  # FIXME: not 4096 but Formatted Spectrum Length
        out = irrad_calibration
        out_length = irrad_calibration.size
        bytes_written = self.sbapi.irradCalibrationRead(self.device_id, self.feature_id, &error_code,
                                                        &out[0], out_length)
        if error_code != 0:
             raise SeaBreezeError(error_code=error_code)
        return irrad_calibration[:bytes_written]

    def write_calibration(self, calibration_array):
        """

        Parameters
        ----------
        calibration_array : `np.ndarray`

        Returns
        -------
        None
        """
        cdef int error_code
        cdef int bytes_written
        cdef float[::1] out
        cdef int out_length
        arr = np.asarray(calibration_array, dtype=np.float)
        if not arr.ndim == 1:
            raise ValueError("calibration_array needs to be 1D")
        out = arr
        out_length = arr.size
        bytes_written = self.sbapi.irradCalibrationWrite(self.device_id, self.feature_id, &error_code,
                                                         &out[0], out_length)
        if error_code != 0:
             raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeEthernetConfigurationFeature(SeaBreezeFeature):

    identifier = "ethernet_configuration"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfEthernetConfigurationFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getEthernetConfigurationFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeMulticastFeature(SeaBreezeFeature):

    identifier = "multicast"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfMulticastFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getMulticastFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeIPv4Feature(SeaBreezeFeature):

    identifier = "ipv4"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIPv4Features(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getIPv4Features(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeDHCPServerFeature(SeaBreezeFeature):

    identifier = "dhcp_server"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfDHCPServerFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getDHCPServerFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeNetworkConfigurationFeature(SeaBreezeFeature):

    identifier = "network_configuration"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfNetworkConfigurationFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getNetworkConfigurationFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeWifiConfigurationFeature(SeaBreezeFeature):

    identifier = "wifi_configuration"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfWifiConfigurationFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getWifiConfigurationFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeGPIOFeature(SeaBreezeFeature):

    identifier = "gpio"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfGPIOFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getGPIOFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeEEPROMFeature(SeaBreezeFeature):

    identifier = "eeprom"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfEEPROMFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getEEPROMFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids

    def eeprom_read_slot(self, int slot_number, bool_t strip_zero_bytes=False):
        """reads a string out of the device's EEPROM slot and returns the result

        Parameters
        ----------
        slot_number : int
            The number of the slot to read out. Possible values are 0 through 17.
        strip_zero_bytes : bool
            strip \x00 bytes from output (default False)

        Returns
        -------
        eeprom_data: str
            the data stored in the eeprom slot
        """
        cdef int error_code
        cdef unsigned char cbuf[_MAXBUFLEN]
        cdef int bytes_written
        try:
            bytes_written = self.sbapi.eepromReadSlot(self.device_id, self.feature_id, &error_code,
                                                      slot_number, cbuf, _MAXBUFLEN)
        except ValueError:
            raise SeaBreezeError("EEProm slot out of bounds.")
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if strip_zero_bytes:
            return cbuf[:bytes_written].strip('\x00')
        return cbuf[:bytes_written]


cdef class SeaBreezeLightSourceFeature(SeaBreezeFeature):

    identifier = "light_source"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfLightSourceFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getLightSourceFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeStrobeLampFeature(SeaBreezeFeature):

    identifier = "strobe_lamp"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfLampFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getLampFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
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



cdef class SeaBreezeContinuousStrobeFeature(SeaBreezeFeature):

    identifier = "continuous_strobe"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfContinuousStrobeFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getContinuousStrobeFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeShutterFeature(SeaBreezeFeature):

    identifier = "shutter"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfShutterFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getShutterFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids

    def set_shutter_open(self, state):
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


cdef class SeaBreezeNonlinearityCoeffsFeature(SeaBreezeFeature):

    identifier = "nonlinearity_coeffs"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfNonlinearityCoeffsFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getNonlinearityCoeffsFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
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


cdef class SeaBreezeTemperatureFeature(SeaBreezeFeature):

    identifier = "temperature"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfTemperatureFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getTemperatureFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeIntrospectionFeature(SeaBreezeFeature):

    identifier = "introspection"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIntrospectionFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getIntrospectionFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeSpectrumProcessingFeature(SeaBreezeFeature):

    identifier = "spectrum_processing"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfSpectrumProcessingFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getSpectrumProcessingFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeRevisionFeature(SeaBreezeFeature):

    identifier = "revision"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfRevisionFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getRevisionFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeOpticalBenchFeature(SeaBreezeFeature):

    identifier = "optical_bench"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfOpticalBenchFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getOpticalBenchFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeStrayLightCoeffsFeature(SeaBreezeFeature):

    identifier = "stray_light_coeffs"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfStrayLightCoeffsFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getStrayLightCoeffsFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
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


cdef class SeaBreezeDataBufferFeature(SeaBreezeFeature):

    identifier = "data_buffer"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfDataBufferFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getDataBufferFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeFastBufferFeature(SeaBreezeFeature):

    identifier = "fast_buffer"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfFastBufferFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getFastBufferFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeAcquisitionDelayFeature(SeaBreezeFeature):

    identifier = "acquisition_delay"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfAcquisitionDelayFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getAcquisitionDelayFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids


cdef class SeaBreezeI2CMasterFeature(SeaBreezeFeature):

    identifier = "i2c_master"
    required = False

    @classmethod
    def get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfI2CMasterFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            sbapi.getI2CMasterFeatures(device.handle, &error_code, feature_ids, num_features)
            cls._raise_if_error(error_code, num_features)
            py_feature_ids = [feature_ids[i] for i in range(num_features)]
            PyMem_Free(feature_ids)
        return py_feature_ids




'''

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
