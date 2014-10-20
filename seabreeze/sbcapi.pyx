
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

cpdef open_spectrometer(int index):
    assert index >= 0
    cdef int error_code
    if csb.seabreeze_open_spectrometer(index, &error_code):
        raise SeaBreezeError(error_code=error_code)

cpdef close_spectrometer(int index):
    assert index >= 0
    cdef int error_code
    # always returns 1
    csb.seabreeze_close_spectrometer(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef get_model(int index):
    assert index >= 0
    cdef int error_code
    cdef char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_get_model(index, &error_code, buffer_, SBMAXBUFLEN)
    model = buffer_[:bytes_written]
    if model == "NONE":
        raise SeaBreezeError(error_code=error_code)
    return model

cpdef set_trigger_mode(int index, int mode):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_trigger_mode(index, &error_code, mode)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef set_integration_time_microsec(int index, unsigned long integration_time_micros):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_integration_time_microsec(index, &error_code, integration_time_micros)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef get_min_integration_time_microsec(int index):
    assert index >= 0
    cdef long min_integration_time
    cdef int error_code
    min_integration_time = csb.seabreeze_get_min_integration_time_microsec(index, &error_code)
    if min_integration_time < 0:
        raise SeaBreezeError(error_code=error_code)
    return min_integration_time

cpdef set_shutter_open(int index, bint opened):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_shutter_open(index, &error_code, opened)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef set_strobe_enable(int index, bint strobe_enable):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_strobe_enable(index, &error_code, strobe_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef get_light_source_count(int index):
    assert index >= 0
    cdef int error_code
    cdef int ls_count
    ls_count = csb.seabreeze_get_light_source_count(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef set_light_source_enable(int index, int light_index, bint enable):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_light_source_enable(index, &error_code, light_index, enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef set_light_source_intensity(int index, int light_index, double intensity):
    assert index >= 0
    assert 0 <= intensity <= 1
    cdef int error_code
    csb.seabreeze_set_light_source_intensity(index, &error_code, light_index, intensity)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef read_eeprom_slot(int index, int slot_number):
    assert index >= 0
    assert 0 <= slot_number < 16 # TODO: check actual max slot_number?
    cdef int error_code
    cdef unsigned char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_read_eeprom_slot(index, &error_code, slot_number, buffer_, SBMAXBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return buffer_[:bytes_written]

cpdef _write_eeprom_slot(int index, int slot_number, buffer_):
    assert index >= 0
    assert 0 <= slot_number < 16 # TODO: check actual max slot_number?
    assert isinstance(buffer_, basestring)
    assert 0 <= len(buffer_) < 16
    cdef int error_code
    cdef int bytes_written
    cdef int buffer_length
    buffer_length = len(buffer_)
    cdef unsigned char* cbuffer_ = buffer_
    bytes_written = csb.seabreeze_write_eeprom_slot(index, &error_code, slot_number, cbuffer_, buffer_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bytes_written

cpdef read_irrad_calibration(int index, cnp.ndarray[cnp.float32_t, ndim=1, mode="c"] out):
    assert index >= 0
    assert out.dtype == np.float32
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_read_irrad_calibration(index, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return


# TODO:
#    int seabreeze_write_irrad_calibration(int index, int *error_code, float *buffer, int buffer_length)

cpdef has_irrad_collection_area(int index):
    assert index >= 0
    cdef int error_code
    cdef int has_ca
    has_ca = csb.seabreeze_has_irrad_collection_area(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return bool(has_ca)

cpdef read_irrad_collection_area(int index):
    assert index >= 0
    cdef int error_code
    cdef float area
    area = csb.seabreeze_read_irrad_collection_area(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return area

cpdef _write_irrad_collection_area(int index, float area):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_write_irrad_collection_area(index, &error_code, area)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef read_tec_temperature(int index):
    assert index >= 0
    cdef int error_code
    cdef double temperature
    temperature = csb.seabreeze_read_tec_temperature(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return temperature

cpdef set_tec_temperature(int index, double temperature_degrees_celsius):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_tec_temperature(index, &error_code, temperature_degrees_celsius)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef set_tec_enable(int index, bint tec_enable):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_tec_enable(index, &error_code, tec_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef set_tec_fan_enable(int index, bint tec_fan_enable):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_tec_fan_enable(index, &error_code, tec_fan_enable)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef get_unformatted_spectrum(int index, cnp.ndarray[cnp.uint8_t, ndim=1, mode="c"] out):
    assert index >= 0
    assert out.dtype == np.ubyte
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_get_unformatted_spectrum(index, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return

cpdef get_formatted_spectrum(int index, cnp.ndarray[cnp.double_t, ndim=1, mode="c"] out):
    assert index >= 0
    assert out.dtype == np.double
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_get_formatted_spectrum(index, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return

cpdef get_unformatted_spectrum_length(int index):
    assert index >= 0
    cdef int error_code
    cdef int spec_length
    spec_length = csb.seabreeze_get_unformatted_spectrum_length(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return spec_length

cpdef get_formatted_spectrum_length(int index):
    assert index >= 0
    cdef int error_code
    cdef int spec_length
    spec_length = csb.seabreeze_get_formatted_spectrum_length(index, &error_code)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return spec_length

cpdef get_wavelengths(int index, cnp.ndarray[cnp.double_t, ndim=1, mode="c"] out):
    assert index >= 0
    assert out.dtype == np.double
    cdef int error_code
    cdef int bytes_written
    cdef int out_length = len(out)
    bytes_written = csb.seabreeze_get_wavelengths(index, &error_code, &out[0], out_length)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    return

cpdef get_serial_number(int index):
    assert index >= 0
    cdef int error_code
    cdef char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_get_serial_number(index, &error_code, buffer_, SBMAXBUFLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    serial = buffer_[:bytes_written]
    return serial.rstrip('\x00')

cpdef get_electric_dark_pixel_indices(int index):
    assert index >= 0
    cdef int error_code
    cdef int ints_written
    cdef int ibuffer[SBMAXDPXLEN]
    cdef cnp.ndarray[cnp.int_t, mode="c"] indices
    ints_written = csb.seabreeze_get_electric_dark_pixel_indices(index, &error_code, ibuffer, SBMAXDPXLEN)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)
    indices = np.empty((ints_written,), dtype=np.int)
    for i in range(ints_written):
        indices[i] = ibuffer[i]
    return indices

cpdef shutdown():
    csb.seabreeze_shutdown()


# TODO:
#    int seabreeze_write_usb(int index, int *error_code, unsigned char endpoint, unsigned char* buffer, unsigned int length)
#    int seabreeze_read_usb(int index, int *error_code, unsigned char endpoint, unsigned char* buffer, unsigned int length)

cpdef get_api_version_string():
    cdef char buffer_[SBMAXBUFLEN]
    cdef int bytes_written
    bytes_written = csb.seabreeze_get_api_version_string(buffer_, SBMAXBUFLEN)
    return buffer_[:bytes_written]


# TODO:
#    int seabreeze_get_usb_descriptor_string(int index, int *error_code, int id, unsigned char *buffer, int len)

cpdef set_continuous_strobe_period_microsec(int index, unsigned short strobe_id, unsigned long period_usec):
    assert index >= 0
    cdef int error_code
    csb.seabreeze_set_continuous_strobe_period_microsec(index, &error_code, strobe_id, period_usec)
    if error_code != 0:
        raise SeaBreezeError(error_code=error_code)

cpdef _set_verbose(bint flag):
    csb.seabreeze_set_verbose(flag)
