
import seabreeze.backends
# get the backend and add some functions/classes to this module
lib = seabreeze.backends.get_backend()

list_devices = lib.device_list_devices
SeaBreezeError = lib.SeaBreezeError
SeaBreezeDevice = lib.SeaBreezeDevice

import numpy


class _HelperFeatureAdder(object):

    def __init__(self, other):
        self._other = other

    def add(self, feature):
        ffunc = getattr(lib, "device_get_%s_feature_id" % feature)
        fids = ffunc(self._other._dev)
        if fids:
            return fids[0]
        else:
            return -1 # It seems as if negative numbers are not used for featureIDs

class LightSource(object):

    def __init__(self, device, featureId, index):
        ident = device, featureId, index
        self._ident = ident
        self._has_enable = lib.light_source_has_enable(*ident)
        self._has_varint = lib.light_source_has_variable_intensity(*ident)

    def set_enable(self, enable):
        if self._has_enable:
            d, f, i = self._ident
            lib.light_source_set_enable(d, f, i, enable)
        else:
            raise SeaBreezeError("Light source #%d can't be enabled or disabled.")

    def set_intensity(self, intensity):
        if self._has_varint:
            d, f, i = self._ident
            lib.light_source_set_intensity(d, f, i, intensity)
        else:
            raise SeaBreezeError("Light source #%d intensity can't be set.")

    def get_intensity(self):
        try:
            return lib.light_source_get_intensity(*self._ident)
        except SeaBreezeError:
            raise

    def __repr__(self):
        d, f, i = self._ident
        return "<LightSource #%d at %s:%s>" % (i, d.model, d.serial)


