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

    def get_number_of_gpio_pins(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_gpio_output_enable_vector(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_gpio_output_enable_vector(
        self, output_enable_vector: int, bit_mask: int
    ) -> None:
        raise NotImplementedError("implement in derived class")

    def get_gpio_value_vector(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_gpio_value_vector(self, value_vector: int, bit_mask: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_number_of_egpio_pins(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_egpio_available_modes(self, pin_number: int) -> int:
        raise NotImplementedError("implement in derived class")

    def get_egpio_current_mode(self, pin_number: int) -> int:
        raise NotImplementedError("implement in derived class")

    def set_egpio_mode(self, pin_number: int, mode: int, value: float = 0.0) -> None:
        raise NotImplementedError("implement in derived class")

    def get_egpio_output_vector_vector(self) -> int:
        raise NotImplementedError("implement in derived class")

    def set_egpio_output_vector(self, output_vector: int, bit_mask: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_egpio_value(self, pin_number: int) -> float:
        raise NotImplementedError("implement in derived class")

    def set_egpio_value(self, pin_number: int, value: float) -> None:
        raise NotImplementedError("implement in derived class")
