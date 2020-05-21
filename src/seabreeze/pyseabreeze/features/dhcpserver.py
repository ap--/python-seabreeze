from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeDHCPServerFeature(SeaBreezeFeature):
    identifier = "dhcp_server"

    def get_address(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_address(self, interface_index, server_address):
        raise NotImplementedError("implement in derived class")

    def get_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_enable_state(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")
