"""Implementation of the USB communication layer


ceanoptics spectrometers use two different communication protocols.

One is called 'OBP protocol'  # OceanBinaryProtocol
the other one 'OOI protocol'  # ??? OceanOpticsInterface ??? maybe

"""
import warnings

import usb.core
import usb.util

from seabreeze.pyseabreeze.exceptions import SeaBreezeError


class TransportInterface(object):

    _required_init_kwargs = ()

    @classmethod
    def supports(cls, device):
        """return if the device supports the transport or vice versa

        Returns
        -------
        bool
        """
        raise NotImplementedError("implement in derived transport class")

    def open_device(self, device):
        """open a seabreeze device

        Parameters
        ----------
        device : seabreeze.pyseabreeze.devices.SeaBreezeDevice
        """
        raise NotImplementedError("implement in derived transport class")

    @property
    def is_open(self):
        """return if device is opened

        Returns
        -------
        bool
        """
        raise NotImplementedError("implement in derived transport class")

    def close_device(self):
        """close the seabreeze device"""
        raise NotImplementedError("implement in derived transport class")

    def write(self, data):
        """write data to the device"""
        raise NotImplementedError("implement in derived transport class")

    def read(self, size=None, timeout_ms=None):
        """read data from the device (in low speed mode)

        Returns
        -------
        str
        """
        raise NotImplementedError("implement in derived transport class")

    def read_high_speed(self, size=None, timeout_ms=None):
        """read data from the device in high speed mode

        Returns
        -------
        str
        """
        raise NotImplementedError("implement in derived transport class")

    def read_high_speed_alt(self, size=None, timeout_ms=None):
        """read data from the device in alternate high speed mode

        Returns
        -------
        str
        """
        raise NotImplementedError("implement in derived transport class")

    @property
    def default_timeout_ms(self):
        raise NotImplementedError("implement in derived transport class")


class TransportLayerUSB(TransportInterface):

    _required_init_kwargs = {'endpoint_map'}
    _default_read_size = {
        'low_speed': 64,
        'high_speed': 512,
        'high_speed_alt': 512
    }

    def __init__(self, endpoint_map=None):
        assert endpoint_map is not None
        self._endpoint_map = endpoint_map
        self._device = None
        self._opened = None

    @classmethod
    def supports(cls, device):
        return isinstance(device, usb.core.Device)

    def open_device(self, device):
        if not isinstance(device, usb.core.Device):
            raise TypeError("device needs to be usb.core.Device")
        # device.reset()
        self._device = device
        try:
            if self._device.is_kernel_driver_active(0):
                self._device.detach_kernel_driver(0)
        except NotImplementedError:
            pass  # unavailable on some systems/backends
        try:
            self._device.set_configuration()
        except usb.core.USBError as err:
            if err.errno == 16:
                # TODO: warn as in cseabreeze
                self._opened = True
            raise
        else:
            self._opened = True

    @property
    def is_open(self):
        return self._opened or False

    def close_device(self):
        usb.util.dispose_resources(self._device)
        self._device.reset()
        self._device = None
        self._opened = False

    def write(self, data):
        self._device.write(self._endpoint_map.ep_out, data)

    def read(self, size=None, timeout_ms=None):
        if size is None:
            size = self._default_read_size['low_speed']
        return self._device.read(self._endpoint_map.lowspeed_in, size, timeout=timeout_ms)

    def read_high_speed(self, size=None, timeout_ms=None):
        if size is None:
            size = self._default_read_size['high_speed']
        return self._device.read(self._endpoint_map.highspeed_in, size, timeout=timeout_ms)

    def read_high_speed_alt(self, size=None, timeout_ms=None):
        if size is None:
            size = self._default_read_size['high_speed_alt']
        return self._device.read(self._endpoint_map.highspeed_in2, size, timeout=timeout_ms)

    @property
    def default_timeout_ms(self):
        return self._device.default_timeout if self._device else None
