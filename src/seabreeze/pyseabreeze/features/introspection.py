from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeIntrospectionFeature(SeaBreezeFeature):
    identifier = "introspection"

    def number_of_pixels(self):
        raise NotImplementedError("implement in derived class")

    def get_active_pixel_ranges(self):
        raise NotImplementedError("implement in derived class")

    def get_optical_dark_pixel_ranges(self):
        raise NotImplementedError("implement in derived class")

    def get_electric_dark_pixel_ranges(self):
        raise NotImplementedError("implement in derived class")
