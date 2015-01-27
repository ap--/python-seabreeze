"""This is the cython Wrapper for the seabreeze library

Author: Andreas Poehlmann

"""
cimport cseabreeze as csb
import numpy as np
cimport numpy as cnp
cnp.import_array()

DEF SBMAXBUFLEN = 32
DEF SBMAXDPXLEN = 128  # We'll just assume that there are never more than 128 dark pixels


class SeaBreezeError(Exception):

    def __init__(self, message=None, error_code=None):
        if error_code is not None:
            message = get_error_string(error_code)
        elif message is not None:
            pass
        else:
            message = ""
        # Call the base class constructor with the parameters it needs
        super(SeaBreezeError, self).__init__(message)
        self.error_code = error_code

cdef get_error_string(int error_code):
    cdef char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_get_error_string(error_code, buffer_, SBMAXBUFLEN)
    return buffer_[:bytes_written]


class SeaBreezeDevice(object):
    def __init__(self, handle, model, serial):
        self.handle = handle
        self.model = model
        self.serial = serial

    def __repr__(self):
        return "<SeaBreezeDevice %s:%s>" % (self.model, self.serial)


def list_spectrometers():
    """list_spectrometers()

    list all available oceanoptics spectrometers and return a list of devices
    """
    # Get number of connected spectrometers
    tmpdevice = SeaBreezeDevice(None, None, None)
    idx = 0
    while True:
        try:
            tmpdevice.handle = idx
            open_spectrometer(tmpdevice)
        except SeaBreezeError:
            break
        idx += 1
    N = idx

    # Get the required info and close the spectrometers
    out = []
    for i in range(N):
        tmpdevice.handle = i
        device = SeaBreezeDevice(i, get_model(tmpdevice), get_serial_number(tmpdevice))
        out.append(device)
        close_spectrometer(tmpdevice)
    return out


def open_spectrometer(device):
    assert device.handle >= 0
    cdef int error_code
    if csb.seabreeze_open_spectrometer(device.handle, &error_code):
        raise SeaBreezeError(error_code=error_code)

