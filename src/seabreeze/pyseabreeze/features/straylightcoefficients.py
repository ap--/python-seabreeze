from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeStrayLightCoefficientsFeature(SeaBreezeFeature):
    identifier = "stray_light_coefficients"

    def get_stray_light_coefficients(self):
        raise NotImplementedError("implement in derived class")
