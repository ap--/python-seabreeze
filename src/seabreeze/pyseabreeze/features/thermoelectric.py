import struct

from seabreeze.pyseabreeze.communication import USBCommOOI, USBCommOBP
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.features._interface import _SeaBreezeThermoElectricFeatureInterface


class ThermoElectricFeatureOOI(SeaBreezeFeature, _SeaBreezeThermoElectricFeatureInterface):
    required_interface_cls = USBCommOOI

    def enable_tec(self, state):
        self.device.usb_send(struct.pack('<BBB', 0x71, bool(state), 0x00))

    def set_temperature_setpoint_degrees_celsius(self, temperature):
        self.device.usb_send(struct.pack('<Bh', 0x73, int(temperature * 10)))

    def read_temperature_degrees_celsius(self):
        self.device.usb_send(struct.pack('<B', 0x72))
        ret = self.device.usb_read_lowspeed(timeout=1000)
        return struct.unpack('<h', ret)[0] / 10.0

    # NOTE: if implemented this would also need to be added to the interface
    # def set_tec_fan_enable(self, tec_fan_enable):
    #    # this is not implemented in the seabreeze library?
    #    self.usb_send(struct.pack('<BBB', 0x70, bool(tec_fan_enable), 0x00))
    #    raise NotImplementedError


class ThermoElectricFeatureOBP(SeaBreezeFeature, _SeaBreezeThermoElectricFeatureInterface):
    required_interface_cls = USBCommOBP

    def enable_tec(self, state):
        self.device.send_command(0x00420010, struct.pack("<B", bool(state)))

    def set_temperature_setpoint_degrees_celsius(self, temperature):
        self.device.send_command(0x00420011, struct.pack("<f", float(temperature)))

    def read_temperature_degrees_celsius(self):
        ret = self.device.query(0x00420004, "")
        return struct.unpack("<f", ret)[0]

    # def set_tec_fan_enable(self, tec_fan_enable):
    #     raise NotImplementedError
