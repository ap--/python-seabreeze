from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeAcquisitionDelayFeature(SeaBreezeFeature):
    identifier = "acquisition_delay"

    def set_delay_microseconds(self, delay_usec):
        raise NotImplementedError("implement in derived class")

    def get_delay_microseconds(self):
        raise NotImplementedError("implement in derived class")

    delay_microseconds = property(get_delay_microseconds, set_delay_microseconds)

    def get_delay_increment_microseconds(self):
        raise NotImplementedError("implement in derived class")

    def get_maximum_delay_microseconds(self):
        raise NotImplementedError("implement in derived class")

    def get_minimum_delay_microseconds(self):
        raise NotImplementedError("implement in derived class")
