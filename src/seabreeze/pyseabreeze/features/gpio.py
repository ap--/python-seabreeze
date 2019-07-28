from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
# 
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeGPIOFeature(SeaBreezeFeature):
    identifier = 'gpio'

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



