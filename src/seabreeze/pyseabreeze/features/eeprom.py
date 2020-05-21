from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OOIProtocol


# Definition
# ==========
#
class SeaBreezeEEPROMFeature(SeaBreezeFeature):
    identifier = "eeprom"

    def eeprom_read_slot(self, slot_number, strip_zero_bytes=False):
        raise NotImplementedError("implement in derived class")


# OOI spectrometer implementation
# ===============================
#
class SeaBreezeEEPromFeatureOOI(SeaBreezeEEPROMFeature):
    _required_protocol_cls = OOIProtocol

    def eeprom_read_slot(self, slot_number, strip_zero_bytes=False):
        return self._func_eeprom_read_slot(
            self.protocol, slot_number, strip_zero_bytes=strip_zero_bytes
        )

    @staticmethod
    def _func_eeprom_read_slot(
        protocol, slot_number, strip_zero_bytes=False, raw=False
    ):
        protocol.send(0x05, slot_number)
        ret = protocol.receive(size=17, mode="low_speed")
        if ret[0] != 0x05 or ret[1] != int(slot_number) % 0xFF:
            raise SeaBreezeError('read_eeprom_slot_raw: wrong answer: "%s"' % ret)
        if raw:
            return ret
        try:
            data = ret[2 : ret[2:].index(0) + 2].tobytes().decode("utf-8")
        except AttributeError:  # Python27
            data = ret[2 : ret[2:].index(0) + 2].tostring().decode("utf-8")
        if not strip_zero_bytes:
            return data
        return data.rstrip("\x00")
