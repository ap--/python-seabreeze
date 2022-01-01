"""This is the cython wrapper for the seabreeze library

Author: Andreas Poehlmann

"""
cimport c_seabreeze as csb
cimport cython
from cpython.mem cimport PyMem_Free
from cpython.mem cimport PyMem_Malloc
from libcpp cimport bool as bool_t

import weakref

import numpy as np


# from libseabreeze api/SeaBreezeConstants.h
class _ErrorCode(object):
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


# DO NOT DIRECTLY IMPORT EXCEPTIONS FROM HERE!
# ALWAYS IMPORT FROM `seabreeze.spectrometers`
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
                message = self._error_msgs[_ErrorCode.INVALID_ERROR]
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


class SeaBreezeNotSupported(SeaBreezeError):
    # ... i know you shouldn't always subclass Exceptions ...
    pass


cdef class SeaBreezeAPI(object):
    """SeaBreeze API interface"""

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
        _seabreeze_device_instance_registry.clear()
        if self.sbapi:
            csb.SeaBreezeAPI.shutdown()
            self.sbapi = NULL

    def add_rs232_device_location(self, device_type, bus_path, baudrate):
        """add RS232 device location

        Parameters
        ----------
        device_type : str
        bus_path : str
            This will be a platform-specific location. Under Windows, this may
            be COM1, COM2, etc.  Under Linux, this might be /dev/ttyS0, /dev/ttyS1,
            etc.
        baudrate : int

        Returns
        -------
        success : bool
        """
        cdef int output
        cdef bytes c_devtype
        cdef bytes c_buspath
        cdef unsigned int c_baudrate
        c_devtype = bytes(device_type)
        c_buspath = bytes(bus_path)
        c_baudrate = int(baudrate)
        cdef char* p_devtype = c_devtype
        cdef char* p_buspath = c_buspath
        if not self.sbapi:
            raise RuntimeError("SeaBreezeAPI not initialized")
        output = self.sbapi.addRS232DeviceLocation(p_devtype, p_buspath, c_baudrate)
        return not bool(output)

    def add_ipv4_device_location(self, device_type, ip_address, port):
        """add ipv4 device location

        Parameters
        ----------
        device_type : str
        ip_address : str
            format XXX.XXX.XXX.XXX
        port : int

        Returns
        -------
        success : bool
        """
        cdef int output
        cdef bytes c_devtype
        cdef bytes c_buspath
        cdef int c_port
        c_devtype = bytes(device_type)
        c_ipaddr = bytes(ip_address)
        c_port = int(port)
        cdef char* p_devtype = c_devtype
        cdef char* p_ipaddr = c_ipaddr
        if not self.sbapi:
            raise RuntimeError("SeaBreezeAPI not initialized")
        output = self.sbapi.addTCPIPv4DeviceLocation(p_devtype, p_ipaddr, c_port)
        return not bool(output)

    def _list_device_ids(self):
        """list device ids for all available spectrometers

        Note: this includes spectrometers that are currently opened in other
        processes on the machine.

        Returns
        -------
        device_ids : list of ints
            unique device_ids for each available spectrometer
        """
        cdef int num_devices
        cdef long* c_device_ids
        cdef int found_devices
        if not self.sbapi:
            raise RuntimeError("SeaBreezeAPI not initialized")
        self.sbapi.probeDevices()
        num_devices = self.sbapi.getNumberOfDeviceIDs()
        c_device_ids = <long*> PyMem_Malloc(num_devices * sizeof(long))
        if not c_device_ids:
            raise MemoryError("could not allocate memory for device_ids")
        try:
            found_devices = self.sbapi.getDeviceIDs(c_device_ids, num_devices)
            device_ids = []
            for i in range(found_devices):
                device_ids.append(int(c_device_ids[i]))
            return device_ids
        finally:
            PyMem_Free(c_device_ids)

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
            dev = _seabreeze_device_factory(handle)
            if dev.is_open:
                was_open_before = True
            else:
                was_open_before = False
                try:
                    dev.open()
                except SeaBreezeError as err:
                    if err.error_code == _ErrorCode.NO_DEVICE:
                        # device used by another thread?
                        continue
            model = dev.model
            serial = dev.serial_number
            if not was_open_before:
                dev.close()
            devices.append(dev)
        return devices

    def supported_models(self):
        """returns SeaBreezeDevices supported by the backend

        models supported by the backend

        Returns
        -------
        devices: list of str
            list of model names that are supported by this backend
        """
        cdef int num_models
        cdef int error_code
        cdef char c_buffer[_MAXBUFLEN]
        cdef int bytes_written
        if not self.sbapi:
            raise RuntimeError("SeaBreezeAPI not initialized")
        num_devices = self.sbapi.getNumberOfSupportedModels()

        output = []
        for i in range(num_devices):
            bytes_written = self.sbapi.getSupportedModelName(i, &error_code, c_buffer, _MAXBUFLEN)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)

            serial = c_buffer[:bytes_written]
            output.append(serial.decode("utf-8").rstrip('\x00'))
        return output


cdef class SeaBreezeDevice(object):
    """SeaBreezeDevice class for handling all spectrometers

    This is the default cseabreeze class interface for all supported spectrometers.
    Users don't instantiate it directly, but retrieve instances via
    :func:`seabreeze.cseabreeze.SeaBreezeAPI.list_devices`

    """
    cdef readonly long handle
    cdef readonly str _model, _serial_number
    cdef csb.SeaBreezeAPI *sbapi
    # enable weak references
    cdef object __weakref__

    def __cinit__(self, handle):
        self.sbapi = csb.SeaBreezeAPI.getInstance()

    def __init__(self, handle=None):
        if handle is None:
            raise SeaBreezeError("Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`.")
        self.handle = handle
        try:
            self._get_info()
        except SeaBreezeError:
            if not self._model:
                # TODO: warn, getting the model string should always succeed...
                self._model = "?"
            if not self._serial_number:
                self._serial_number = "?"

    def __dealloc__(self):
        cdef int error_code
        # always returns 1
        # under normal circumstances this is fine because it'll return
        # the pointer to the same instance.
        # In a pytest scenario where SeaBreezeAPI.shutdown is called to
        # allow switching to other backends, this will prevent a segfault
        # (and create a little bit of overhead because it recreates the
        # DeviceFactory instance...)
        self.sbapi = csb.SeaBreezeAPI.getInstance()
        self.sbapi.closeDevice(self.handle, &error_code)

    cdef _get_info(self):
        """populate model and serial_number attributes (internal)"""
        model = self.get_model()
        try:
            self._model = model
        except TypeError:
            self._model = model.encode("utf-8")
        serial_number = self.get_serial_number()
        try:
            self._serial_number = serial_number
        except TypeError:
            self._serial_number = serial_number.encode("utf-8")

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
    def model(self):
        return "{}.".format(self._model)[:-1]

    @property
    def serial_number(self):
        return "{}.".format(self._serial_number)[:-1]

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
        except SeaBreezeNumFeaturesError:
            return False
        except SeaBreezeError as err:
            if err.error_code == _ErrorCode.TRANSFER_ERROR:
                return False
            raise err
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
        cdef char c_buffer[_MAXBUFLEN]
        cdef int bytes_written
        bytes_written = self.sbapi.getSerialNumber(self.handle, feature_id, &error_code, c_buffer, max_length)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        serial = c_buffer[:bytes_written]
        return serial.decode("utf-8").rstrip('\x00')

    def get_model(self):
        """return the model string of the spectrometer

        Returns
        -------
        model: str
        """
        cdef int error_code
        cdef char c_buffer[_MAXBUFLEN]
        cdef int bytes_written
        bytes_written = self.sbapi.getDeviceType(self.handle, &error_code, c_buffer, _MAXBUFLEN)
        model = c_buffer[:bytes_written]
        if model == "NONE":
            raise SeaBreezeError(error_code=error_code)
        return model.decode("utf-8")

    @property
    def features(self):
        """return a dictionary of all supported features

        this returns a dictionary with all supported Features of the spectrometer

        Returns
        -------
        features : `dict` [`str`, `seabreeze.cseabreeze.SeaBreezeFeature`]
        """
        # TODO: make this a cached property
        features = {}
        # noinspection PyProtectedMember
        feature_registry = SeaBreezeFeature.get_feature_class_registry()
        for identifier, feature_class in feature_registry.items():
            feature_ids = feature_class._get_feature_ids_from_device(self)
            features[identifier] = [feature_class(self, feature_id) for feature_id in feature_ids]
        return features

    @property
    def f(self):
        """convenience assess to features via attributes

        this allows you to access a feature like this::

            # via .features
            device.features['spectrometer'][0].get_intensities()
            # via .f
            device.f.spectrometer.get_intensities()

        """
        class FeatureAccessHandler(object):
            def __init__(self, feature_dict):
                for identifier, features in feature_dict.items():
                    setattr(self, identifier, features[0] if features else None)  # TODO: raise FeatureNotAvailable?
        return FeatureAccessHandler(self.features)


