from typing import Tuple

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeIntrospectionFeature(SeaBreezeFeature):
    identifier = "introspection"

    def number_of_pixels(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_active_pixel_ranges(self) -> Tuple[Tuple[int, int], ...]:
        raise NotImplementedError("implement in derived class")

    def get_optical_dark_pixel_ranges(self) -> Tuple[Tuple[int, int], ...]:
        raise NotImplementedError("implement in derived class")

    def get_electric_dark_pixel_ranges(self) -> Tuple[Tuple[int, int], ...]:
        raise NotImplementedError("implement in derived class")
