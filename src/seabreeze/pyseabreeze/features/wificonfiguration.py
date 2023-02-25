from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeWifiConfigurationFeature(SeaBreezeFeature):
    identifier = "wifi_configuration"

    def get_wifi_mode(self, interface_index: int) -> int:
        raise NotImplementedError("implement in derived class")

    def set_wifi_mode(self, interface_index: int, wifi_mode: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_wifi_security_type(self, interface_index: int) -> str:
        raise NotImplementedError("implement in derived class")

    def set_wifi_security_type(self, interface_index: int, security_type: str) -> None:
        raise NotImplementedError("implement in derived class")

    def get_wifi_ssid(self, interface_index: int) -> str:
        raise NotImplementedError("implement in derived class")

    def set_wifi_ssid(self, interface_index: int, ssid: str) -> None:
        raise NotImplementedError("implement in derived class")

    def set_wifi_pass_phrase(self, interface_index: int, pass_phrase: str) -> None:
        raise NotImplementedError("implement in derived class")
