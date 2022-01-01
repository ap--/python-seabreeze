from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeOpticalBenchFeature(SeaBreezeFeature):
    identifier = "optical_bench"

    def get_fiber_diameter_microns(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_slit_width_microns(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_id(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_serial_number(self) -> str:
        raise NotImplementedError("implement in derived class")

    def get_coating(self) -> str:
        raise NotImplementedError("implement in derived class")

    def get_filter(self) -> str:
        raise NotImplementedError("implement in derived class")

    def get_grating(self) -> str:
        raise NotImplementedError("implement in derived class")
