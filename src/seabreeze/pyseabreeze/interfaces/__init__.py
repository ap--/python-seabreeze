from .common import SeaBreezeError
from .spectrometer import (
                            SpectrometerFeatureUSB2000,
                            SpectrometerFeatureHR2000,
                            SpectrometerFeatureHR4000,
                            SpectrometerFeatureUSB650,
                            SpectrometerFeatureHR2000PLUS,
                            SpectrometerFeatureQE65000,
                            SpectrometerFeatureUSB2000PLUS,
                            SpectrometerFeatureUSB4000,
                            SpectrometerFeatureNIRQUEST512,
                            SpectrometerFeatureNIRQUEST256,
                            SpectrometerFeatureMAYA2000PRO,
                            SpectrometerFeatureMAYA2000,
                            SpectrometerFeatureTORUS,
                            SpectrometerFeatureAPEX,
                            SpectrometerFeatureMAYALSL,
                            SpectrometerFeatureJAZ,
                            SpectrometerFeatureSTS,
                            SpectrometerFeatureQEPRO,
                            SpectrometerFeatureVENTANA,
                          )

from .wavelength import WavelengthCoefficientsEEPromFeature
from .eeprom import EEPromFeature
from .thermoelectric import ThermoElectricFeatureOOI, ThermoElectricFeatureOBP
from .nonlinearity import NonlinearityCoefficientsEEPromFeature, NonlinearityCoefficientsOBPFeature
from .spectrumprocessing import SpectrumProcessingFeatureOBP
from .defines import EndPoints
from .common import (
                      NotImplementedWrapper,
                      NoShutterFeature,
                      NoTecFeature,
                      NoEEPromFeature,
                    )


class USB2000(SpectrometerFeatureUSB2000,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['USB2000']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2) + 1
    _INTEGRATION_TIME_MIN = 3000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1000
    _MAX_PIXEL_VALUE = 4095

class HR2000(SpectrometerFeatureHR2000,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['HR2000']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2) + 1
    _INTEGRATION_TIME_MIN = 3000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1000
    _MAX_PIXEL_VALUE = 4095

