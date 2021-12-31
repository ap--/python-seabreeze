from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeEthernetConfigurationFeature(SeaBreezeFeature):
    identifier = "ethernet_configuration"

    def get_mac_address(self, interface_index: int) -> str:
        raise NotImplementedError("implement in derived class")

    def set_mac_address(self, interface_index: int, mac_address: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_gbe_enable_status(self, interface_index: int) -> bool:
        raise NotImplementedError("implement in derived class")

    def set_gbe_enable_status(self, interface_index: int, enable_state: bool) -> None:
        raise NotImplementedError("implement in derived class")
