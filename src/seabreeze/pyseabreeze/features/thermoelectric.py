import struct

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OBPProtocol, OOIProtocol


# Definition
# ==========
#
class SeaBreezeThermoElectricFeature(SeaBreezeFeature):
    identifier = "thermo_electric"

    def read_temperature_degrees_celsius(self):
        raise NotImplementedError("implement in derived class")

    def set_temperature_setpoint_degrees_celsius(self, temperature):
        raise NotImplementedError("implement in derived class")

    def enable_tec(self, state):
        raise NotImplementedError("implement in derived class")


# OOI implementation
# ==================
#
class ThermoElectricFeatureOOI(SeaBreezeThermoElectricFeature):
    _required_protocol_cls = OOIProtocol

    def enable_tec(self, state):
        self.protocol.send(0x71, (bool(state), 0x00))

    def set_temperature_setpoint_degrees_celsius(self, temperature):
        self.protocol.send(0x73, temperature * 10)

    def read_temperature_degrees_celsius(self):
        self.protocol.send(0x72)
        ret = self.protocol.receive(timeout_ms=1000, mode="low_speed")
        return struct.unpack("<h", ret)[0] / 10.0

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

    def enable_tec(self, state):
        self.protocol.send(0x00420010, bool(state))

    def set_temperature_setpoint_degrees_celsius(self, temperature):
        self.protocol.send(0x00420011, float(temperature))

    def read_temperature_degrees_celsius(self):
        ret = self.protocol.query(0x00420004)
        return struct.unpack("<f", ret)[0]

    # def set_tec_fan_enable(self, tec_fan_enable):
    #     raise NotImplementedError
