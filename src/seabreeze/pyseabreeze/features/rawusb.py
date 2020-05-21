from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import ProtocolInterface
from seabreeze.pyseabreeze.transport import USBTransport


class SeaBreezeRawUSBBusAccessFeature(SeaBreezeFeature):
    identifier = "raw_usb_bus_access"
    _required_protocol_cls = ProtocolInterface

    @classmethod
    def supports_protocol(cls, protocol):
        return isinstance(protocol.transport, USBTransport)

    def raw_usb_write(self, data, endpoint):
        if endpoint not in {"primary_out", "secondary_out"}:
            raise ValueError("endpoint has to be one of 'primary_out', 'secondary_out'")
        if endpoint == "secondary_out":
            pass
        self.protocol.transport.write(data)

    def raw_usb_read(self, endpoint, buffer_length=None):
        if endpoint == "primary_in":
            return self.protocol.transport.read(size=buffer_length, mode="low_speed")
        elif endpoint == "secondary_in":
            return self.protocol.transport.read(size=buffer_length, mode="high_speed")
        elif endpoint == "secondary_in2":
            return self.protocol.transport.read(
                size=buffer_length, mode="high_speed_alt"
            )
        else:
            raise ValueError(
                "endpoint has to be one of 'primary_in', 'secondary_in', 'secondary_in2'"
            )
