from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeIrradCalFeature(SeaBreezeFeature):
    identifier = "irrad_cal"

    def has_collection_area(self):
        raise NotImplementedError("implement in derived class")

    def read_collection_area(self):
        raise NotImplementedError("implement in derived class")

    def write_collection_area(self, area):
        raise NotImplementedError("implement in derived class")

    def read_calibration(self):
        raise NotImplementedError("implement in derived class")

    def write_calibration(self, calibration_array):
        raise NotImplementedError("implement in derived class")
