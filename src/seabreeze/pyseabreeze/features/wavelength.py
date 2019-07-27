'''

from seabreeze.pyseabreeze.features._interface import _S
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.communication import USBCommOOI

import struct


class WavelengthCoefficientsEEPromFeature(SeaBreezeFeature, ):

    def get_wavelength_coefficients(self):
        # The spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            coeffs.append(float(self.read_eeprom_slot(i)))
        return coeffs

'''
