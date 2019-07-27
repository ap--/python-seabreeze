import struct

from seabreeze.pyseabreeze.communication import USBCommOOI
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.features._interface import _SeaBreezeEEPROMFeatureInterface


class SeaBreezeEEPromFeature(SeaBreezeFeature, _SeaBreezeEEPROMFeatureInterface):
    identifier = "eeprom"
    requires_interface_cls = USBCommOOI

    def eeprom_read_slot(self, slot_number, strip_zero_bytes=False, raw=False):
        self.device.usb_send(struct.pack('<BB', 0x05, int(slot_number)))
        ret = self.device.usb_read_lowspeed(size=17)
        if ret[0] != 0x05 or ret[1] != int(slot_number) % 0xFF:
            raise SeaBreezeError('read_eeprom_slot_raw: wrong answer: "%s"' % ret)
        if raw:
            return ret
        data = ret[2:ret[2:].index(0) + 2].tostring().decode('utf-8')
        if not strip_zero_bytes:
            return data
        return data.rstrip('\x00')
