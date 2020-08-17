import struct
import time
import enum


class _FPGARegisterFeatureOOI(object):
    """extract fpga register feature

    (has no equivalent in cseabreeze)
    """

    class Codes(enum.IntEnum):
        FIRMWARE_VERSION = 0x04
        V1_CONTINUOUS_STROBE_TIMER_INTERVAL_DIVISOR = (
            V3_CONTINUOUS_STROBE_TIMER_MSB
        ) = 0x08
        V1_CONTINUOUS_STROBE_BASE_CLOCK_DIVISOR = V3_CONTINUOUS_STROBE_TIMER_LSB = 0x0C

    def __init__(self, protocol):
        self.protocol = protocol

    def read_register(self, register):
        fw_raw = self.protocol.query(0x6B, register)
        command, data = struct.unpack("<BH", fw_raw)
        assert command in {
            self.Codes.FIRMWARE_VERSION,  # as specified in datasheets
            0xFE  # FlameS seems to return this?
        }, str(list(map(hex, fw_raw)))
        return data

    def write_register(self, register, data):
        self.protocol.send(0x6A, payload=(register, data))
        time.sleep(0.0001)  # guarantee 100us sleep between commands

    def get_firmware_version(self):
        data = self.read_register(self.Codes.FIRMWARE_VERSION)
        fw_version = (
            (data >> 12) & 0x0F,  # major
            (data >> 4) & 0xFF,  # minor
            data & 0xF,  # patch
        )
        return fw_version