class HR4000(SpectrometerFeatureHR4000,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['HR4000']
    _PIXELS = 3840  # FIXME
    _RAW_SPECTRUM_LEN = (3840 * 2) + 1
    _INTEGRATION_TIME_MIN = 10
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 16383

class USB650(SpectrometerFeatureUSB650,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['USB650']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2) + 1
    _INTEGRATION_TIME_MIN = 3000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1000
    _MAX_PIXEL_VALUE = 4095

class HR2000PLUS(SpectrometerFeatureHR2000PLUS,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['HR2000PLUS']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2) + 1
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 16383

class QE65000(SpectrometerFeatureQE65000,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             ThermoElectricFeatureOOI,
             NoShutterFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['QE65000']
    _PIXELS = 1280  # FIXME
    _RAW_SPECTRUM_LEN = (1024 + 256)*2 + 1
    _INTEGRATION_TIME_MIN = 8000
    _INTEGRATION_TIME_MAX = 1600000000
    _INTEGRATION_TIME_BASE = 1000
    _MAX_PIXEL_VALUE = 65535

class USB2000PLUS(SpectrometerFeatureUSB2000PLUS,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['USB2000PLUS']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2) + 1
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 65535

class USB4000(SpectrometerFeatureUSB4000,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['USB4000']
    _PIXELS = 3840  # FIXME
    _RAW_SPECTRUM_LEN = (3840 * 2) + 1
    _INTEGRATION_TIME_MIN = 10
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 65535

class NIRQUEST512(SpectrometerFeatureNIRQUEST512,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             ThermoElectricFeatureOOI,
             NoShutterFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['NIRQUEST512']
    _PIXELS = 512  # FIXME
    _RAW_SPECTRUM_LEN = (512 * 2) + 1
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 1600000000
    _INTEGRATION_TIME_BASE = 1000
    _MAX_PIXEL_VALUE = 65535

class NIRQUEST256(SpectrometerFeatureNIRQUEST256,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             ThermoElectricFeatureOOI,
             NoShutterFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['NIRQUEST256']
    _PIXELS = 256  # FIXME
    _RAW_SPECTRUM_LEN = (256 * 2) + 1
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 1600000000
    _INTEGRATION_TIME_BASE = 1000
    _MAX_PIXEL_VALUE = 65535

class MAYA2000PRO(SpectrometerFeatureMAYA2000PRO,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['MAYA2000PRO']
    _PIXELS = 2304  # FIXME
    _RAW_SPECTRUM_LEN = (2304 * 2) + 1
    _INTEGRATION_TIME_MIN = 7200
    _INTEGRATION_TIME_MAX = 65000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 64000

class MAYA2000(SpectrometerFeatureMAYA2000,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['MAYA2000']
    _PIXELS = 2304  # FIXME
    _RAW_SPECTRUM_LEN = (2304 * 2) + 1
    _INTEGRATION_TIME_MIN = 15000
    _INTEGRATION_TIME_MAX = 1600000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 65535

class TORUS(SpectrometerFeatureTORUS,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['TORUS']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2) + 1
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 65535

class APEX(SpectrometerFeatureAPEX,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['APEX']
    _PIXELS = 2304  # FIXME
    _RAW_SPECTRUM_LEN = (2304 * 2) + 1
    _INTEGRATION_TIME_MIN = 15000
    _INTEGRATION_TIME_MAX = 1600000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 64000

class MAYALSL(SpectrometerFeatureMAYALSL,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['MAYALSL']
    _PIXELS = 2304  # FIXME
    _RAW_SPECTRUM_LEN = (2304 * 2) + 1
    _INTEGRATION_TIME_MIN = 7200
    _INTEGRATION_TIME_MAX = 65000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 64000

class JAZ(SpectrometerFeatureJAZ,
             WavelengthCoefficientsEEPromFeature,
             NonlinearityCoefficientsEEPromFeature,
             EEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['JAZ']
    _PIXELS = 2048  # FIXME
    _RAW_SPECTRUM_LEN = (2048 * 2)  # XXX: No Sync byte!
    _INTEGRATION_TIME_MIN = 1000
    _INTEGRATION_TIME_MAX = 655350000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 65535

class STS(SpectrometerFeatureSTS,
             NonlinearityCoefficientsOBPFeature,
             SpectrumProcessingFeatureOBP,
             NoEEPromFeature,
             NoShutterFeature,
             NoTecFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['STS']
    _PIXELS = 1024  # FIXME
    _RAW_SPECTRUM_LEN = (1024 * 2)  # XXX: No Sync byte!
    _INTEGRATION_TIME_MIN = 10
    _INTEGRATION_TIME_MAX = 85000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 16383

class QEPRO(SpectrometerFeatureQEPRO,
             ThermoElectricFeatureOBP,
             NonlinearityCoefficientsOBPFeature,
             NoEEPromFeature,
             NoShutterFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['QEPRO']
    _PIXELS = 1044  # FIXME
    _RAW_SPECTRUM_LEN = (1044 * 4) + 32  # XXX: Metadata
    _INTEGRATION_TIME_MIN = 10000
    _INTEGRATION_TIME_MAX = 1600000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = (2**18)-1

class VENTANA(SpectrometerFeatureVENTANA,
             ThermoElectricFeatureOBP,
             NonlinearityCoefficientsOBPFeature,
             NoEEPromFeature,
             NoShutterFeature,
             NotImplementedWrapper):
    _ENDPOINT_MAP = EndPoints['VENTANA']
    _PIXELS = 1024  # FIXME
    _RAW_SPECTRUM_LEN = (1024 * 2)  # XXX: No Sync byte!
    _INTEGRATION_TIME_MIN = 22000
    _INTEGRATION_TIME_MAX = 60000000
    _INTEGRATION_TIME_BASE = 1
    _MAX_PIXEL_VALUE = 65535


USBInterfaces = {
    0x1002: USB2000,
    0x100a: HR2000,
    0x1012: HR4000,
    0x1014: USB650,
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
    0x1046: MAYALSL,
    0x2000: JAZ,
    0x4000: STS,
    0x4004: QEPRO,
    0x5000: VENTANA,
}

