from typing import List

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeTemperatureFeature(SeaBreezeFeature):
    identifier = "temperature"

    def count_temperatures(self) -> int:
        raise NotImplementedError("implement in derived class")

    def read_temperature(self, index: int) -> float:
        raise NotImplementedError("implement in derived class")

    def temperature_get_all(self) -> List[float]:
        raise NotImplementedError("implement in derived class")
