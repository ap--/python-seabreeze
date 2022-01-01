from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezePixelBinningFeature(SeaBreezeFeature):
    identifier = "pixel_binning"

    def set_binning_factor(self, factor: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_binning_factor(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_max_binning_factor(self) -> int:
        raise NotImplementedError("implement in derived class")

    binning_factor: int = property(get_binning_factor, set_binning_factor)  # type: ignore
    max_binning_factor: int = property(get_max_binning_factor)  # type: ignore

    def set_default_binning_factor(self, factor: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_default_binning_factor(self) -> int:
        raise NotImplementedError("implement in derived class")

    default_binning_factor: int = property(  # type: ignore
        get_default_binning_factor, set_default_binning_factor
    )
