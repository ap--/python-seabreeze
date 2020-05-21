from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OOIProtocol


# Definition
# ==========
class SeaBreezeContinuousStrobeFeature(SeaBreezeFeature):
    identifier = 'continuous_strobe'

    def set_enable(self, strobe_enable):
        raise NotImplementedError("implement in derived class")

    def set_period_micros(self, period_micros):
        raise NotImplementedError("implement in derived class")


class SeaBreezeContinuousStrobeFeatureOOI(SeaBreezeContinuousStrobeFeature):
    _required_protocol_cls = OOIProtocol

    def set_enable(self, strobe_enable):
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
