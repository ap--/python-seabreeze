from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeI2CMasterFeature(SeaBreezeFeature):
    identifier = "i2c_master"

    def get_number_of_buses(self):
        raise NotImplementedError("implement in derived class")

    def read_bus(self, bus_index, slave_address, buffer_length=1024):
        raise NotImplementedError("implement in derived class")

    def write_bus(self, bus_index, slave_address, data):
        raise NotImplementedError("implement in derived class")
