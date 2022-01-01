from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeMulticastFeature(SeaBreezeFeature):
    identifier = "multicast"

    def get_multicast_enable_state(self, interface_index: int) -> bool:
        raise NotImplementedError("implement in derived class")

    def set_multicast_enable_state(
        self, interface_index: int, enable_state: bool
    ) -> None:
        raise NotImplementedError("implement in derived class")
