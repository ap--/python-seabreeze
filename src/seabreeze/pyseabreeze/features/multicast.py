import struct
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OBPProtocol

# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeMulticastFeature(SeaBreezeFeature):
    identifier = "multicast"

    def get_multicast_enable_state(self, interface_index: int) -> bool:
        raise NotImplementedError("implement in derived class")

    def set_multicast_enable_state(
        self, interface_index: int, enable_state: bool
    ) -> None:
        raise NotImplementedError("implement in derived class")

    def get_multicast_group_address(self, interface_index: int) -> tuple[int, int , int, int]:
        raise NotImplementedError("implement in derived class")

    def get_multicast_group_port(self, interface_index: int) -> int:
        raise NotImplementedError("implement in derived class")

    def get_multicast_ttl(self, interface_index: int) -> int:
        raise NotImplementedError("implement in derived class")


# OBP implementation
# ==================
#
class SeaBreezeMulticastFeatureOBP(SeaBreezeMulticastFeature):
    _required_protocol_cls = OBPProtocol

    def get_multicast_enable_state(self, interface_index: int) -> bool:
        ret = self.protocol.query(0x00000A80, int(interface_index))
        return bool(struct.unpack("<B", ret))

    def set_multicast_enable_state(
        self, interface_index: int, enable_state: bool
    ) -> None:
        raise NotImplementedError("missing from HDX FW documentation")

    def get_multicast_group_address(self, interface_index: int) -> tuple[int, int, int, int]:
        ret = self.protocol.query(0x00000A81, int(interface_index))
        ip = struct.unpack("<BBBB", ret)
        return ip

    def get_multicast_group_port(self, interface_index: int) -> int:
        ret = self.protocol.query(0x00000A82, int(interface_index))
        return struct.unpack("<H", ret)

    def get_multicast_ttl(self, interface_index: int) -> int:
        ret = self.protocol.query(0x00000A83, int(interface_index))
        return struct.unpack("<B", ret)
