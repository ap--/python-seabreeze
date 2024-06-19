"""custom seabreeze exceptions"""

# DO NOT DIRECTLY IMPORT EXCEPTIONS FROM HERE!
# ALWAYS IMPORT FROM `seabreeze.spectrometers`
from seabreeze._exc import SeaBreezeError as SeaBreezeError

__all__ = [
    "SeaBreezeError",
    "SeaBreezeNumFeaturesError",
    "SeaBreezeNotSupported",
]


class SeaBreezeNumFeaturesError(SeaBreezeError):
    pass


class SeaBreezeNotSupported(SeaBreezeError):
    pass