class Spectrometer(object):

    def __init__(self, device):
        self._open_device(device)

    @classmethod
    def from_serial_number(cls, serial=None):
        if serial is None:  # pick first spectrometer
            for dev in lib.device_list_devices():
                if not lib.device_is_open(dev):
                    return cls(dev)
            else:
                raise SeaBreezeError("No unopened device found.")
        else:  # pick spectrometer with correct serial
            for dev in lib.device_list_devices():
                if dev.serial == str(serial):
                    if lib.device_is_open(dev):
                        raise SeaBreezeError("Device already opened.")
                    else:
                        return cls(dev)
            else:
                raise SeaBreezeError("No device attached with serial number '%s'." % serial)

    def _open_device(self, device):
        if not isinstance(device, SeaBreezeDevice):
            raise SeaBreezeError("Not a SeaBreezeDevice")
        if lib.device_is_open(device):
            raise SeaBreezeError("Device already opened.")
        if hasattr(self, '_dev') and lib.device_is_open(getattr(self, '_dev')):
            lib.device_close(getattr(self, '_dev'))
        self._dev = device
        lib.device_open(self._dev)
        # get default information
        self._serial = self._dev.serial
        self._model = self._dev.model
        # get features
        feature = _HelperFeatureAdder(self)
        self._fidsp = feature.add('spectrometer')
        self._fidsh = feature.add('shutter')
        self._fidls = feature.add('light_source')
        self._fidcs = feature.add('continuous_strobe')
        self._fidee = feature.add('eeprom')
        self._fidic = feature.add('irrad_calibration')
        self._fidla = feature.add('lamp')
        self._fidte = feature.add('tec')
        self._fidnc = feature.add('nonlinearity_coeffs')  # Added
        self._fidsl = feature.add('stray_light_coeffs')
        self._fidspp = feature.add('spectrum_processing') # Not implemented in pyseabreeze
        # get additional information
        self._pixels = lib.spectrometer_get_formatted_spectrum_length(self._dev, self._fidsp)
        self._minimum_integration_time_micros = (
                lib.spectrometer_get_minimum_integration_time_micros(self._dev, self._fidsp))
        # get wavelengths
        self._wavelengths = numpy.zeros((self._pixels,), dtype=numpy.double)
        transfered_N = 0
        while True:
            transfered_N += lib.spectrometer_get_wavelengths(self._dev, self._fidsp,
                                                            self._wavelengths[transfered_N:])
            if transfered_N >= self._pixels:
                break
        # get dark pixel indices
        self._dark = lib.spectrometer_get_electric_dark_pixel_indices(self._dev, self._fidsp)
        self._has_dark_pixels = True if len(self._dark) > 0 else False
        # get nonlinearity coefficients
        try:
            sbnc = lib.nonlinearity_coeffs_get(self._dev, self._fidnc)
            self._nc = numpy.poly1d(sbnc[::-1])
            self._has_nonlinearity_coeffs = True
        except SeaBreezeError:
            self._has_nonlinearity_coeffs = False
        # if lightsources
        try:
            N_light_sources = lib.light_source_get_count(self._dev, self._fidls)
            self._light_sources = tuple(LightSource(self._dev, self._fidls, i)
                                                    for i in range(N_light_sources))
        except SeaBreezeError:
            self._light_sources = tuple()

    def wavelengths(self):
        return self._wavelengths

    def intensities(self, correct_dark_counts=False, correct_nonlinearity=False):
        if correct_dark_counts and not self._has_dark_pixels:
            raise SeaBreezeError("This device does not support dark count correction.")
        if correct_nonlinearity and not self._has_nonlinearity_coeffs:
            raise SeaBreezeError("This device does not support nonlinearity correction.")
        # Get the intensities
        out = numpy.empty((self._pixels,), dtype=numpy.double)
        transfered_N = 0
        while True:
            transfered_N += lib.spectrometer_get_formatted_spectrum(self._dev, self._fidsp,
                                                                    out[transfered_N:])
            if transfered_N >= self._pixels:
                break
        # Do corrections if requested
        if (correct_nonlinearity or correct_dark_counts):
            dark_offset = numpy.mean(out[self._dark]) if self._has_dark_pixels else 0.
            out -= dark_offset
        if (correct_nonlinearity):
            out = out / numpy.polyval(self._nc, out)
        if correct_nonlinearity and (not correct_dark_counts):
            out += dark_offset
        return out

    def spectrum(self, correct_dark_counts=False, correct_nonlinearity=False):
        return numpy.vstack((self._wavelengths,
                             self.intensities(correct_dark_counts, correct_nonlinearity)))

    def integration_time_micros(self, integration_time_micros):
        # Protect against a bug in libseabreeze:
        # If integration time is out of bounds, libseabreeze returns Undefined Error
        # (Probably only for devices with a non micro second time base...)
        try:
            lib.spectrometer_set_integration_time_micros(self._dev,
                                                         self._fidsp, integration_time_micros)
        except SeaBreezeError as e:
            if getattr(e, 'error_code', None) == 1:
                # Only replace if 'Undefined Error'
                raise SeaBreezeError("FIX: Specified integration time is out of range.")
            else:
                raise e

    def trigger_mode(self, mode):
        lib.spectrometer_set_trigger_mode(self._dev, self._fidsp, mode)

    def boxcar_width(self, boxcar_width):
        lib.spectrum_processing_set_boxcar_width(self._dev, self._fidspp, boxcar_width) # Not implemented in pyseabreeze

    def scans_to_average(self, scans_to_average):
        lib.spectrum_processing_set_scans_to_average(self._dev, self._fidspp, scans_to_average) # Not implemented in pyseabreeze

    def get_boxcar_width(self):
        lib.spectrum_processing_get_boxcar_width(self._dev, self._fidspp) # Not implemented in pyseabreeze

    def get_scans_to_average(self):
        lib.spectrum_processing_get_scans_to_average(self._dev, self._fidspp) # Not implemented in pyseabreeze


    @property
    def serial_number(self):
        return self._serial

    @property
    def model(self):
        return self._model

    @property
    def pixels(self):
        return self._pixels

    @property
    def minimum_integration_time_micros(self):
        return self._minimum_integration_time_micros

    @property
    def light_sources(self):
        return self._light_sources

    def eeprom_read_slot(self, slot):
        return lib.eeprom_read_slot(self._dev, self._fidee, slot)

    def tec_set_enable(self, enable):
        lib.tec_set_enable(self._dev, self._fidte, enable)

    def tec_set_temperature_C(self, set_point_C):
        lib.tec_set_temperature_setpoint_degrees_C(self._dev, self._fidte, set_point_C)

    def tec_get_temperature_C(self):
        return lib.tec_read_temperature_degrees_C(self._dev, self._fidte)

    def lamp_set_enable(self, enable):
        lib.lamp_set_lamp_enable(self._dev, self._fidla, enable)

    def shutter_set_open(self, state):
        lib.shutter_set_shutter_open(self._dev, self._fidsh, state)

    def stray_light_coeffs(self):
        return lib.stray_light_coeffs_get(self._dev, self._fidsl)

    def irrad_calibration(self):
        out = numpy.empty((self._pixels,), dtype=numpy.float32)
        lib.irrad_calibration_read(self._dev, self._fidic, out)
        return out

    def irrad_calibration_collection_area(self):
        if lib.irrad_calibration_has_collection_area(self._dev, self._fidic):
            return lib.irrad_calibration_read_collection_area(self._dev, self._fidic)
        else:
            raise SeaBreezeError("Device does not store irrad calibration area.")

    def continuous_strobe_set_enable(self, enable):
        lib.continuous_strobe_set_enable(self._dev, self._fidcs, enable)

    def continuous_strobe_set_period_micros(self, period_micros):
        lib.continuous_strobe_set_period_micros(self._dev, self._fidcs, period_micros)

    def close(self):
        lib.device_close(self._dev)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def __repr__(self):
        return "<Spectrometer %s:%s>" % (self.model, self.serial_number)