def close_spectrometer(device):
    assert device.handle >= 0
    cdef int error_code
    # always returns 1
    csb.seabreeze_close_spectrometer(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def get_model(device):
    assert device.handle >= 0
    cdef int error_code
    cdef char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_get_model(device.handle, &error_code, buffer_, SBMAXBUFLEN)
    model = buffer_[:bytes_written]
    if model == "NONE":
        raise SeaBreezeError(error_code=error_code)
    return model

def set_trigger_mode(device, int mode):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_trigger_mode(device.handle, &error_code, mode)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def set_integration_time_microsec(device, unsigned long integration_time_micros):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_integration_time_microsec(device.handle, &error_code, integration_time_micros)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def get_min_integration_time_microsec(device):
    assert device.handle >= 0
    cdef long min_integration_time
    cdef int error_code
    min_integration_time = csb.seabreeze_get_min_integration_time_microsec(device.handle, &error_code)
    if min_integration_time < 0:
        raise SeaBreezeError(error_code=error_code)
    return min_integration_time

def set_shutter_open(device, bint opened):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_shutter_open(device.handle, &error_code, opened)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def set_strobe_enable(device, bint strobe_enable):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_strobe_enable(device.handle, &error_code, strobe_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def get_light_source_count(device):
    assert device.handle >= 0
    cdef int error_code
    cdef int ls_count
    ls_count = csb.seabreeze_get_light_source_count(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def set_light_source_enable(device, int light_index, bint enable):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_light_source_enable(device.handle, &error_code, light_index, enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def set_light_source_intensity(device, int light_index, double intensity):
    assert device.handle >= 0
    assert 0 <= intensity <= 1
    cdef int error_code
    csb.seabreeze_set_light_source_intensity(device.handle, &error_code, light_index, intensity)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def read_eeprom_slot(device, int slot_number):
    assert device.handle >= 0
    assert 0 <= slot_number < 16 # TODO: check actual max slot_number?
    cdef int error_code
    cdef unsigned char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_read_eeprom_slot(device.handle, &error_code, slot_number, buffer_, SBMAXBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return buffer_[:bytes_written]

def write_eeprom_slot(device, int slot_number, data):
    assert device.handle >= 0
    assert 0 <= slot_number < 16 # TODO: check actual max slot_number?
    assert isinstance(data, basestring)
    assert 0 <= len(data) < 16
    cdef int error_code
    cdef int bytes_written
    cdef int buffer_length
    buffer_length = len(data)
    cdef unsigned char* cbuffer_ = data
    bytes_written = csb.seabreeze_write_eeprom_slot(device.handle, &error_code, slot_number, cbuffer_, buffer_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bytes_written

def read_irrad_calibration(device):
    raise NotImplementedError
    # TODO:
    # def read_irrad_calibration(device, cnp.ndarray[cnp.float32_t, ndim=1, mode="c"] out):
    #     assert device.handle >= 0
    #     assert out.dtype == np.float32
    #     cdef int error_code
    #     cdef int bytes_written
    #     cdef int out_length = len(out)
    #     bytes_written = csb.seabreeze_read_irrad_calibration(device.handle, &error_code, &out[0], out_length)
    #     if error_code != 0:
    #         raise SeaBreezeError(error_code=error_code)
    #     return

def write_irrad_calibration(device, data):
    raise NotImplementedError
    # TODO:
    #    int seabreeze_write_irrad_calibration(device, int *error_code, float *buffer, int buffer_length)

def has_irrad_collection_area(device):
    assert device.handle >= 0
    cdef int error_code
    cdef int has_ca
    has_ca = csb.seabreeze_has_irrad_collection_area(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bool(has_ca)

def read_irrad_collection_area(device):
    assert device.handle >= 0
    cdef int error_code
    cdef float area
    area = csb.seabreeze_read_irrad_collection_area(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return area

def write_irrad_collection_area(device, float area):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_write_irrad_collection_area(device.handle, &error_code, area)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def read_tec_temperature(device):
    assert device.handle >= 0
    cdef int error_code
    cdef double temperature
    temperature = csb.seabreeze_read_tec_temperature(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return temperature

def set_tec_temperature(device, double temperature_degrees_celsius):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_tec_temperature(device.handle, &error_code, temperature_degrees_celsius)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def set_tec_enable(device, bint tec_enable):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_tec_enable(device.handle, &error_code, tec_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def set_tec_fan_enable(device, bint tec_fan_enable):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_tec_fan_enable(device.handle, &error_code, tec_fan_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

def get_unformatted_spectrum(device, cnp.ndarray[cnp.uint8_t, ndim=1, mode="c"] out):
    assert device.handle >= 0
    assert out.dtype == np.ubyte
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_get_unformatted_spectrum(device.handle, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return

def get_formatted_spectrum(device, cnp.ndarray[cnp.double_t, ndim=1, mode="c"] out):
    assert device.handle >= 0
    assert out.dtype == np.double
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_get_formatted_spectrum(device.handle, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return

def get_unformatted_spectrum_length(device):
    assert device.handle >= 0
    cdef int error_code
    cdef int spec_length
    spec_length = csb.seabreeze_get_unformatted_spectrum_length(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return spec_length

def get_formatted_spectrum_length(device):
    assert device.handle >= 0
    cdef int error_code
    cdef int spec_length
    spec_length = csb.seabreeze_get_formatted_spectrum_length(device.handle, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return spec_length

def get_wavelengths(device, cnp.ndarray[cnp.double_t, ndim=1, mode="c"] out):
    assert device.handle >= 0
    assert out.dtype == np.double
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_get_wavelengths(device.handle, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return

def get_serial_number(device):
    assert device.handle >= 0
    cdef int error_code
    cdef char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_get_serial_number(device.handle, &error_code, buffer_, SBMAXBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    serial = buffer_[:bytes_written]
    return serial.rstrip('\x00')

def get_electric_dark_pixel_indices(device):
    assert device.handle >= 0
    cdef int error_code
    cdef int ints_written
    cdef int ibuffer[SBMAXDPXLEN]
    cdef cnp.ndarray[cnp.int_t, mode="c"] indices
    ints_written = csb.seabreeze_get_electric_dark_pixel_indices(device.handle, &error_code, ibuffer, SBMAXDPXLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    indices = np.empty((ints_written,), dtype=np.int)
    for i in range(ints_written):
        indices[i] = ibuffer[i]
    return indices

def set_continuous_strobe_period_microsec(device, unsigned short strobe_id, unsigned long period_usec):
    assert device.handle >= 0
    cdef int error_code
    csb.seabreeze_set_continuous_strobe_period_microsec(device.handle, &error_code, strobe_id, period_usec)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

#
# XXX: not required in wrapper
# ============================
# cpdef shutdown():
#     csb.seabreeze_shutdown()
#
# int seabreeze_write_usb(device, int *error_code, unsigned char endpoint, unsigned char* buffer, unsigned int length)
# int seabreeze_read_usb(device, int *error_code, unsigned char endpoint, unsigned char* buffer, unsigned int length)
#
# cpdef get_api_version_string():
#     cdef char buffer_[SBMAXBUFLEN]
#     cdef int bytes_written
#     bytes_written = csb.seabreeze_get_api_version_string(buffer_, SBMAXBUFLEN)
#     return buffer_[:bytes_written]
#
# int seabreeze_get_usb_descriptor_string(device, int *error_code, int id, unsigned char *buffer, int len)
#
# cpdef _set_verbose(bint flag):
#     csb.seabreeze_set_verbose(flag)
#
