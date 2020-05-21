from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezePixelBinningFeature(SeaBreezeFeature):
    identifier = "pixel_binning"

    def set_binning_factor(self, factor):
        raise NotImplementedError("implement in derived class")

    def get_binning_factor(self):
        raise NotImplementedError("implement in derived class")

    def get_max_binning_factor(self):
        raise NotImplementedError("implement in derived class")

    binning_factor = property(get_binning_factor, set_binning_factor)
    max_binning_factor = property(get_max_binning_factor)

    def set_default_binning_factor(self, factor):
        raise NotImplementedError("implement in derived class")

    def get_default_binning_factor(self):
        raise NotImplementedError("implement in derived class")

    default_binning_factor = property(
        get_default_binning_factor, set_default_binning_factor
    )
