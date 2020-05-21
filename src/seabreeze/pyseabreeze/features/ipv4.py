from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeIPv4Feature(SeaBreezeFeature):
    identifier = "ipv4"

    def get_dhcp_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_dhcp_enable_state(self, interface_index, is_enabled):
        raise NotImplementedError("implement in derived class")

    def get_number_of_ipv4_addresses(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def get_ipv4_address(self, interface_index, address_index):
        raise NotImplementedError("implement in derived class")

    def get_default_gateway(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_default_gateway(self, interface_index, default_gateway_address):
        raise NotImplementedError("implement in derived class")

    def add_ipv4_address(self, interface_index, ipv4_address):
        raise NotImplementedError("implement in derived class")

    def delete_ipv4_address(self, interface_index, address_index):
        raise NotImplementedError("implement in derived class")
