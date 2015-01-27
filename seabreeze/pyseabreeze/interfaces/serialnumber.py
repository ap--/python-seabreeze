
from .common import SeaBreezeError, get_pyseabreeze_decorator
from .eeprom import EEPromFeature

convert_exceptions = get_pyseabreeze_decorator('interfaces.serialnumber')


class SerialNumberEEPromFeature(EEPromFeature):

    @convert_exceptions("Error when reading serial number.")
    def get_serial_number(self):
        # The serial is stored in slot 0
        return str(self.read_eeprom_slot(0))

