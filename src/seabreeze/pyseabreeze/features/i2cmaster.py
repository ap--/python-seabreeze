from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeI2CMasterFeature(SeaBreezeFeature):
    identifier = "i2c_master"

    def get_number_of_buses(self) -> int:
        raise NotImplementedError("implement in derived class")

    def read_bus(
        self, bus_index: int, slave_address: int, buffer_length: int = 1024
    ) -> bytes:
        raise NotImplementedError("implement in derived class")

    def write_bus(self, bus_index: int, slave_address: int, data: bytes) -> None:
        raise NotImplementedError("implement in derived class")
