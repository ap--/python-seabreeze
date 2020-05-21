from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeLightSourceFeature(SeaBreezeFeature):
    identifier = "light_source"

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
