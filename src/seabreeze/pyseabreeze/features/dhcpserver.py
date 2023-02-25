from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeDHCPServerFeature(SeaBreezeFeature):
    identifier = "dhcp_server"

    def get_address(self, interface_index: int) -> str:
        raise NotImplementedError("implement in derived class")

    def set_address(self, interface_index: int, server_address: str) -> None:
        raise NotImplementedError("implement in derived class")

    def get_enable_state(self, interface_index: int) -> bool:
        raise NotImplementedError("implement in derived class")

    def set_enable_state(self, interface_index: int, enable_state: bool) -> None:
        raise NotImplementedError("implement in derived class")