# create only one SeaBreezeDevice instance per handle
_seabreeze_device_instance_registry = weakref.WeakValueDictionary()
def _seabreeze_device_factory(handle):
    """return existing instances instead of creating temporary ones"""
    try:
        return _seabreeze_device_instance_registry[handle]
    except KeyError:
        dev = _seabreeze_device_instance_registry[handle] = SeaBreezeDevice(handle)
        return dev


cdef class SeaBreezeFeature(object):
    """BaseClass for SeaBreezeFeatures

    defines the minimum class interface for all features
    """
    cdef SeaBreezeDevice device
    cdef long device_id
    cdef readonly long feature_id
    cdef csb.SeaBreezeAPI *sbapi

    identifier = "base_feature"

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
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):
        return []

    @classmethod
    def _raise_if_error(cls, error_code, num_features):
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if num_features == 0:
            # TODO: raise SeaBreezeNumFeaturesError(cls.identifier, num_features) ?
            pass


cdef class SeaBreezeRawUSBBusAccessFeature(SeaBreezeFeature):

    identifier = "raw_usb_bus_access"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfRawUSBBusAccessFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getRawUSBBusAccessFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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
        """read raw data from usb

        Parameters
        ----------
        endpoint : str
            one of {'primary_out', 'primary_in', 'secondary_out', 'secondary_in', 'secondary_in2'}
        buffer_length : int, default=1024
            length of the allocated outputbuffer

        Returns
        -------
        data: str
            raw readout from usb
        """
        cdef unsigned char* c_buffer = NULL
        cdef unsigned int buflen = int(buffer_length)
        cdef int error_code
        cdef int bytes_written
        cdef unsigned char ep = self._get_device_endpoint(endpoint)
        c_buffer = <unsigned char*> PyMem_Malloc(buffer_length * sizeof(unsigned char))
        if not c_buffer:
            raise MemoryError("could not allocate memory for buffer")
        try:
            bytes_written = self.sbapi.rawUSBBusAccessRead(self.device_id, self.feature_id, &error_code,
                                                           &c_buffer[0], buflen, ep)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            data = c_buffer[:bytes_written]
        finally:
            PyMem_Free(c_buffer)
        return data

    def raw_usb_write(self, data, endpoint):
        """send raw data to usb

        Parameters
        ----------
        data : str
            raw data that should be transfered to spectrometer
        endpoint : str
            one of {'primary_out', 'primary_in', 'secondary_out', 'secondary_in', 'secondary_in2'}

        Returns
        -------
        bytes_written : int
        """
        cdef unsigned char* c_buffer
        cdef unsigned int c_buffer_length
        cdef int error_code
        cdef int bytes_written
        cdef unsigned char ep = self._get_device_endpoint(endpoint)
        bdata = bytes(data)
        c_buffer = bdata
        c_buffer_length = len(data)
        bytes_written = self.sbapi.rawUSBBusAccessWrite(self.device_id, self.feature_id, &error_code,
                                                        &c_buffer[0], c_buffer_length, ep)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(bytes_written)


cdef class SeaBreezeSpectrometerFeature(SeaBreezeFeature):

    identifier = "spectrometer"

    cdef readonly int _cached_spectrum_length

    def __cinit__(self, SeaBreezeDevice device, int feature_id):
        self._cached_spectrum_length = -1

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfSpectrometerFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getSpectrometerFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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
        if not cindices:
            raise MemoryError("could not allocate memory for cindices")
        try:
            written = self.sbapi.spectrometerGetElectricDarkPixelIndices(self.device_id, self.feature_id, &error_code,
                                                                         &cindices[0], dp_count)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            assert int(written) == int(dp_count)
            indices = [int(cindices[i]) for i in range(dp_count)]
        finally:
            PyMem_Free(cindices)
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

        intensities = np.zeros((self._spectrum_length, ), dtype=np.double)
        out = intensities
        out_length = intensities.size
        with nogil:
            bytes_written = self.sbapi.spectrometerGetFormattedSpectrum(self.device_id, self.feature_id, &error_code,
                                                                        &out[0], out_length)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert bytes_written == self._spectrum_length
        return intensities

    def _get_spectrum_raw(self):
        # int spectrometerGetUnformattedSpectrumLength(long deviceID, long spectrometerFeatureID, int *errorCode)
        # int spectrometerGetUnformattedSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode,
        #                                        unsigned char *buffer, int bufferLength)
        raise NotImplementedError("unformatted spectrum")

    def get_fast_buffer_spectrum(self):
        # TODO: requires wrapping of OBP command GetRawSpectrumWithMetadata
        #       which returns N raw spectra each with a 64 byte metadata prefix
        # int spectrometerGetFastBufferSpectrum(long deviceID, long spectrometerFeatureID, int *errorCode,
        #                                       unsigned char *dataBuffer, int dataMaxLength,
        #                                       unsigned int numberOfSampleToRetrieve) # currently 15 max
        raise NotImplementedError("Not yet supported via cseabreeze. Requires changes to libseabreeze.")


