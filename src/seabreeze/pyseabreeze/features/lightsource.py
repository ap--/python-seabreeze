from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OOIProtocol


# Definition
# ==========
# 
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeLightSourceFeature(SeaBreezeFeature):
    identifier = 'light_source'

    def get_count(self):
        raise NotImplementedError("implement in derived class")

    def has_enable(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def is_enabled(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def set_enable(self, light_source_index, enable):
        raise NotImplementedError("implement in derived class")

    def has_variable_intensity(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def get_intensity(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def set_intensity(self, light_source_index, intensity):
        raise NotImplementedError("implement in derived class")

class SeaBreezeLightSourceFeatureOOI(SeaBreezeLightSourceFeature):
    _required_protocol_cls = OOIProtocol
    _light_enabled = False # default after initialization of device

    def set_enable(self, enable=True):
        self.protocol.send(0x03, int(enable))
        self._light_enabled = bool(enable)

    def is_enabled(self):
        return self._light_enabled
