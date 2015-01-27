
import seabreeze.backends
# get the backend and add some functions/classes to this module
csb = seabreeze.backends.get_backend()
list_spectrometers = csb.list_spectrometers
SeaBreezeError = csb.SeaBreezeError
SeaBreezeDevice = csb.SeaBreezeDevice

import numpy as np

class Spectrometer(object):

    def __init__(self, device):
        if not isinstance(device, SeaBreezeDevice):
            raise SeaBreezeError("Not a SeaBreezeDevice")
        self._dev = device
        self.open()

    def open(self):
        csb.open_spectrometer(self._dev)
        # get default information
        self._serial = csb.get_serial_number(self._dev)
        self._model = csb.get_model(self._dev)
        self._pixels = csb.get_formatted_spectrum_length(self._dev)
        self._minimum_integration_time_us = csb.get_min_integration_time_microsec(self._dev)
        # get wavelengths
        self._wavelengths = np.zeros((self._pixels,), dtype=np.double)
        csb.get_wavelengths(self._dev, self._wavelengths)
        # get dark pixel indices
        self._dark = csb.get_electric_dark_pixel_indices(self._dev)

    def wavelengths(self):
        return self._wavelengths

    def intensities(self):
        out = np.empty((self._pixels,), dtype=np.double)
        csb.get_formatted_spectrum(self._dev, out)
        return out

    def spectrum(self):
        return np.vstack((self._wavelengths, self.intensities()))

    def integration_time_microsec(self, integration_time):
        csb.set_integration_time_microsec(self._dev, integration_time)

    @property
    def serial(self):
        return self._serial

    @property
    def model(self):
        return self._model

    @property
    def pixels(self):
        return self._pixels

    @property
    def minimum_integration_time_us(self):
        return self._minimum_integration_time_us

    def close(self):
        csb.close_spectrometer(self._dev)

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def __repr__(self):
        return "<Spectrometer %s:%s>" % (self.model, self.serial)

