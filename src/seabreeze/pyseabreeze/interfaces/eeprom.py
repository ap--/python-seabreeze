
from .common import SeaBreezeError, get_pyseabreeze_decorator
from .communication import USBCommOOI

import struct

convert_exceptions = get_pyseabreeze_decorator('interfaces.eeprom')


class EEPromFeature(USBCommOOI):

    def has_eeprom_feature(self):
        return [True]

    @convert_exceptions("Error during EEProm read.")
    def read_eeprom_slot(self, slot_number):
        """reads data from an eeprom slot"""
        ret = self.read_eeprom_slot_raw(slot_number)
        data = ret[2:ret[2:].index(0)+2].tostring().decode('utf-8')
        return data.rstrip('\x00')

    def write_eeprom_slot(self, slot_number, data):
        raise NotImplementedError

    def read_eeprom_slot_raw(self, slot_number):
        """reads the raw data from an eeprom slot"""
        self.usb_send(struct.pack('<BB', 0x05, int(slot_number)))
        ret = self.usb_read_lowspeed(size=17)
        if ret[0] != 0x05 or ret[1] != int(slot_number) % 0xFF:
            raise SeaBreezeError('read_eeprom_slot_raw: wrong answer: "%s"' % ret)
        else:
            return ret
