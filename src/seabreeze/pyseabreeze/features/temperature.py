from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeTemperatureFeature(SeaBreezeFeature):
    identifier = "temperature"

    def count_temperatures(self):
        raise NotImplementedError("implement in derived class")

    def read_temperature(self, index):
        raise NotImplementedError("implement in derived class")

    def temperature_get_all(self):
        raise NotImplementedError("implement in derived class")
