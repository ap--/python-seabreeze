from seabreeze.pyseabreeze.communication import USBCommBase
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


class SeaBreezeRawUSBAccessFeature(SeaBreezeFeature):
    identifier = 'raw_usb_bus_access'
    required_interface_cls = USBCommBase

    def raw_usb_write(self, data, endpoint):
        if endpoint not in {'primary_out', 'secondary_out'}:
            raise ValueError("endpoint has to be one of 'primary_out', 'secondary_out'")
        self.device.usb_send(data)

    def raw_usb_read(self, endpoint, buffer_length=None):
        if endpoint == 'primary_in':
            return self.device.usb_read_lowspeed(size=buffer_length or 64)
        elif endpoint == 'secondary_in':
            return self.device.usb_read_highspeed(size=buffer_length or 512)
        elif endpoint == 'secondary_in2':
            return self.device.usb_read_highspeed_alt(size=buffer_length or 512)
        else:
            raise ValueError("endpoint has to be one of 'primary_in', 'secondary_in', 'secondary_in2'")
