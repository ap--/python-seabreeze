from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeSpectrumProcessingFeature(SeaBreezeFeature):
    identifier = "spectrum_processing"

    def get_boxcar_width(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_boxcar_width(self, boxcar_width: int) -> None:
        raise NotImplementedError("implement in derived class")

    boxcar_width: int = property(get_boxcar_width, set_boxcar_width)  # type: ignore

    def get_scans_to_average(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_scans_to_average(self, scans_to_average: int) -> None:
        raise NotImplementedError("implement in derived class")

    scans_to_average: int = property(get_scans_to_average, set_scans_to_average)  # type: ignore
