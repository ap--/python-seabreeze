from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeStrobeLampFeature(SeaBreezeFeature):
    identifier = "strobe_lamp"

    def enable_lamp(self, state: bool) -> None:
        raise NotImplementedError("implement in derived class")
