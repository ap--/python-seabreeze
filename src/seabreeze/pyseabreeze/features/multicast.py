from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeMulticastFeature(SeaBreezeFeature):
    identifier = "multicast"

    def get_multicast_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_multicast_enable_state(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")