cdef class SeaBreezePixelBinningFeature(SeaBreezeFeature):

    identifier = "pixel_binning"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfPixelBinningFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getPixelBinningFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    def set_binning_factor(self, factor):
        """sets the pixel binning factor on the device

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char binning
        binning = int(factor)
        self.sbapi.binningSetPixelBinningFactor(self.device_id, self.feature_id, &error_code, binning)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def get_binning_factor(self):
        """gets the pixel binning factor on the device

        Returns
        -------
        binning_factor: int
        """
        cdef int error_code
        cdef unsigned char binning
        binning = self.sbapi.binningGetPixelBinningFactor(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(binning)

    def get_max_binning_factor(self):
        """gets the max pixel binning factor on the device

        Returns
        -------
        binning_factor: int
        """
        cdef int error_code
        cdef unsigned char binning
        binning = self.sbapi.binningGetMaxPixelBinningFactor(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(binning)

    binning_factor = property(get_binning_factor, set_binning_factor)
    max_binning_factor = property(get_max_binning_factor)

    def set_default_binning_factor(self, factor):
        """sets the pixel binning factor on the device

        Parameters
        ----------
        factor : int or None
            the desired default pixel binning factor. If None, resets the default.

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char binning
        if factor is None:
            self.sbapi.binningSetDefaultPixelBinningFactor(self.device_id, self.feature_id, &error_code)
        else:
            binning = int(factor)
            self.sbapi.binningSetDefaultPixelBinningFactor(self.device_id, self.feature_id, &error_code, binning)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def get_default_binning_factor(self):
        """gets the default pixel binning factor on the device

        Returns
        -------
        binning_factor: int
        """
        cdef int error_code
        cdef unsigned char binning
        binning = self.sbapi.binningGetDefaultPixelBinningFactor(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(binning)

    default_binning_factor = property(get_default_binning_factor, set_default_binning_factor)


cdef class SeaBreezeThermoElectricFeature(SeaBreezeFeature):

    identifier = "thermo_electric"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfThermoElectricFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getThermoElectricFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    def read_temperature_degrees_celsius(self):
        """reads the actual temperature of the TEC in degrees Celsius

        Returns
        -------
        temperature: float
            tec temperature in degrees Celsius
        """
        cdef int error_code
        cdef double temperature
        temperature = self.sbapi.tecReadTemperatureDegreesC(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return float(temperature)

    def set_temperature_setpoint_degrees_celsius(self, temperature):
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

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIrradCalFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getIrradCalFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfEthernetConfigurationFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getEthernetConfigurationFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # void ethernetConfiguration_Get_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char (*macAddress)[6])
    def get_mac_address(self, interface_index):
        """get mac address of interface

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        mac_address : str
        """
        cdef int error_code
        cdef unsigned char (*mac_address)[6]
        cdef unsigned char *view_mac_address
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        mac_address = <unsigned char(*)[6]> PyMem_Malloc(sizeof(unsigned char[6]))
        if not mac_address:
            raise MemoryError("can't allocate memory for mac_address")
        try:
            self.sbapi.ethernetConfiguration_Get_MAC_Address(self.device_id, self.feature_id, &error_code,
                                                             c_interface_index, mac_address)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            view_mac_address = <unsigned char*> mac_address
            mbytes = []
            for i in range(6):
                mbytes.append("%0.2X" % int(view_mac_address[i]))
            return ":".join(mbytes)
        finally:
            PyMem_Free(mac_address)

    # void ethernetConfiguration_Set_MAC_Address(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, const unsigned char macAddress[6])
    def set_mac_address(self, interface_index, mac_address):
        """set mac address of interface

        Parameters
        ----------
        interface_index : int
        mac_address : str
            mac_address XX:XX:XX:XX:XX:XX

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        # convert mac address
        mbytes = map(lambda x: chr(int(x, 16)), mac_address.split(':'))
        assert len(mbytes) == 6
        cbytes = bytes("".join(mbytes))[:6]
        cdef unsigned char macAddress[6]
        macAddress = <const unsigned char*> PyMem_Malloc(6 * sizeof(unsigned char))
        try:
            for i in range(6):
                macAddress[i] = cbytes[i]
            self.sbapi.ethernetConfiguration_Set_MAC_Address(self.device_id, self.feature_id, &error_code, c_interface_index, macAddress)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
        finally:
            PyMem_Free(macAddress)

    # unsigned char ethernetConfiguration_Get_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_gbe_enable_status(self, interface_index):
        """reads the GbE enable status from the device's internal memory

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        gbe_enabled : bool
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.ethernetConfiguration_Get_GbE_Enable_Status(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(output)

    # void ethernetConfiguration_Set_GbE_Enable_Status(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char enableState)
    def set_gbe_enable_status(self, interface_index, enable_state):
        """writes the GbE enable status to the spectrometer's internal memory

        Parameters
        ----------
        interface_index : int
        enable_state : bool

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char enableState
        c_interface_index = int(interface_index)
        enableState = 1 if enable_state else 0
        self.sbapi.ethernetConfiguration_Set_GbE_Enable_Status(self.device_id, self.feature_id, &error_code, c_interface_index, enableState)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeMulticastFeature(SeaBreezeFeature):

    identifier = "multicast"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfMulticastFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getMulticastFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # Both multicast address methods are commented out in libseabreeze
    #
    # void getMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char(&macAddress)[6])
    # def get_multicast_group_address(self, c_interface_index,  unsigned char(&macAddress)[6]):
    #     output = self.sbapi.getMulticastGroupAddress(self.device_id, self.feature_id, &error_code, c_interface_index, char(&macAddress))
    #
    # void setMulticastGroupAddress(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, const unsigned char macAddress[6])
    # def set_multicast_group_address(self, c_interface_index,  const unsigned char macAddress[6]):
    #     output = self.sbapi.setMulticastGroupAddress(self.device_id, self.feature_id, &error_code, c_interface_index, macAddress)

    # unsigned char getMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_multicast_enable_state(self, interface_index):
        """get multicast enable state

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        enabled : bool
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.getMulticastEnableState(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(output)

    # void setMulticastEnableState(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char enableState)
    def set_multicast_enable_state(self, interface_index, enable_state):
        """set multicast enable state

        Parameters
        ----------
        interface_index : int
        enable_state : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char enableState
        c_interface_index = int(interface_index)
        enableState = 1 if bool(enable_state) else 0
        self.sbapi.setMulticastEnableState(self.device_id, self.feature_id, &error_code, c_interface_index, enableState)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeIPv4Feature(SeaBreezeFeature):

    identifier = "ipv4"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIPv4Features(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getIPv4Features(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char get_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_dhcp_enable_state(self, interface_index):
        """get dhcp enable state

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        enabled : bool
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.get_IPv4_DHCP_Enable_State(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(output)

    # void set_IPv4_DHCP_Enable_State(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char isEnabled)
    def set_dhcp_enable_state(self, interface_index, is_enabled):
        """set dhcp enable state

        Parameters
        ----------
        interface_index : int
        is_enabled : bool

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char isEnabled
        c_interface_index = int(interface_index)
        isEnabled = 1 if bool(is_enabled) else 0
        self.sbapi.set_IPv4_DHCP_Enable_State(self.device_id, self.feature_id, &error_code, c_interface_index, isEnabled)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned char get_Number_Of_IPv4_Addresses(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_number_of_ipv4_addresses(self, interface_index):
        """get number of ipv4 addresses for interface

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        num_addresses : int
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.get_Number_Of_IPv4_Addresses(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void get_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index,
    #                       unsigned char addressIndex, unsigned char(*IPv4_Address)[4], unsigned char *netMask)
    def get_ipv4_address(self, interface_index, address_index):
        """get ipv4 address

        Parameters
        ----------
        interface_index : int
        address_index : int
        char(IPv_Address)

        Returns
        -------
        ipv4_address : str
            in format XXX.XXX.XXX.XXX/MM
        """
        cdef int error_code

        cdef unsigned char c_interface_index
        cdef unsigned char addressIndex
        cdef unsigned char(*ipv4_address)[4]
        cdef unsigned char* netMask
        cdef unsigned char* view_addr
        c_interface_index = int(interface_index)
        addressIndex = int(address_index)
        ipv4_address = <unsigned char(*)[4]> PyMem_Malloc(sizeof(unsigned char[4]))
        if not ipv4_address:
            raise MemoryError("can't allocate memory for ipv4 address")
        net_mask = <unsigned char*> PyMem_Malloc(sizeof(unsigned char))
        if not net_mask:
            raise MemoryError("can't allocate memory for ipv4 netmask")
        try:
            self.sbapi.get_IPv4_Address(self.device_id, self.feature_id, &error_code, c_interface_index, addressIndex,
                                        ipv4_address, net_mask)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            view_addr = <unsigned char*> ipv4_address
            ip = []
            for i in range(4):
                ip.append(int(view_addr[i]))
            mask = int(*net_mask)
            return "{ip[0]:d}.{ip[1]:d}.{ip[2]:d}.{ip[3]:d}/{netmask:d}".format(ip=ip, netmask=mask)
        finally:
            PyMem_Free(ipv4_address)
            PyMem_Free(net_mask)

    # void get_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char(*defaultGatewayAddress)[4])
    def get_default_gateway(self, interface_index):
        """get gateway address

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        gateway_address : str
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char(*address)[4]
        cdef unsigned char* view_addr
        c_interface_index = int(interface_index)
        address = <unsigned char(*)[4]> PyMem_Malloc(sizeof(unsigned char[4]))
        if not address:
            raise MemoryError("can't allocate memory for ipv4 address")
        try:
            self.sbapi.get_IPv4_Default_Gateway(self.device_id, self.feature_id, &error_code, c_interface_index, address)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            view_addr = <unsigned char*> address
            ip = []
            for i in range(4):
                ip.append(int(view_addr[i]))
            return "{ip[0]:d}.{ip[1]:d}.{ip[2]:d}.{ip[3]:d}".format(ip=ip)
        finally:
            PyMem_Free(address)

    # void set_IPv4_Default_Gateway(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, const unsigned char defaultGatewayAddress[4])
    def set_default_gateway(self, interface_index, default_gateway_address):
        """set default gateway

        Parameters
        ----------
        interface_index : int
        default_gateway_address : str
            format xxx.xxx.xxx.xxx

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char address[4]
        c_interface_index = int(interface_index)
        mbytes = map(int, default_gateway_address.split('.'))
        assert len(mbytes) == 4
        cbytes = bytes("".join(mbytes))[:4]
        address = <const unsigned char*> PyMem_Malloc(4 * sizeof(unsigned char))
        try:
            for i in range(4):
                address[i] = cbytes[i]
            self.sbapi.set_IPv4_Default_Gateway(self.device_id, self.feature_id, &error_code, c_interface_index, address)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
        finally:
            PyMem_Free(address)

    # void add_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, const unsigned char IPv4_Address[4], unsigned char netMask)
    def add_ipv4_address(self, interface_index, ipv4_address):
        """add a ipv4 address

        Parameters
        ----------
        interface_index : int
        ipv4_address : str
            format xxx.xxx.xxx.xxx/nm, default nm=24

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char address[4]
        cdef  unsigned char netMask
        c_interface_index = int(interface_index)

        addr_nm = ipv4_address.split('/')
        if len(addr_nm) == 1:
            nm = 24  # default netmask
        else:
            nm = int(addr_nm[1])
        netMask = nm
        mbytes = map(int, addr_nm[0].split('.'))
        assert len(mbytes) == 4
        cbytes = bytes("".join(mbytes))[:4]
        address = <const unsigned char*> PyMem_Malloc(4 * sizeof(unsigned char))
        try:
            for i in range(4):
                address[i] = cbytes[i]
            self.sbapi.add_IPv4_Address(self.device_id, self.feature_id, &error_code, c_interface_index, address, netMask)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
        finally:
            PyMem_Free(address)

    # void delete_IPv4_Address(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char addressIndex)
    def delete_ipv4_address(self, interface_index, address_index):
        """delete a ipv4 address

        Parameters
        ----------
        interface_index : int
        address_index : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char addressIndex
        c_interface_index = int(interface_index)
        addressIndex = int(address_index)
        self.sbapi.delete_IPv4_Address(self.device_id, self.feature_id, &error_code, c_interface_index, addressIndex)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeDHCPServerFeature(SeaBreezeFeature):

    identifier = "dhcp_server"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfDHCPServerFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getDHCPServerFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # void dhcpServerGetAddress(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char(*serverAddress)[4], unsigned char *netMask)
    def get_address(self, interface_index):
        """get dhcp server address

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        server_address : str
            format xxx.xxx.xxx.xxx/nm
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char(*server_address)[4]
        cdef unsigned char* netMask
        cdef unsigned char* view_addr
        c_interface_index = int(interface_index)
        server_address = <unsigned char(*)[4]> PyMem_Malloc(sizeof(unsigned char[4]))
        if not server_address:
            raise MemoryError("can't allocate memory for ipv4 address")
        netMask = <unsigned char*> PyMem_Malloc(sizeof(unsigned char))
        if not netMask:
            raise MemoryError("can't allocate memory for ipv4 netmask")
        try:
            self.sbapi.dhcpServerGetAddress(self.device_id, self.feature_id, &error_code, c_interface_index,
                                            server_address, netMask)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            view_addr = <unsigned char*> server_address
            ip = []
            for i in range(4):
                ip.append(int(view_addr[i]))
            mask = int(*netMask)
            return "{ip[0]:d}.{ip[1]:d}.{ip[2]:d}.{ip[3]:d}/{netmask:d}".format(ip=ip, netmask=mask)
        finally:
            PyMem_Free(server_address)
            PyMem_Free(netMask)

    # void dhcpServerSetAddress(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, const unsigned char serverAddress[4], unsigned char netMask)
    def set_address(self, interface_index, server_address):
        """set dhcp server address

        Parameters
        ----------
        interface_index : int
        server_address : str
            format xxx.xxx.xxx.xxx/nm, default nm=24

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char address[4]
        cdef unsigned char netMask
        c_interface_index = int(interface_index)

        addr_nm = server_address.split('/')
        if len(addr_nm) == 1:
            nm = 24  # default netmask
        else:
            nm = int(addr_nm[1])
        netMask = nm
        mbytes = map(int, addr_nm[0].split('.'))
        assert len(mbytes) == 4
        cbytes = bytes("".join(mbytes))[:4]
        address = <const unsigned char*> PyMem_Malloc(4 * sizeof(unsigned char))
        try:
            for i in range(4):
                address[i] = cbytes[i]
            self.sbapi.dhcpServerSetAddress(self.device_id, self.feature_id, &error_code, c_interface_index, address, netMask)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
        finally:
            PyMem_Free(address)

    # unsigned char dhcpServerGetEnableState(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_enable_state(self, interface_index):
        """get dhcp server enable state

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        enabled : bool
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.dhcpServerGetEnableState(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(output)

    # void dhcpServerSetEnableState(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char enableState)
    def set_enable_state(self, interface_index, enable_state):
        """set dhcp server enable state

        Parameters
        ----------
        interface_index : int
        enable_state : bool

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char enableState
        c_interface_index = int(interface_index)
        enableState = 1 if bool(enable_state) else 0
        self.sbapi.dhcpServerSetEnableState(self.device_id, self.feature_id, &error_code, c_interface_index, enableState)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeNetworkConfigurationFeature(SeaBreezeFeature):

    identifier = "network_configuration"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfNetworkConfigurationFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getNetworkConfigurationFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char getNumberOfNetworkInterfaces(long deviceID, long featureID, int *errorCode)
    def get_number_of_network_interfaces(self):
        """return the number of network interfaces

        Returns
        -------
        num_interfaces : int
        """
        cdef int error_code
        cdef unsigned char output
        output = self.sbapi.getNumberOfNetworkInterfaces(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned char getNetworkInterfaceConnectionType(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_network_interface_connection_type(self, interface_index):
        """get the network interface connection type

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        network_type : int
            {0: 'loopback', 1: 'wired ethernet', 2: 'wifi', 3: 'cdc ethernet (usb)'}
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.getNetworkInterfaceConnectionType(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned char getNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_network_interface_enable_state(self, interface_index):
        """get network interface enable state

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        enabled : bool
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.getNetworkInterfaceEnableState(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(output)

    # void setNetworkInterfaceEnableState(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char enableState)
    def set_network_interface_enable_state(self, interface_index, enable_state):
        """
        Parameters
        ----------
        interface_index : int
        enable_state : bool

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef  unsigned char enableState
        c_interface_index = int(interface_index)
        enableState = 1 if enable_state else 0
        self.sbapi.setNetworkInterfaceEnableState(self.device_id, self.feature_id, &error_code, c_interface_index, enableState)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned char runNetworkInterfaceSelfTest(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def run_network_interface_self_test(self, interface_index):
        """run network interface self test

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        passed_test : bool
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.runNetworkInterfaceSelfTest(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(output)

    # void saveNetworkInterfaceConnectionSettings(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def save_network_interface_connection_settings(self, interface_index):
        """save network interface connection settings

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        self.sbapi.saveNetworkInterfaceConnectionSettings(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeWifiConfigurationFeature(SeaBreezeFeature):

    identifier = "wifi_configuration"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfWifiConfigurationFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getWifiConfigurationFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char getWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_wifi_mode(self, interface_index):
        """get wifi configuration mode

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        wifi_mode : int
	        mode `{0: 'client', 1: 'access point'}`
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.getWifiConfigurationMode(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void setWifiConfigurationMode(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char mode)
    def set_wifi_mode(self, interface_index, wifi_mode):
        """set wifi mode

        Parameters
        ----------
        interface_index : int
        wifi_mode : int
	        mode `{0: 'client', 1: 'access point'}`

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char mode
        c_interface_index = int(interface_index)
        mode = int(wifi_mode)
        self.sbapi.setWifiConfigurationMode(self.device_id, self.feature_id, &error_code, c_interface_index, mode)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned char getWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index)
    def get_wifi_security_type(self, interface_index):
        """get wifi security type

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        security_type : int
	        security_type `{0: 'open', 1: 'WPA2'}`
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        c_interface_index = int(interface_index)
        output = self.sbapi.getWifiConfigurationSecurityType(self.device_id, self.feature_id, &error_code, c_interface_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void   setWifiConfigurationSecurityType(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char securityType)
    def set_wifi_security_type(self, interface_index, security_type):
        """set wifi security type

        Parameters
        ----------
        interface_index : int
        security_type : int
	        security_type `{0: 'open', 1: 'WPA2'}`

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char securityType
        c_interface_index = int(interface_index)
        securityType = int(security_type)
        self.sbapi.setWifiConfigurationSecurityType(self.device_id, self.feature_id, &error_code, c_interface_index, securityType)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned char   getWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, unsigned char(*ssid)[32])
    def get_wifi_ssid(self, interface_index):
        """get wifi ssid

        Parameters
        ----------
        interface_index : int

        Returns
        -------
        ssid : str
        """
        cdef int error_code
        cdef unsigned char output
        cdef unsigned char c_interface_index
        cdef unsigned char(*ssid)[32]
        cdef unsigned char* ssid_view
        c_interface_index = int(interface_index)
        ssid = <unsigned char(*)[32]> PyMem_Malloc(sizeof(unsigned char[32]))
        if not ssid:
            raise MemoryError("can't allocate memory for ssid")
        try:
            output = self.sbapi.getWifiConfigurationSSID(self.device_id, self.feature_id, &error_code, c_interface_index, ssid)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            ssid_view = <unsigned char*> ssid
            return str(ssid_view[:output])
        finally:
            PyMem_Free(ssid)

    # void setWifiConfigurationSSID(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index, const unsigned char ssid[32], unsigned char length)
    def set_wifi_ssid(self, interface_index, ssid):
        """set wifi ssid

        Parameters
        ----------
        interface_index : int
        ssid : str

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char c_ssid[32]
        cdef unsigned char length
        c_interface_index = int(interface_index)
        length = len(ssid)
        if length > 32:
            raise ValueError("maxlength ssid is 32")
        cbytes = bytes(ssid)
        for i in range(length):
            c_ssid[i] = cbytes[i]
        self.sbapi.setWifiConfigurationSSID(self.device_id, self.feature_id, &error_code, c_interface_index, c_ssid, length)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # void setWifiConfigurationPassPhrase(long deviceID, long featureID, int *errorCode, unsigned char c_interface_index,
    #                                     const unsigned char *passPhrase, unsigned char passPhraseLength)
    def set_wifi_pass_phrase(self, interface_index, pass_phrase):
        """set wifi pass phrase

        Parameters
        ----------
        interface_index : int
        pass_phrase : str

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_interface_index
        cdef unsigned char* passPhrase
        cdef unsigned char passPhraseLength
        c_interface_index = int(interface_index)
        passPhraseLength = len(pass_phrase)
        cbytes = bytes(pass_phrase)
        passPhrase = <unsigned char*> PyMem_Malloc(passPhraseLength * sizeof(unsigned char))
        if not passPhrase:
            raise MemoryError("can't allocate memory for passPhrase")
        try:
            for i in range(passPhraseLength):
                passPhrase[i] = cbytes[i]
            self.sbapi.setWifiConfigurationPassPhrase(self.device_id, self.feature_id, &error_code, c_interface_index, passPhrase, passPhraseLength)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
        finally:
            PyMem_Free(passPhrase)


cdef class SeaBreezeGPIOFeature(SeaBreezeFeature):

    identifier = "gpio"

    modes = dict(
        GPIO_OUTPUT_PUSH_PULL=0x00,
        GPIO_OPEN_DRAIN_OUTPUT=0x01,
        DAC_OUTPUT=0x02,
        GPIO_INPUT_HIGH_Z=0x80,
        GPIO_INPUT_PULL_DOWN=0x81,
        ADC_INPUT=0x82,
    )

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfGPIOFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getGPIOFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char getGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode)
    def get_number_of_gpio_pins(self):
        """number of gpio pins on device

        Returns
        -------
        num_gpio_pins : int
        """
        cdef int error_code
        cdef unsigned char num_gpio_pins
        num_gpio_pins = self.sbapi.getGPIO_NumberOfPins(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(num_gpio_pins)

    # unsigned int getGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode)
    def get_gpio_output_enable_vector(self):
        """return the current gpio output enable vector

        Returns
        -------
        output : unsigned int
        """
        cdef int error_code
        cdef unsigned int output
        output = self.sbapi.getGPIO_OutputEnableVector(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void setGPIO_OutputEnableVector(long deviceID, long featureID, int *errorCode, unsigned int outputEnableVector, unsigned int bitMask)
    def set_gpio_output_enable_vector(self, output_enable_vector, bit_mask):
        """set the current gpio output enable vector

        Parameters
        ----------
        output_enable_vector : int
        bit_mask : int

        Returns
        -------
        None
        """
        cdef int error_code

        cdef unsigned int outputEnableVector
        cdef  unsigned int bitMask
        outputEnableVector = int(output_enable_vector)
        bitMask = int(bit_mask)
        self.sbapi.setGPIO_OutputEnableVector(self.device_id, self.feature_id, &error_code, outputEnableVector, bitMask)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


    # unsigned int getGPIO_ValueVector(long deviceID, long featureID, int *errorCode)
    def get_gpio_value_vector(self):
        """get gpio value vector

        Returns
        -------
        output : int
        """
        cdef int error_code
        cdef unsigned int output
        output = self.sbapi.getGPIO_ValueVector(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void setGPIO_ValueVector(long deviceID, long featureID, int *errorCode, unsigned int valueVector, unsigned int bitMask)
    def set_gpio_value_vector(self, value_vector, bit_mask):
        """set gpio value vector

        allows to set multiple pins at once

        Parameters
        ----------
        value_vector : int
        bit_mask :  int

        Returns
        -------
        None
        """
        cdef int error_code

        cdef unsigned int valueVector
        cdef  unsigned int bitMask
        valueVector = int(value_vector)
        bitMask = int(bit_mask)
        self.sbapi.setGPIO_ValueVector(self.device_id, self.feature_id, &error_code, valueVector, bitMask)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned char getEGPIO_NumberOfPins(long deviceID, long featureID, int *errorCode)
    def get_number_of_egpio_pins(self):
        """return the number of egpio pins on the device

        Returns
        -------
        num_egpio_pins : int
        """
        cdef int error_code
        cdef unsigned char num_egpio_pins
        num_egpio_pins = self.sbapi.getEGPIO_NumberOfPins(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(num_egpio_pins)

    # unsigned char getEGPIO_AvailableModes(long deviceID, long featureID, int *errorCode, unsigned char pinNumber,
    #                                       unsigned char *availableModes, unsigned char maxModeCount)
    def get_egpio_available_modes(self, pin_number):
        """return all available modes for the specific egpio pin

        see `SeaBreezeGPIOFeature.modes` for all possible modes

        Parameters
        ----------
        pin_number : int

        Returns
        -------
        available_modes : set of int
        """
        cdef int error_code
        # , availableModes, maxModeCount
        cdef unsigned char modes_written
        cdef unsigned char pinNumber
        cdef  unsigned char* availableModes
        cdef  unsigned char maxModeCount
        pinNumber = int(pin_number)
        # let's allocate two times the amount of labeled modes
        # this might be wrong, at most use 256
        n_max_mode_count = min(2 * len(self.modes), 256)
        availableModes = <unsigned char*> PyMem_Malloc(n_max_mode_count * sizeof(unsigned char))
        if not availableModes:
            raise MemoryError("could not allocate memory for availableModes")
        try:
            maxModeCount = n_max_mode_count
            modes_written = self.sbapi.getEGPIO_AvailableModes(self.device_id, self.feature_id, &error_code,
                                                               pinNumber, availableModes, maxModeCount)
            assert modes_written < n_max_mode_count, "BUG: probably more modes available for pin"
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            modes = set()
            for i in range(modes_written):
                modes.add(int(availableModes[i]))
        finally:
            PyMem_Free(availableModes)
        return modes

    # unsigned char getEGPIO_CurrentMode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber)
    def get_egpio_current_mode(self, pin_number):
        """get the current egpio mode for a pin

        see `SeaBreezeGPIOFeature.modes`

        Parameters
        ----------
        pin_number : int

        Returns
        -------
        mode : int
        """
        cdef int error_code
        cdef unsigned char mode
        cdef unsigned char pinNumber
        pinNumber = int(pin_number)
        mode = self.sbapi.getEGPIO_CurrentMode(self.device_id, self.feature_id, &error_code, pinNumber)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(mode)

    # void setEGPIO_Mode(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, unsigned char mode, float value)
    def set_egpio_mode(self, pin_number, mode, value=0.0):
        """set the egpio mode for a specific pin

        Parameters
        ----------
        pin_number : int
        mode : int
            mode value from `SeaBreezeGPIOFeature.modes`
        value : float, Optional
            optional value from 0.0 to 1.0 (???)

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char pinNumber
        cdef unsigned char c_mode
        cdef float c_value
        pinNumber = int(pin_number)
        c_mode = int(mode)
        c_value = int(value)
        self.sbapi.setEGPIO_Mode(self.device_id, self.feature_id, &error_code, pinNumber, c_mode, value)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned int getEGPIO_OutputVector(long deviceID, long featureID, int *errorCode)
    def get_egpio_output_vector_vector(self):
        """return the egpio output vector

        Returns
        -------
        output_vector : int
        """
        cdef int error_code
        cdef unsigned int output_vector
        output_vector = self.sbapi.getEGPIO_OutputVector(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output_vector)

    # void setEGPIO_OutputVector(long deviceID, long featureID, int *errorCode, unsigned int outputVector, unsigned int bitMask)
    def set_egpio_output_vector(self, output_vector, bit_mask):
        """set the egpio output vector

        allows to set multiple pins at once

        Parameters
        ----------
        output_vector : int
        bit_mask : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned int outputVector
        cdef unsigned int bitMask
        outputVector = int(output_vector)
        bitMask = int(bit_mask)
        self.sbapi.setEGPIO_OutputVector(self.device_id, self.feature_id, &error_code, outputVector, bitMask)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


    # float getEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber)
    def get_egpio_value(self, pin_number):
        """return the value of an egpio pin

        Parameters
        ----------
        pin_number : int

        Returns
        -------
        value : float
            egpio value between 0.0 and 1.0
        """
        cdef int error_code
        cdef float value
        cdef unsigned char pinNumber
        pinNumber = int(pin_number)
        value = self.sbapi.getEGPIO_Value(self.device_id, self.feature_id, &error_code, pinNumber)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return value

    # void setEGPIO_Value(long deviceID, long featureID, int *errorCode, unsigned char pinNumber, float value)
    def set_egpio_value(self, pin_number, value):
        """set the value of an egpio pin

        Parameters
        ----------
        pin_number : int
        value : float
            egpio value between 0.0 and 1.0

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char pinNumber
        cdef float c_value
        pinNumber = int(pin_number)
        c_value = float(value)
        self.sbapi.setEGPIO_Value(self.device_id, self.feature_id, &error_code, pinNumber, c_value)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeEEPROMFeature(SeaBreezeFeature):

    identifier = "eeprom"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfEEPROMFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getEEPROMFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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
        cdef unsigned char c_buffer[_MAXBUFLEN]
        cdef int bytes_written
        try:
            bytes_written = self.sbapi.eepromReadSlot(self.device_id, self.feature_id, &error_code,
                                                      slot_number, c_buffer, _MAXBUFLEN)
        except ValueError:
            raise SeaBreezeError("EEProm slot out of bounds.")
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        if strip_zero_bytes:
            return c_buffer[:bytes_written].strip('\x00')
        return c_buffer[:bytes_written]


cdef class SeaBreezeLightSourceFeature(SeaBreezeFeature):

    identifier = "light_source"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfLightSourceFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getLightSourceFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    def get_count(self):
        """returns the total number of light sources available

        This function gets the number of light sources that are represented by
        the given featureID.  Such light sources could be individual LEDs,
        light bulbs, lasers, etc.  Each of these light sources may have different
        capabilities, such as programmable intensities and enables, which should
        be queried before they are used.

        Returns
        -------
        number_of_light_sources: int
        """
        cdef int error_code
        cdef int ls_count
        ls_count = self.sbapi.lightSourceGetCount(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(ls_count)

    def has_enable(self, int light_source_index):
        """returns if light source can be enabled

        Queries whether the indicated light source within the given feature
        instance has a usable enable/disable control.

        Parameters
        ----------
        light_source_index : int

        Returns
        -------
        has_enable: bool
        """
        cdef int error_code
        cdef bool_t has_enable
        has_enable = self.sbapi.lightSourceHasEnable(self.device_id, self.feature_id, &error_code, light_source_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(has_enable)

    def is_enabled(self, int light_source_index):
        """returns if light source is enabled

        Parameters
        ----------
        light_source_index : int

        Returns
        -------
        is_enabled: bool
        """
        cdef int error_code
        cdef bool_t is_enable
        is_enabled = self.sbapi.lightSourceIsEnabled(self.device_id, self.feature_id, &error_code, light_source_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(is_enabled)

    def set_enable(self, int light_source_index, bool_t enable):
        """enable or disable the light source

        Attempts to enable or disable the indicated light source. Not all light
        sources have an enable/disable control, and this capability can be queried
        with has_enable(). Note that an enabled light source should emit light
        according to its last (or default) intensity setting which might be the
        minimum; in this case, the light source might appear to remain off.

        Parameters
        ----------
        light_source_index : int
        enable: bool

        Returns
        -------
        None
        """
        cdef int error_code
        self.sbapi.lightSourceSetEnable(self.device_id, self.feature_id, &error_code, light_source_index, enable)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def has_variable_intensity(self, int light_source_index):
        """returns if light source has variable intensity

        Parameters
        ----------
        light_source_index : int

        Returns
        -------
        is_enabled: bool
        """
        cdef int error_code
        cdef bool_t has_vi
        has_vi = self.sbapi.lightSourceHasVariableIntensity(self.device_id, self.feature_id, &error_code,
                                                            light_source_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return bool(has_vi)

    def get_intensity(self, int light_source_index):
        """queries the normalized intensity level of the indicated light source

        Parameters
        ----------
        light_source_index : int

        Returns
        -------
        intensity: float
            The intensity is normalized over the range [0, 1], with 0 as the minimum and 1 as the maximum.
        """
        cdef int error_code
        cdef double intensity
        intensity = self.sbapi.lightSourceGetIntensity(self.device_id, self.feature_id, &error_code, light_source_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return float(intensity)

    def set_intensity(self, int light_source_index, double intensity):
        """set the normalized intensity level of the indicated light source

        Parameters
        ----------
        light_source_index : int
        intensity: float
            The intensity is normalized over the range [0, 1], with 0 as the minimum and 1 as the maximum.

        Returns
        -------
        None
        """
        cdef int error_code
        self.sbapi.lightSourceSetIntensity(self.device_id, self.feature_id, &error_code, light_source_index, intensity)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeStrobeLampFeature(SeaBreezeFeature):

    identifier = "strobe_lamp"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfLampFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getLampFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfContinuousStrobeFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getContinuousStrobeFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    def set_enable(self, strobe_enable):
        """enable continuous strobe

        Parameters
        ----------
        strobe_enable : bool
            on or off

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_enable
        c_enable = 0 if not strobe_enable else 1
        self.sbapi.continuousStrobeSetContinuousStrobeEnable(self.device_id, self.feature_id, &error_code, c_enable)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    def set_period_micros(self, period_micros):
        """set continuous strobe period in microseconds

        Parameters
        ----------
        period_micros : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned long c_micros
        c_micros = int(period_micros)
        # void (long deviceID, long featureID, int *errorCode, unsigned long strobePeriodMicroseconds)
        self.sbapi.continuousStrobeSetContinuousStrobePeriodMicroseconds(self.device_id, self.feature_id, &error_code,
                                                                         c_micros)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeShutterFeature(SeaBreezeFeature):

    identifier = "shutter"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfShutterFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getShutterFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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


cdef class SeaBreezeNonlinearityCoefficientsFeature(SeaBreezeFeature):

    identifier = "nonlinearity_coefficients"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfNonlinearityCoeffsFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getNonlinearityCoeffsFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfTemperatureFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getTemperatureFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char temperatureCountGet(long deviceID, long featureID, int *errorCode)
    def count_temperatures(self):
        """reads out an the number of indexed temperatures available

        Returns
        -------
        num_temperatures : int
        """
        cdef int error_code
        cdef unsigned char num_temperatures
        num_temperatures = self.sbapi.temperatureCountGet(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(num_temperatures)

    # double temperatureGet(long deviceID, long featureID, int *errorCode, int index)
    def read_temperature(self, index):
        """read a specific temperature

        Parameters
        ----------
        index : int
            index of the temperature to be received

        Returns
        -------
        temperature : double
        """
        cdef int error_code
        cdef double output
        cdef int c_index
        c_index = int(index)
        output = self.sbapi.temperatureGet(self.device_id, self.feature_id, &error_code, c_index)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return float(output)

    # int temperatureGetAll(long deviceID, long featureID, int *errorCode, double *buffer, int maxLength)
    def temperature_get_all(self):
        """read all available temperatures

        Returns
        -------
        temperatures : tuple of float
        """
        cdef int error_code
        cdef int output
        cdef double* c_buffer
        cdef int maxLength
        maxLength = self.count_temperatures()
        c_buffer = <double *> PyMem_Malloc(maxLength * sizeof(double))
        if not c_buffer:
            raise MemoryError("could not allocate memory for temperatures")
        temperatures = []
        try:
            output = self.sbapi.temperatureGetAll(self.device_id, self.feature_id, &error_code, c_buffer, maxLength)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            for i in range(output):
                temperatures.append(float(c_buffer[i]))
        finally:
            PyMem_Free(c_buffer)
        return tuple(temperatures)


cdef class SeaBreezeIntrospectionFeature(SeaBreezeFeature):

    identifier = "introspection"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfIntrospectionFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getIntrospectionFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned short int introspectionNumberOfPixelsGet(long deviceID, long featureID, int *errorCode)
    def number_of_pixels(self):
        """read the number of detector pixels available

        Returns
        -------
        num_pixels : int
        """
        cdef int error_code
        cdef unsigned short int num_pixels
        num_pixels = self.sbapi.introspectionNumberOfPixelsGet(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(num_pixels)

    # int introspectionActivePixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength)
    def get_active_pixel_ranges(self):
        """return the active pixel indices of a device

        Returns
        -------
        output : tuple of int
        """
        cdef int error_code
        cdef int output
        cdef unsigned int* pixelIndexPairs
        cdef int maxLength
        maxLength = self.number_of_pixels()
        pixelIndexPairs = <unsigned int*> PyMem_Malloc(maxLength * sizeof(unsigned int))
        if not pixelIndexPairs:
            raise MemoryError("can't allocate memory for pixelIndexPairs")
        active_pixels = []
        try:
            output = self.sbapi.introspectionActivePixelRangesGet(self.device_id, self.feature_id, &error_code, pixelIndexPairs, maxLength)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            for i in range(output):
                active_pixels.append(int(pixelIndexPairs[i]))
        finally:
            PyMem_Free(pixelIndexPairs)
        return tuple(active_pixels)

    # int introspectionOpticalDarkPixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength)
    def get_optical_dark_pixel_ranges(self):
        """return the optical dark pixel indices of a device

        Returns
        -------
        output : tuple of int
        """
        cdef int error_code
        cdef int output
        cdef unsigned int* pixelIndexPairs
        cdef  int maxLength
        maxLength = self.number_of_pixels()
        pixelIndexPairs = <unsigned int*> PyMem_Malloc(maxLength * sizeof(unsigned int))
        if not pixelIndexPairs:
            raise MemoryError("can't allocate memory for pixelIndexPairs")
        dark_pixels = []
        try:
            output = self.sbapi.introspectionOpticalDarkPixelRangesGet(self.device_id, self.feature_id, &error_code,
                                                                       pixelIndexPairs, maxLength)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            for i in range(output):
                dark_pixels.append(int(pixelIndexPairs[i]))
        finally:
            PyMem_Free(pixelIndexPairs)
        return tuple(dark_pixels)

    # int introspectionElectricDarkPixelRangesGet(long deviceID, long featureID, int *errorCode, unsigned int *pixelIndexPairs, int maxLength)
    def get_electric_dark_pixel_ranges(self):
        """return the electric dark pixel indices of a device

        Returns
        -------
        output : tuple of int
        """
        cdef int error_code
        cdef int output
        cdef unsigned int* pixelIndexPairs
        cdef  int maxLength
        maxLength = self.number_of_pixels()
        pixelIndexPairs = <unsigned int*> PyMem_Malloc(maxLength * sizeof(unsigned int))
        if not pixelIndexPairs:
            raise MemoryError("can't allocate memory for pixelIndexPairs")
        electric_dark_pixels = []
        try:
            output = self.sbapi.introspectionElectricDarkPixelRangesGet(self.device_id, self.feature_id, &error_code,
                                                                        pixelIndexPairs, maxLength)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            for i in range(output):
                electric_dark_pixels.append(int(pixelIndexPairs[i]))
        finally:
            PyMem_Free(pixelIndexPairs)
        return tuple(electric_dark_pixels)


cdef class SeaBreezeSpectrumProcessingFeature(SeaBreezeFeature):

    identifier = "spectrum_processing"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfSpectrumProcessingFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getSpectrumProcessingFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    def get_boxcar_width(self):
        cdef unsigned char boxcar_width
        cdef int error_code
        boxcar_width = self.sbapi.spectrumProcessingBoxcarWidthGet(self.device_id, self.feature_id, &error_code)
        if boxcar_width < 0 or error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(boxcar_width)

    def set_boxcar_width(self, boxcar_width):
        """

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char c_boxcar_width
        c_boxcar_width = int(boxcar_width)
        self.sbapi.spectrumProcessingBoxcarWidthSet(self.device_id, self.feature_id, &error_code, c_boxcar_width)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    boxcar_width = property(get_boxcar_width, set_boxcar_width)

    def get_scans_to_average(self):
        cdef unsigned short int scans_to_average
        cdef int error_code
        scans_to_average = self.sbapi.spectrumProcessingScansToAverageGet(self.device_id, self.feature_id, &error_code)
        if scans_to_average < 1 or error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(scans_to_average)

    def set_scans_to_average(self, scans_to_average):
        """

        Parameters
        ----------
        scans_to_average : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned short int c_scans_to_average
        c_scans_to_average = int(scans_to_average)
        self.sbapi.spectrumProcessingScansToAverageSet(self.device_id, self.feature_id, &error_code, c_scans_to_average)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    scans_to_average = property(get_scans_to_average, set_scans_to_average)


cdef class SeaBreezeRevisionFeature(SeaBreezeFeature):

    identifier = "revision"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfRevisionFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getRevisionFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char revisionHardwareGet(long deviceID, long featureID, int *errorCode)
    def hardware_revision(self):
        """returns the hardware revision

        Returns
        -------
        revision : int
        """
        cdef int error_code
        cdef unsigned char output
        output = self.sbapi.revisionHardwareGet(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned short int revisionFirmwareGet(long deviceID, long featureID, int *errorCode)
    def revision_firmware_get(self):
        """returns the firmware revision

        Returns
        -------
        output : int
        """
        cdef int error_code
        cdef unsigned short int output
        output = self.sbapi.revisionFirmwareGet(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)


cdef class SeaBreezeOpticalBenchFeature(SeaBreezeFeature):

    identifier = "optical_bench"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfOpticalBenchFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getOpticalBenchFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned short int opticalBenchGetFiberDiameterMicrons(long deviceID, long featureID, int *errorCode)
    def get_fiber_diameter_microns(self):
        """return the fiber diameter in micrometers

        Returns
        -------
        fiber_diameter : int
        """
        cdef int error_code
        cdef unsigned short int output
        output = self.sbapi.opticalBenchGetFiberDiameterMicrons(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned short int opticalBenchGetSlitWidthMicrons(long deviceID, long featureID, int *errorCode)
    def get_slit_width_microns(self):
        """return the slit width in micrometers

        Returns
        -------
        slit_width : int
        """
        cdef int error_code
        cdef unsigned short int output
        output = self.sbapi.opticalBenchGetSlitWidthMicrons(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # int opticalBenchGetID(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
    def get_id(self):
        """return the bench id

        Returns
        -------
        bench_id : str
        """
        cdef int error_code
        cdef int bytes_written
        cdef char buffer[_MAXBUFLEN]
        bytes_written = self.sbapi.opticalBenchGetID(self.device_id, self.feature_id, &error_code,
                                                     &buffer[0], _MAXBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert bytes_written < _MAXBUFLEN, "BUG: should increase hardcoded buffer size"
        return str(buffer[:bytes_written])

    # int opticalBenchGetSerialNumber(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
    def get_serial_number(self):
        """return the serial number

        Returns
        -------
        serial_number : str
        """
        cdef int error_code
        cdef int bytes_written
        cdef char buffer[_MAXBUFLEN]
        bytes_written = self.sbapi.opticalBenchGetSerialNumber(self.device_id, self.feature_id, &error_code,
                                                               &buffer[0], _MAXBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert bytes_written < _MAXBUFLEN, "BUG: should increase hardcoded buffer size"
        return str(buffer[:bytes_written])

    # int opticalBenchGetCoating(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
    def get_coating(self):
        """return the bench coating

        Returns
        -------
        coating : str
        """
        cdef int error_code
        cdef int bytes_written
        cdef char buffer[_MAXBUFLEN]
        bytes_written = self.sbapi.opticalBenchGetCoating(self.device_id, self.feature_id, &error_code,
                                                          &buffer[0], _MAXBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert bytes_written < _MAXBUFLEN, "BUG: should increase hardcoded buffer size"
        return str(buffer[:bytes_written])

    # int opticalBenchGetFilter(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
    def get_filter(self):
        """return the bench filter

        Returns
        -------
        filter : str
        """
        cdef int error_code
        cdef int bytes_written
        cdef char buffer[_MAXBUFLEN]
        bytes_written = self.sbapi.opticalBenchGetFilter(self.device_id, self.feature_id, &error_code,
                                                         &buffer[0], _MAXBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert bytes_written < _MAXBUFLEN, "BUG: should increase hardcoded buffer size"
        return str(buffer[:bytes_written])

    # int opticalBenchGetGrating(long deviceID, long featureID, int *errorCode, char *buffer, int bufferLength)
    def get_grating(self):
        """return the bench grating

        Returns
        -------
        output : int
        """
        cdef int error_code
        cdef int bytes_written
        cdef char buffer[_MAXBUFLEN]
        bytes_written = self.sbapi.opticalBenchGetGrating(self.device_id, self.feature_id, &error_code,
                                                   &buffer[0], _MAXBUFLEN)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert bytes_written < _MAXBUFLEN, "BUG: should increase hardcoded buffer size"
        return str(buffer[:bytes_written])


cdef class SeaBreezeStrayLightCoefficientsFeature(SeaBreezeFeature):

    identifier = "stray_light_coefficients"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfStrayLightCoeffsFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getStrayLightCoeffsFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
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
        coefficients = [float(ccoeffs[i]) for i in range(values_written)]
        return coefficients


cdef class SeaBreezeDataBufferFeature(SeaBreezeFeature):

    identifier = "data_buffer"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfDataBufferFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getDataBufferFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # void dataBufferClear(long deviceID, long featureID, int *errorCode)
    def clear(self):
        """clear the data buffer

        Returns
        -------
        None
        """
        cdef int error_code
        self.sbapi.dataBufferClear(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # void dataBufferRemoveOldestSpectra(long deviceID, long featureID, int *errorCode, unsigned int numberOfSpectra)
    def remove_oldest_spectra(self, number_of_spectra):
        """remove the oldest data from the buffer

        Parameters
        ----------
        number_of_spectra : int
            Number of spectra from oldest to newest to remove

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned int numberOfSpectra
        numberOfSpectra = int(number_of_spectra)
        self.sbapi.dataBufferRemoveOldestSpectra(self.device_id, self.feature_id, &error_code, numberOfSpectra)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned long dataBufferGetNumberOfElements(long deviceID, long featureID, int *errorCode)
    def get_number_of_elements(self):
        """get the number of data elements currently in the buffer

        Returns
        -------
        num_elements : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.dataBufferGetNumberOfElements(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned long dataBufferGetBufferCapacity(long deviceID, long featureID, int *errorCode)
    def get_buffer_capacity(self):
        """get the present limit of how many data elements will be retained by the buffer.

        This value can be changed with `set_buffer_capacity`

        Returns
        -------
        buffer_capacity : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.dataBufferGetBufferCapacity(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void dataBufferSetBufferCapacity(long deviceID, long featureID, int *errorCode, unsigned long capacity)
    def set_buffer_capacity(self, capacity):
        """set the number of data elements that the buffer should retain

        Parameters
        ----------
        capacity : int
            Limit on the number of data elements to store.  This is
            bounded by what is returned by get_buffer_capacity_minimum()
            and get_buffer_capacity_maximum().

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned long c_capacity
        c_capacity = int(capacity)
        self.sbapi.dataBufferSetBufferCapacity(self.device_id, self.feature_id, &error_code, c_capacity)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned long dataBufferGetBufferCapacityMaximum(long deviceID, long featureID, int *errorCode)
    def get_buffer_capacity_maximum(self):
        """get the maximum possible configurable size for the data buffer

        Returns
        -------
        max_capacity : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.dataBufferGetBufferCapacityMaximum(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned long dataBufferGetBufferCapacityMinimum(long deviceID, long featureID, int *errorCode)
    def get_buffer_capacity_minimum(self):
        """get the minimum possible configurable size for the data buffer

        Returns
        -------
        min_capacity : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.dataBufferGetBufferCapacityMinimum(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)


cdef class SeaBreezeFastBufferFeature(SeaBreezeFeature):

    identifier = "fast_buffer"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfFastBufferFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getFastBufferFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char fastBufferGetBufferingEnable(long deviceID, long featureID, int *errorCode)
    def get_buffering_enable(self):
        """returns the fast buffer enable state

        Returns
        -------
        state : bool
        """
        cdef int error_code
        cdef unsigned char output
        output = self.sbapi.fastBufferGetBufferingEnable(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return False if output == 0 else True

    # void fastBufferSetBufferingEnable(long deviceID, long featureID, int *errorCode, unsigned char isEnabled)
    def set_buffering_enable(self, is_enabled):
        """
        Parameters
        ----------
        is_enabled : bool

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned char isEnabled
        isEnabled = 0 if not is_enabled else 1
        self.sbapi.fastBufferSetBufferingEnable(self.device_id, self.feature_id, &error_code, isEnabled)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned int fastBufferGetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode)
    def get_consecutive_sample_count(self):
        """returns the number of consecutive samples

        Returns
        -------
        num_samples : int
        """
        cdef int error_code
        cdef unsigned int output
        output = self.sbapi.fastBufferGetConsecutiveSampleCount(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # void fastBufferSetConsecutiveSampleCount(long deviceID, long featureID, int *errorCode, unsigned int consecutiveSampleCount)
    def set_consecutive_sample_count(self, consecutive_sample_count):
        """set the number of consecutive samples

        Parameters
        ----------
        consecutive_sample_count : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned int consecutiveSampleCount
        consecutiveSampleCount = int(consecutive_sample_count)
        self.sbapi.fastBufferSetConsecutiveSampleCount(self.device_id, self.feature_id, &error_code, consecutiveSampleCount)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)


cdef class SeaBreezeAcquisitionDelayFeature(SeaBreezeFeature):

    identifier = "acquisition_delay"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfAcquisitionDelayFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getAcquisitionDelayFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # void acquisitionDelaySetDelayMicroseconds(long deviceID, long featureID, int *errorCode, unsigned long delay_usec)
    def set_delay_microseconds(self, delay_usec):
        """set the acquisition delay in microseconds

        Parameters
        ----------
        delay_usec : int

        Returns
        -------
        None
        """
        cdef int error_code
        cdef unsigned long c_delay_usec
        c_delay_usec = int(delay_usec)
        self.sbapi.acquisitionDelaySetDelayMicroseconds(self.device_id, self.feature_id, &error_code, c_delay_usec)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)

    # unsigned long acquisitionDelayGetDelayMicroseconds(long deviceID, long featureID, int *errorCode)
    def get_delay_microseconds(self):
        """get the acquisition delay in microseconds

        Returns
        -------
        delay : int
        """
        cdef int error_code
        cdef unsigned long delay
        delay = self.sbapi.acquisitionDelayGetDelayMicroseconds(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(delay)

    delay_microseconds = property(get_delay_microseconds, set_delay_microseconds)

    # unsigned long acquisitionDelayGetDelayIncrementMicroseconds(long deviceID, long featureID, int *errorCode)
    def get_delay_increment_microseconds(self):
        """get the acquisition delay increment in microseconds

        Returns
        -------
        delay increment : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.acquisitionDelayGetDelayIncrementMicroseconds(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned long acquisitionDelayGetDelayMaximumMicroseconds(long deviceID, long featureID, int *errorCode)
    def get_maximum_delay_microseconds(self):
        """get the maximum acquisition delay in microseconds

        Returns
        -------
        maximum_delay : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.acquisitionDelayGetDelayMaximumMicroseconds(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned long acquisitionDelayGetDelayMinimumMicroseconds(long deviceID, long featureID, int *errorCode)
    def get_minimum_delay_microseconds(self):
        """get the minimum acquisition delay in microseconds

        Returns
        -------
        minimum_delay : int
        """
        cdef int error_code
        cdef unsigned long output
        output = self.sbapi.acquisitionDelayGetDelayMinimumMicroseconds(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)


cdef class SeaBreezeI2CMasterFeature(SeaBreezeFeature):

    identifier = "i2c_master"

    @classmethod
    def _get_feature_ids_from_device(cls, SeaBreezeDevice device):  # autogenerated
        cdef int num_features, error_code
        cdef csb.SeaBreezeAPI* sbapi = csb.SeaBreezeAPI.getInstance()
        num_features = sbapi.getNumberOfI2CMasterFeatures(device.handle, &error_code)
        cls._raise_if_error(error_code, num_features)
        py_feature_ids = []
        if num_features != 0:
            feature_ids = <long*> PyMem_Malloc(num_features * sizeof(long))
            if not feature_ids:
                raise MemoryError("could not allocate memory for feature_ids")
            try:
                sbapi.getI2CMasterFeatures(device.handle, &error_code, feature_ids, num_features)
                cls._raise_if_error(error_code, num_features)
                py_feature_ids = [feature_ids[i] for i in range(num_features)]
            finally:
                PyMem_Free(feature_ids)
        return py_feature_ids

    # unsigned char i2cMasterGetNumberOfBuses(long deviceID, long featureID, int *errorCode)
    def get_number_of_buses(self):
        """return the number of i2c buses

        Returns
        -------
        num_buses : int
        """
        cdef int error_code
        cdef unsigned char output
        output = self.sbapi.i2cMasterGetNumberOfBuses(self.device_id, self.feature_id, &error_code)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return int(output)

    # unsigned short i2cMasterReadBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, unsigned char *readData, unsigned short numberOfBytes)
    def read_bus(self, bus_index, slave_address, buffer_length=1024):
        """read data from the i2c bus

        Parameters
        ----------
        bus_index : int
        slave_address : int
        buffer_length : int, optional

        Returns
        -------
        data : str
        """
        cdef int error_code
        cdef unsigned short output
        cdef unsigned char busIndex
        cdef unsigned char slaveAddress
        cdef unsigned char* readData
        cdef unsigned short numberOfBytes
        busIndex = int(bus_index)
        slaveAddress = int(slave_address)
        readData = <unsigned char*> PyMem_Malloc(buffer_length * sizeof(unsigned char))
        numberOfBytes = buffer_length
        if not readData:
            raise MemoryError("can't allocate memory for data")
        try:
            output = self.sbapi.i2cMasterReadBus(self.device_id, self.feature_id, &error_code, busIndex, slaveAddress,
                                                 readData, numberOfBytes)
            if error_code != 0:
                raise SeaBreezeError(error_code=error_code)
            assert output <= buffer_length, "BUG: buffer_length probably to small"
            out = readData[:output]
        finally:
            PyMem_Free(readData)
        return out

    # unsigned short i2cMasterWriteBus(long deviceID, long featureID, int *errorCode, unsigned char busIndex, unsigned char slaveAddress, const unsigned char *writeData, unsigned short numberOfBytes)
    def write_bus(self, bus_index, slave_address, data):
        """write data to i2c bus

        Parameters
        ----------
        bus_index : int
        slave_address : int
        data : str

        Returns
        -------
        None
        """
        cdef int error_code

        cdef unsigned short output
        cdef unsigned char busIndex
        cdef unsigned char slaveAddress
        cdef const unsigned char* writeData
        cdef unsigned short numberOfBytes
        assert 0 <= int(bus_index) <= 256
        assert 0 <= int(slave_address) <= 256
        busIndex = int(bus_index)
        slaveAddress = int(slave_address)
        numberOfBytes = len(data)
        bdata = bytes(data)
        writeData = bdata
        output = self.sbapi.i2cMasterWriteBus(self.device_id, self.feature_id, &error_code, busIndex,
                                              slaveAddress, writeData, numberOfBytes)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        assert output == numberOfBytes, "BUG: didn't write all data"
