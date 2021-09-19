import struct

from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeGPIOFeature(SeaBreezeFeature):
    identifier = "gpio"

    modes = dict(
        GPIO_OUTPUT_PUSH_PULL=0x00,
        GPIO_OPEN_DRAIN_OUTPUT=0x01,
        DAC_OUTPUT=0x02,
        GPIO_INPUT_HIGH_Z=0x80,
        GPIO_INPUT_PULL_DOWN=0x81,
        ADC_INPUT=0x82,
    )

    def get_number_of_gpio_pins(self):
        raise NotImplementedError("implement in derived class")

    def get_gpio_output_enable_vector(self):
        raise NotImplementedError("implement in derived class")

    def set_gpio_output_enable_vector(self, output_enable_vector, bit_mask):
        raise NotImplementedError("implement in derived class")

    def get_gpio_value_vector(self):
        raise NotImplementedError("implement in derived class")

    def set_gpio_value_vector(self, value_vector, bit_mask):
        raise NotImplementedError("implement in derived class")

    def get_number_of_egpio_pins(self):
        raise NotImplementedError("implement in derived class")

    def get_egpio_available_modes(self, pin_number):
        raise NotImplementedError("implement in derived class")

    def get_egpio_current_mode(self, pin_number):
        raise NotImplementedError("implement in derived class")

    def set_egpio_mode(self, pin_number, mode, value=0.0):
        raise NotImplementedError("implement in derived class")

    def get_egpio_output_vector_vector(self):
        raise NotImplementedError("implement in derived class")

    def set_egpio_output_vector(self, output_vector, bit_mask):
        raise NotImplementedError("implement in derived class")

    def get_egpio_value(self, pin_number):
        raise NotImplementedError("implement in derived class")

    def set_egpio_value(self, pin_number, value):
        raise NotImplementedError("implement in derived class")


class SeaBreezeGPIOFeatureOBP(SeaBreezeGPIOFeature):
    def get_number_of_gpio_pins(self):
        """output is the I/O pin count"""
        ret = self.protocol.query(0x00200000)
        return struct.unpack("B", ret)[0]

    def get_output_enable_vector(self):
        """output is the data direction definition"""
        ret = self.protocol.query(0x00200100)
        # maybe we could return a list of bool here?
        return struct.unpack("<H", ret)[0]

    def set_gpio_output_enable_vector(self, output_enable_vector, bit_mask):
        """set the data direction"""
        # we could allow lists of bool (or strings "001010101...") here too
        if not (
            isinstance(output_enable_vector, int)
            and 0 <= output_enable_vector < 2 ** 16
        ):
            raise ValueError(
                "expecting a 16bit integer representing the output enable bits"
            )
        if not (isinstance(bit_mask, int) and 0 <= bit_mask < 2 ** 16):
            raise ValueError(
                "expecting a 16bit integer representing the output enable mask"
            )
        self.protocol.send(0x00200110, payload=(output_enable_vector, bit_mask))

    def get_gpio_value_vector(self):
        """return the value vector of the gpio registers"""
        ret = self.protocol.query(0x00200300)
        # maybe we could return a list of bool here?
        return struct.unpack("<H", ret)[0]

    def set_gpio_value_vector(self, value_vector, bit_mask):
        """set the gpio value vector"""
        # we could allow lists of bool (or strings "001010101...") here too
        if not (isinstance(value_vector, int) and 0 <= value_vector < 2 ** 16):
            raise ValueError(
                "expecting a 16bit integer representing the output enable bits"
            )
        if not (isinstance(bit_mask, int) and 0 <= bit_mask < 2 ** 16):
            raise ValueError(
                "expecting a 16bit integer representing the output enable mask"
            )
        self.protocol.send(0x00200310, payload=(value_vector, bit_mask))
