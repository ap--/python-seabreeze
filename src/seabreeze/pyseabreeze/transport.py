"""Implementation of the Seabreeze Transport layer.

Some spectrometers can support different transports (usb, network, rs232, etc.)

"""
import warnings

import usb.core
import usb.util


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

    def write(self, data, timeout_ms=None, **kwargs):
        """write data to the device"""
        raise NotImplementedError("implement in derived transport class")

    def read(self, size=None, timeout_ms=None, **kwargs):
        """read data from the

        Returns
        -------
        str
        """
        raise NotImplementedError("implement in derived transport class")

    @property
    def default_timeout_ms(self):
        raise NotImplementedError("implement in derived transport class")


class TransportLayerUSB(TransportInterface):
    """implementation of the usb transport interface for spectrometers"""

    _required_init_kwargs = ('endpoint_map',)
    _default_read_size = {
        'low_speed': 64,
        'high_speed': 512,
        'high_speed_alt': 512
    }
    _read_endpoints = {
        'low_speed': 'lowspeed_in',
        'high_speed': 'highspeed_in',
        'high_speed_alt': 'highspeed_in2'
    }
    _default_read_endpoint = 'low_speed'

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

    def write(self, data, timeout_ms=None, **kwargs):
        if self._device is None:
            raise RuntimeError("device not opened")
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        self._device.write(self._endpoint_map.ep_out, data, timeout=timeout_ms)

    def read(self, size=None, timeout_ms=None, mode=None, **kwargs):
        if self._device is None:
            raise RuntimeError("device not opened")
        mode = mode if mode is not None else self._default_read_endpoint
        endpoint = getattr(self._endpoint_map, mode)
        if size is None:
            size = self._default_read_size[mode]
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        return self._device.read(endpoint, size, timeout=timeout_ms)

    @property
    def default_timeout_ms(self):
        return self._device.default_timeout if self._device else None
