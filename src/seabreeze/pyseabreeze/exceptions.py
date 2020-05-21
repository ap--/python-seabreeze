"""custom seabreeze exceptions

"""


class SeaBreezeError(Exception):
    """SeaBreezeError base class"""

    pass


class SeaBreezeNumFeaturesError(SeaBreezeError):
    pass


class SeaBreezeNotSupported(SeaBreezeError):
    pass
