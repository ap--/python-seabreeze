from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeEthernetConfigurationFeature(SeaBreezeFeature):
    identifier = "ethernet_configuration"

    def get_mac_address(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_mac_address(self, interface_index, mac_address):
        raise NotImplementedError("implement in derived class")

    def get_gbe_enable_status(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_gbe_enable_status(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")
