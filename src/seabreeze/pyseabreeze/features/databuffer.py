from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeDataBufferFeature(SeaBreezeFeature):
    identifier = "data_buffer"

    def clear(self) -> None:
        raise NotImplementedError("implement in derived class")

    def remove_oldest_spectra(self, number_of_spectra: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_number_of_elements(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_buffer_capacity(self, capacity: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity_maximum(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity_minimum(self) -> int:
        raise NotImplementedError("implement in derived class")
