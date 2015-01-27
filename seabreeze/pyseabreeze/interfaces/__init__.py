
from .common import SeaBreezeError, NotImplementedWrapper
from .spectrometer import SpectrometerFeatureOOI
from .wavelength import WavelengthCoefficientsEEPromFeature
from .eeprom import EEPromFeature
from .defines import EndPoints

class USB2000(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class HR2000(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class HR4000(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class HR2000PLUS(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class QE65000(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class USB2000PLUS(SpectrometerFeatureOOI,
              WavelengthCoefficientsEEPromFeature,
              EEPromFeature,
              NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['USB2000PLUS']
    _PIXELS = 2048
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _EEPROM_SLOT_LENGTH = 17

class USB4000(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class NIRQUEST512(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class NIRQUEST256(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class MAYA2000PRO(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class MAYA2000(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class TORUS(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class APEX(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class JAZ(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass

class STS(NotImplementedWrapper):
    pass

class QEPRO(NotImplementedWrapper):
    pass

class VENTANA(NotImplementedWrapper):
    pass

class USB650(SpectrometerFeatureOOI, NotImplementedWrapper):
    pass


USBInterfaces = {
    0x1002: USB2000,
    0x100a: HR2000,
    0x1012: HR4000,
    0x1016: HR2000PLUS,
    0x1018: QE65000,
    0x101E: USB2000PLUS,
    0x1022: USB4000,
    0x1026: NIRQUEST512,
    0x1028: NIRQUEST256,
    0x102a: MAYA2000PRO,
    0x102c: MAYA2000,
    0x1040: TORUS,
    0x1044: APEX,
    0x2000: JAZ,
    0x4000: STS,
    0x4004: QEPRO,
    0x5000: VENTANA,
    0x1014: USB650,
}


