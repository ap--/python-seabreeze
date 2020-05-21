from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeWifiConfigurationFeature(SeaBreezeFeature):
    identifier = "wifi_configuration"

    def get_wifi_mode(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_wifi_mode(self, interface_index, wifi_mode):
        raise NotImplementedError("implement in derived class")

    def get_wifi_security_type(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_wifi_security_type(self, interface_index, security_type):
        raise NotImplementedError("implement in derived class")

    def get_wifi_ssid(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_wifi_ssid(self, interface_index, ssid):
        raise NotImplementedError("implement in derived class")

    def set_wifi_pass_phrase(self, interface_index, pass_phrase):
        raise NotImplementedError("implement in derived class")
