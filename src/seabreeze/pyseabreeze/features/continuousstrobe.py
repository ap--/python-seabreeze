from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
# 
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeContinuousStrobeFeature(SeaBreezeFeature):
    identifier = 'continuous_strobe'

    def set_enable(self, strobe_enable):
        raise NotImplementedError("implement in derived class")

    def set_period_micros(self, period_micros):
        raise NotImplementedError("implement in derived class")
