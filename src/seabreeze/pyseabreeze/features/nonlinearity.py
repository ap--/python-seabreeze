import struct
from typing import List

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.features.eeprom import SeaBreezeEEPromFeatureOOI
from seabreeze.pyseabreeze.protocol import OBPProtocol
from seabreeze.pyseabreeze.protocol import OOIProtocol


# Definition
# ==========
#
class SeaBreezeNonlinearityCoefficientsFeature(SeaBreezeFeature):
    identifier = "nonlinearity_coefficients"

    def get_nonlinearity_coefficients(self) -> List[float]:
        raise NotImplementedError("implement in derived class")


# OOI implementation
# ==================
#
class NonlinearityCoefficientsEEPromFeatureOOI(
    SeaBreezeNonlinearityCoefficientsFeature
):
    _required_protocol_cls = OOIProtocol
    _required_features = ("eeprom",)

    def get_nonlinearity_coefficients(self) -> List[float]:
        # The spectrometers store the wavelength calibration in slots 6..13
        coeffs = []
        # noinspection PyProtectedMember
        order = int(
            float(SeaBreezeEEPromFeatureOOI._func_eeprom_read_slot(self.protocol, 14))
        )
        for i in range(6, 6 + order + 1):
            # noinspection PyProtectedMember
            coeffs.append(
                float(
                    SeaBreezeEEPromFeatureOOI._func_eeprom_read_slot(self.protocol, i)
                )
            )
        return coeffs


# OBP implementation
# ==================
#
class NonlinearityCoefficientsFeatureOBP(SeaBreezeNonlinearityCoefficientsFeature):
    _required_protocol_cls = OBPProtocol

    def get_nonlinearity_coefficients(self) -> List[float]:
        # get number of nonlinearity coefficients
        data = self.protocol.query(0x00181100)
        N = struct.unpack("<B", data)[0]
        # now query the coefficients
        coeffs = []
        for i in range(N):
            data = self.protocol.query(0x00181101, i)
            coeffs.append(struct.unpack("<f", data)[0])
        return coeffs
