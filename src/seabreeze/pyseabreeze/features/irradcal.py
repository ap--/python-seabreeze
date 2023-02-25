from __future__ import annotations

from typing import TYPE_CHECKING

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature

if TYPE_CHECKING:
    import numpy as np
    from numpy.typing import NDArray


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeIrradCalFeature(SeaBreezeFeature):
    identifier = "irrad_cal"

    def has_collection_area(self) -> bool:
        raise NotImplementedError("implement in derived class")

    def read_collection_area(self) -> float:
        raise NotImplementedError("implement in derived class")

    def write_collection_area(self, area: float) -> None:
        raise NotImplementedError("implement in derived class")

    def read_calibration(self) -> NDArray[np.int_]:
        raise NotImplementedError("implement in derived class")

    def write_calibration(self, calibration_array: NDArray[np.int_]) -> None:
        raise NotImplementedError("implement in derived class")
