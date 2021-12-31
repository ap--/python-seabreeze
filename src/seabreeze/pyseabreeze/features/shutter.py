from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeShutterFeature(SeaBreezeFeature):
    identifier = "shutter"

    def set_shutter_open(self, state: bool) -> None:
        raise NotImplementedError("implement in derived class")
