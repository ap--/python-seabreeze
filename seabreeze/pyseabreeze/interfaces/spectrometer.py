
from .common import SeaBreezeError, get_pyseabreeze_decorator
from .communication import USBCommOOI, USBCommOBP
from .defines import ModelNames, DarkPixels
from .wavelength import WavelengthCoefficientsEEPromFeature
from .serialnumber import SerialNumberEEPromFeature
from .eeprom import EEPromFeature

import struct
import numpy
import time

convert_exceptions = get_pyseabreeze_decorator('interfaces.spectrometer')


class SpectrometerFeatureOOI(WavelengthCoefficientsEEPromFeature,
                             SerialNumberEEPromFeature,
                             EEPromFeature,
                             USBCommOOI):

    _INTEGRATION_TIME_MIN = None
    _INTEGRATION_TIME_MAX = None
    _INTEGRATION_TIME_BASE = None
    _PIXELS = None

    @convert_exceptions("An error occured during opening.")
    def open_spectrometer(self, device):
        # open the usb device with USBCommBase
        self.open_device(device.handle)
        # initialize the spectrometer
        self.usb_send(struct.pack('<B', 0x01))
        time.sleep(0.1)  # wait shortly after init command

    @convert_exceptions("An error occured during closing.")
    def close_spectrometer(self):
        # close the usb device with USBCommBase
        self.close_device()

    @convert_exceptions("Is the spectrometer initialized?")
    def get_model(self):
        # return model name with lookuptable
        return ModelNames[self._device.idProduct]

    @convert_exceptions("Trigger mode error.")
    def set_trigger_mode(self, mode):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    @convert_exceptions("Integration time error.")
    def set_integration_time_microsec(self, integration_time_micros):
        if self._INTEGRATION_TIME_MIN <= integration_time_micros < self._INTEGRATION_TIME_MAX:
            itime = int(integration_time_micros / self._INTEGRATION_TIME_BASE)
            self.usb_send(struct.pack('<BI', 0x02, itime))
        else:
            raise SeaBreezeError("Integration time must be in range(%d, %d)." %
                                 (self._INTEGRATION_TIME_MIN, self._INTEGRATION_TIME_MAX))

    @convert_exceptions("Is the spectrometer initialized?")
    def get_min_integration_time_microsec(self):
        return self._INTEGRATION_TIME_MIN

    @convert_exceptions("")
    def get_unformatted_spectrum(self, out):
        raise NotImplementedError

    @convert_exceptions("")
    def get_formatted_spectrum(self, out):
        raise NotImplementedError

    @convert_exceptions("")
    def get_unformatted_spectrum_length(self):
        raise NotImplementedError

    @convert_exceptions("")
    def get_formatted_spectrum_length(self):
        return self._PIXELS

    @convert_exceptions("")
    def get_wavelengths(self, out):
        indices = numpy.arange(self._PIXELS, dtype=numpy.float64)
        wlcoeff = self.get_wavelength_coefficients()
        out[:] = sum( wl * (indices**i) for i, wl in enumerate(wlcoeff) )

    @convert_exceptions("Is the spectrometer initialized?")
    def get_electric_dark_pixel_indices(self):
        # return model name with lookuptable
        return DarkPixels[self.get_model()]
