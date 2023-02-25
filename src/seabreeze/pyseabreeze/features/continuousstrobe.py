import math
from typing import Any

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.features.fpga import _FPGARegisterFeatureOOI
from seabreeze.pyseabreeze.protocol import OOIProtocol
from seabreeze.pyseabreeze.types import PySeaBreezeProtocol

# Definition
# ==========


class SeaBreezeContinuousStrobeFeature(SeaBreezeFeature):
    identifier = "continuous_strobe"

    def set_enable(self, strobe_enable: bool) -> None:
        raise NotImplementedError("implement in derived class")

    def set_period_micros(self, period_micros: int) -> None:
        raise NotImplementedError("implement in derived class")


class SeaBreezeContinuousStrobeFeatureOOI(SeaBreezeContinuousStrobeFeature):
    _required_protocol_cls = OOIProtocol

    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        super().__init__(protocol, feature_id, **kwargs)
        self._fpga = _FPGARegisterFeatureOOI(protocol)

    def set_enable(self, strobe_enable: bool) -> None:
        """
        Sets the Lamp Enable line (J2 pin 4) as follows.
        The Single Strobe and Continuous Strobe signals are enabled/disabled by this Lamp Enable Signal.

        Parameters
        ----------
        strobe_enable: `bool`
            False: Lamp Enable Low/Off
            True: Lamp Enable HIGH/On

        Returns
        -------
        None
        """
        self.protocol.send(0x03, int(strobe_enable))

    def set_period_micros(self, period_micros: int) -> None:
        """set continuous strobe period in microseconds

        Parameters
        ----------
        period_micros : `int`
            period in microseconds 0 < period_micros <~ 60 seconds

        Returns
        -------
        None
        """
        period_micros = int(period_micros)
        if period_micros <= 0:
            raise ValueError("requires period_micros > 0")

        fpga_major_version = self._fpga.get_firmware_version()[0]

        # ported from cseabreeze
        if fpga_major_version == 1:
            # The base clock value is 48Mhz, so divide out the 48 leaving a 1 usec resolution
            # Compute how many bits are needed to represent the entire amount.
            # The first 10 will be absorbed by timerValue, and up to 16 more by baseClockValue.
            # If more than 26 bits (64 seconds) are given, it is too large.
            bits = int(math.ceil(math.log(period_micros, 2)))

            if bits <= 16:  # 0-~1023 usec
                timer_interval = 48  # use 5.6 bits (leaving about 10)
                base_clock = period_micros - 1

            elif bits <= 26:  # up to about 64 seconds
                timer_interval = 48000  # = 2^(15.55), about 1ms (new minimum step size)
                base_clock = int(period_micros / 1000) - 1

            else:
                raise ValueError("period_micros is too large")

            self._fpga.write_register(
                self._fpga.Codes.V1_CONTINUOUS_STROBE_TIMER_INTERVAL_DIVISOR,
                timer_interval,
            )
            self._fpga.write_register(
                self._fpga.Codes.V1_CONTINUOUS_STROBE_BASE_CLOCK_DIVISOR, base_clock
            )

        elif fpga_major_version == 3:
            counts = period_micros * 48
            if not 0 < counts < 2**32:
                raise ValueError("period_micros is too large")
            self._fpga.write_register(
                self._fpga.Codes.V3_CONTINUOUS_STROBE_TIMER_MSB, (counts >> 16) & 0xFFFF
            )
            self._fpga.write_register(
                self._fpga.Codes.V3_CONTINUOUS_STROBE_TIMER_LSB, counts & 0xFFFF
            )

        else:
            raise RuntimeError("unsupported FPGA major version")
