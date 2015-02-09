
from .common import SeaBreezeError, get_pyseabreeze_decorator
from .eeprom import EEPromFeature

convert_exceptions = get_pyseabreeze_decorator('interfaces.wavelength')


class WavelengthCoefficientsEEPromFeature(EEPromFeature):

    @convert_exceptions("")
    def get_wavelength_coefficients(self):
        # The spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            coeffs.append(float(self.read_eeprom_slot(i)))
        return coeffs

