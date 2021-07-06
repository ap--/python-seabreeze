"""custom seabreeze exceptions"""
# DO NOT DIRECTLY IMPORT EXCEPTIONS FROM HERE!
# ALWAYS IMPORT FROM `seabreeze.spectrometers`


class SeaBreezeError(Exception):
    """SeaBreezeError base class"""


class SeaBreezeNumFeaturesError(SeaBreezeError):
    pass


class SeaBreezeNotSupported(SeaBreezeError):
    pass
