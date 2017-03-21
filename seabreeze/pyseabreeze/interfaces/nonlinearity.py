
from .common import SeaBreezeError, get_pyseabreeze_decorator
from .eeprom import EEPromFeature
from .communication import USBCommOBP

import struct

convert_exceptions = get_pyseabreeze_decorator('interfaces.nonlinearity')


class NonlinearityCoefficientsEEPromFeature(EEPromFeature):

    def has_nonlinearity_coeffs_feature(self):
        return [True]

    @convert_exceptions("Error when reading nonlinearity coeffs.")
    def get_nonlinearity_coeffs(self):
        # The spectrometers store the wavelength calibration in slots 6..13
        coeffs = []
        order = int(float(self.read_eeprom_slot(14)))
        for i in range(6, 6 + order + 1):
            coeffs.append(float(self.read_eeprom_slot(i)))
        return coeffs


class NonlinearityCoefficientsOBPFeature(USBCommOBP):

    def has_nonlinearity_coeffs_feature(self):
        return [True]

    @convert_exceptions("Error when reading nonlinearity coeffs.")
    def get_nonlinearity_coeffs(self):
        # get number of nonlinearity coefficients
        data = self.query(0x00181100, "")
        N = struct.unpack("<B", data)[0]
        # now query the coefficients
        coeffs = []
        for i in range(N):
            data = self.query(0x00181101, struct.pack("<B", i))
            coeffs.append(struct.unpack("<f", data)[0])
        return coeffs
