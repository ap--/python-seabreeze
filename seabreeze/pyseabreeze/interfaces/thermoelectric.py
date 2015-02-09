from .common import SeaBreezeError, get_pyseabreeze_decorator
from .communication import USBCommOOI, USBCommOBP

import struct

convert_exceptions = get_pyseabreeze_decorator('interfaces.thermoelectric')


class ThermoElectricFeatureOOI(USBCommOOI):

    def has_tec_feature(self):
        return [True]

    @convert_exceptions("Error tec temperature read.")
    def read_tec_temperature(self):
        self.usb_send(struct.pack('<B', 0x72))
        ret = self.usb_read_lowspeed(timeout=1000)
        return struct.unpack('<h', ret)[0] / 10.0

    @convert_exceptions("Error set tec temperature.")
    def set_tec_temperature(self, temperature_degrees_celsius):
        self.usb_send(struct.pack('<Bh', 0x73, int(temperature_degrees_celsius * 10)))

    @convert_exceptions("Error set tec enable.")
    def set_tec_enable(self, tec_enable):
        self.usb_send(struct.pack('<BBB', 0x71, bool(tec_enable), 0x00))

    @convert_exceptions("Error set tec fan enable.")
    def set_tec_fan_enable(self, tec_fan_enable):
        # this is not implemented in the seabreeze library?
        # self.usb_send(struct.pack('<BBB', 0x70, bool(tec_fan_enable), 0x00))
        raise NotImplementedError


class ThermoElectricFeatureOBP(USBCommOBP):

    def has_tec_feature(self):
        return [True]

    @convert_exceptions("Error tec temperature read.")
    def read_tec_temperature(self):
        ret = self.query(0x00420004, "")
        return struct.unpack("<f", ret)[0]

    @convert_exceptions("Error set tec temperature.")
    def set_tec_temperature(self, temperature_degrees_celsius):
        #self.usb_send(struct.pack('<Bh', 0x73, int(temperature_degrees_celsius * 10)))
        self.send_command(0x00420011, struct.pack("<f", float(temperature_degrees_celsius)))

    @convert_exceptions("Error set tec enable.")
    def set_tec_enable(self, tec_enable):
        self.send_command(0x00420010, struct.pack("<B", bool(tec_enable)))

    @convert_exceptions("Error set tec fan enable.")
    def set_tec_fan_enable(self, tec_fan_enable):
        raise NotImplementedError

