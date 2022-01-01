import struct

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OBPProtocol
from seabreeze.pyseabreeze.protocol import OOIProtocol


# Definition
# ==========
#
class SeaBreezeThermoElectricFeature(SeaBreezeFeature):
    identifier = "thermo_electric"

    def read_temperature_degrees_celsius(self) -> float:
        raise NotImplementedError("implement in derived class")

    def set_temperature_setpoint_degrees_celsius(self, temperature: float) -> None:
        raise NotImplementedError("implement in derived class")

    def enable_tec(self, state: bool) -> None:
        raise NotImplementedError("implement in derived class")


# OOI implementation
# ==================
#
class ThermoElectricFeatureOOI(SeaBreezeThermoElectricFeature):
    _required_protocol_cls = OOIProtocol

    def enable_tec(self, state: bool) -> None:
        self.protocol.send(0x71, (bool(state), 0x00))

    def set_temperature_setpoint_degrees_celsius(self, temperature: float) -> None:
        self.protocol.send(0x73, int(temperature * 10))

    def read_temperature_degrees_celsius(self) -> float:
        self.protocol.send(0x72)
        ret = self.protocol.receive(timeout_ms=1000, mode="low_speed")
        return float(struct.unpack("<h", ret)[0] / 10.0)

    # NOTE: if implemented this would also need to be added to the interface
    # def set_tec_fan_enable(self, tec_fan_enable):
    #    # this is not implemented in the seabreeze library?
    #    self.usb_send(struct.pack('<BBB', 0x70, bool(tec_fan_enable), 0x00))
    #    raise NotImplementedError


# OBP implementation
# ==================
#
class ThermoElectricFeatureOBP(SeaBreezeThermoElectricFeature):
    _required_protocol_cls = OBPProtocol

    def enable_tec(self, state: bool) -> None:
        self.protocol.send(0x00420010, bool(state))

    def set_temperature_setpoint_degrees_celsius(self, temperature: float) -> None:
        self.protocol.send(0x00420011, float(temperature))

    def read_temperature_degrees_celsius(self) -> float:
        ret = self.protocol.query(0x00420004)
        return float(struct.unpack("<f", ret)[0])

    # def set_tec_fan_enable(self, tec_fan_enable):
    #     raise NotImplementedError
