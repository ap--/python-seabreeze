from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeDataBufferFeature(SeaBreezeFeature):
    identifier = "data_buffer"

    def clear(self):
        raise NotImplementedError("implement in derived class")

    def remove_oldest_spectra(self, number_of_spectra):
        raise NotImplementedError("implement in derived class")

    def get_number_of_elements(self):
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity(self):
        raise NotImplementedError("implement in derived class")

    def set_buffer_capacity(self, capacity):
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity_maximum(self):
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity_minimum(self):
        raise NotImplementedError("implement in derived class")
