"""This is the cython Wrapper for the seabreeze library

Author: Andreas Poehlmann

"""
cimport cseabreeze as csb

import cython

from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

# define max length for some strings
DEF SBMAXBUFLEN = 32
DEF SBMAXDBUFLEN = 256

class SeaBreezeError(Exception):

    def __init__(self, message=None, error_code=None):
        cdef const char *cmessage
        if error_code is not None:
            cmessage = csb.sbapi_get_error_string(error_code)
            message = str(cmessage)
        elif message is not None:
            pass
        else:
            message = ""
        # Call the base class constructor with the parameters it needs
        super(SeaBreezeError, self).__init__(str(message))
        self.error_code = error_code


cdef class SeaBreezeDevice(object):

    cdef public long handle
    cdef public str model, serial

    def __init__(self, handle, model, serial):
        self.handle = handle
        try:
            self.model = model
        except TypeError:
            self.model = model.encode("utf-8")
        try:
            self.serial = serial
        except TypeError:
            self.serial = serial.encode("utf-8")

    def __repr__(self):
        return "<SeaBreezeDevice %s:%s>" % (self.model, self.serial)


cpdef initialize():
    csb.sbapi_initialize()

cpdef shutdown():
    csb.sbapi_shutdown()


def device_list_devices():
    """list_devices()

    return a list of all connected oceanoptics devices.
    """
    # Probe Devices on all Buses
    csb.sbapi_probe_devices()

    cdef int N_devices
    N_devices = csb.sbapi_get_number_of_device_ids()
    deviceIDs = <long*> PyMem_Malloc(N_devices * sizeof(long))

    csb.sbapi_get_device_ids(deviceIDs, N_devices)

    devices = []
    tmpdevice = SeaBreezeDevice(-1, "", "")
    for i in range(N_devices):
        handle = deviceIDs[i]
        tmpdevice.handle = handle
        if device_is_open(tmpdevice):
            was_open_before = True
        else:
            was_open_before = False
            device_open(tmpdevice)
        model = device_get_model(tmpdevice)
        serial = device_get_serial_number(tmpdevice)
        if not was_open_before:
            device_close(tmpdevice)
        devices.append(SeaBreezeDevice(handle, model, serial))

    PyMem_Free(deviceIDs)
    return devices


def device_open(SeaBreezeDevice device not None):
    cdef int error_code
    cdef int ret
    ret = csb.sbapi_open_device(device.handle, &error_code)
    if int(ret) > 0:
        raise SeaBreezeError(error_code=error_code)

def device_is_open(SeaBreezeDevice device not None):
    try:
        # this is a hack to figure out if the spectrometer is connected
        device_get_serial_number(device)
    except SeaBreezeError:
        return False
    else:
        return True

def device_close(SeaBreezeDevice device not None):
    cdef int error_code
    # always returns 1
    csb.sbapi_close_device(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def device_get_model(SeaBreezeDevice device not None):
    cdef int error_code
    cdef char cbuf[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.sbapi_get_device_type(device.handle, &error_code, cbuf, SBMAXBUFLEN)
    model = cbuf[:bytes_written]
    if model == "NONE":
        raise SeaBreezeError(error_code=error_code)
    return model.decode("utf-8")

def device_get_serial_number(SeaBreezeDevice device not None):
    cdef int error_code
    cdef int N_serialnumber_features
    N_serialnumber_features = csb.sbapi_get_number_of_serial_number_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N_serialnumber_features != 1:
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d serial number features. The code expects it to have 1 and only 1. "
                "Please file a bug report including a description of your device." %
                N_serialnumber_features)
    cdef long featureID
    csb.sbapi_get_serial_number_features(device.handle, &error_code, &featureID, 1)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    cdef char cbuf[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.sbapi_get_serial_number(device.handle, featureID, &error_code, cbuf, SBMAXBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    serial = cbuf[:bytes_written]
    return serial.decode("utf-8").rstrip('\x00')


def device_get_spectrometer_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_spectrometer_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_spectrometer_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d spectrometer features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

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


def device_get_shutter_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_shutter_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_shutter_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d shutter features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

def shutter_set_shutter_open(SeaBreezeDevice device not None, long featureID, unsigned char opened):
    cdef int error_code
    csb.sbapi_shutter_set_shutter_open(device.handle, featureID, &error_code, opened)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)


def device_get_light_source_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_light_source_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_light_source_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d light source features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

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


def device_get_continuous_strobe_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_continuous_strobe_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_continuous_strobe_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d continuous strobe features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

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


def device_get_eeprom_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_eeprom_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_eeprom_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d eeprom features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

def eeprom_read_slot(SeaBreezeDevice device not None, long featureID, int slot_number):
    cdef int error_code
    cdef unsigned char cbuf[SBMAXBUFLEN]
    cdef int bytes_written
    try:
        bytes_written = csb.sbapi_eeprom_read_slot(device.handle, featureID, &error_code, slot_number, cbuf, SBMAXBUFLEN)
    except ValueError:
        raise SeaBreezeError("EEProm slot out of bounds.")
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return cbuf[:bytes_written]


def device_get_irrad_calibration_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_irrad_cal_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_irrad_cal_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d irrad cal features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

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


def device_get_tec_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_thermo_electric_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_thermo_electric_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d tec features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

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


def device_get_lamp_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_lamp_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_lamp_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d lamp features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

def lamp_set_lamp_enable(SeaBreezeDevice device not None, long featureID, unsigned char lamp_enable):
    cdef int error_code
    csb.sbapi_lamp_set_lamp_enable(device.handle, featureID, &error_code, lamp_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)


def device_get_nonlinearity_coeffs_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_nonlinearity_coeffs_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_nonlinearity_coeffs_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d nonlinearity coeffs features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

def nonlinearity_coeffs_get(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef int values_written
    cdef double[SBMAXBUFLEN] ccoeffs
    values_written = csb.sbapi_nonlinearity_coeffs_get(device.handle, featureID, &error_code, &ccoeffs[0], SBMAXBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    coeffs = [float(ccoeffs[i]) for i in range(values_written)]
    return coeffs


def device_get_stray_light_coeffs_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_stray_light_coeffs_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_stray_light_coeffs_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:  # N_specs > 1
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d stray light coeffs features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

def stray_light_coeffs_get(SeaBreezeDevice device not None, long featureID):
    cdef int error_code
    cdef int values_written
    cdef double[SBMAXDBUFLEN] ccoeffs
    values_written = csb.sbapi_stray_light_coeffs_get(device.handle, featureID, &error_code, &ccoeffs[0], SBMAXDBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    coeffs = [float(ccoeffs[i]) for i in range(values_written)]
    return coeffs


def device_get_spectrum_processing_feature_id(SeaBreezeDevice device not None):
    cdef int N
    cdef int error_code
    cdef long featureID
    N = csb.sbapi_get_number_of_spectrum_processing_features(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    if N == 0:
        return []
    elif N == 1:
        csb.sbapi_get_spectrum_processing_features(device.handle, &error_code, &featureID, 1)
        if error_code != 0:
            raise SeaBreezeError(error_code=error_code)
        return [featureID]
    else:
        raise SeaBreezeError("This should not have happened. Apparently this device has "
                "%d spectrum_processing features. The code expects it to have 0 or 1. "
                "Please file a bug report including a description of your device." % N)

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
