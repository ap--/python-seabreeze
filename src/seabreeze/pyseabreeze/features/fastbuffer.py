from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeFastBufferFeature(SeaBreezeFeature):
    identifier = "fast_buffer"

    def get_buffering_enable(self) -> bool:
        raise NotImplementedError("implement in derived class")

    def set_buffering_enable(self, is_enabled: bool) -> None:
        raise NotImplementedError("implement in derived class")

    def get_consecutive_sample_count(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_consecutive_sample_count(self, consecutive_sample_count: int) -> None:
        raise NotImplementedError("implement in derived class")
