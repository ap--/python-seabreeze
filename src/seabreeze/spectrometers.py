"""
spectrometers core module
=========================

Author: Andreas Poehlmann
Email: andreas@poehlmann.io

"""
import numpy
import seabreeze.backends
# get the backend and add some functions/classes to this module
_lib = seabreeze.backends.get_backend()
_api = _lib.SeaBreezeAPI()

list_devices = _api.list_devices
SeaBreezeError = _lib.SeaBreezeError
SeaBreezeDevice = _lib.SeaBreezeDevice


class Spectrometer(object):

    def __init__(self, device):
        self._dev = device
        self._dev.open()

        nc = self._dev.f.nonlinearity_coefficients
        if nc is not None:
            self._nc = self._dev.f.nonlinearity_coefficients.get_nonlinearity_coefficients()
        else:
            self._nc = None

        self._dp = self._dev.f.spectrometer.get_electric_dark_pixel_indices()

    @classmethod
    def from_serial_number(cls, serial=None):
        if serial is None:  # pick first spectrometer
            for dev in list_devices():
                if not dev.is_open:
                    return cls(dev)
            else:
                raise SeaBreezeError("No unopened device found.")
        else:  # pick spectrometer with correct serial
            for dev in list_devices():
                if dev.serial == str(serial):
                    if dev.is_open:
                        raise SeaBreezeError("Device already opened.")
                    else:
                        return cls(dev)
            else:
                raise SeaBreezeError("No device attached with serial number '%s'." % serial)

    def wavelengths(self):
        return self._dev.f.spectrometer.wavelengths()

    def intensities(self, correct_dark_counts=False, correct_nonlinearity=False):
        if correct_dark_counts and not self._dp:
            raise SeaBreezeError("This device does not support dark count correction.")
        if correct_nonlinearity and not self._nc:
            raise SeaBreezeError("This device does not support nonlinearity correction.")
        # Get the intensities
        out = self._dev.f.spectrometer.get_intensities()
        # Do corrections if requested
        if correct_nonlinearity or correct_dark_counts:
            dark_offset = numpy.mean(out[self._dp]) if self._dp else 0.
            out -= dark_offset
        if correct_nonlinearity:
            out = out / numpy.polyval(self._nc, out)
        if correct_nonlinearity and (not correct_dark_counts):
            # noinspection PyUnboundLocalVariable
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
            self._dev.f.spectrometer.integration_time_micros(integration_time_micros)
        except SeaBreezeError as e:
            if getattr(e, 'error_code', None) == 1:
                # Only replace if 'Undefined Error'
                raise SeaBreezeError("FIX: Specified integration time is out of range.")
            else:
                raise e

    def trigger_mode(self, mode):
        self._dev.f.spectrometer.trigger_mode(mode)

    @property
    def serial_number(self):
        return self._dev.serial_number

    @property
    def model(self):
        return self._dev.model

    @property
    def pixels(self):
        return self._dev.f.spectrometer.pixels

    def close(self):
        self._dev.close()

    def __enter__(self):
        return self

    # noinspection PyShadowingBuiltins
    def __exit__(self, type, value, traceback):
        self.close()

    def __repr__(self):
        return "<Spectrometer %s:%s>" % (self.model, self.serial_number)
