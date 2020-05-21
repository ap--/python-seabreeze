"""Implementation of the Seabreeze Transport layer.

Some spectrometers can support different transports (usb, network, rs232, etc.)

"""
import warnings
from functools import partial

import usb.core
import usb.util

try:
    from functools import partialmethod
except ImportError:
    # https://gist.github.com/carymrobbins/8940382
    # noinspection PyPep8Naming
    class partialmethod(partial):
        def __get__(self, instance, owner):
            if instance is None:
                return self
            args, kwargs = self.args or (), self.keywords or {}
            return partial(self.func, instance, *args, **kwargs)


class TransportInterface(object):

    _required_init_kwargs = ()

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

    @classmethod
    def list_devices(cls):
        raise NotImplementedError("implement in derived transport class")

    @classmethod
    def register_model(cls, model_name, **kwargs):
        raise NotImplementedError("implement in derived transport class")

    @classmethod
    def supported_model(cls, device):
        """return if the device supports the transport or vice versa

        Returns
        -------
        model_name : ``
        """
        return None

    @classmethod
    def specialize(cls, model_name, **kwargs):
        raise NotImplementedError("implement in derived transport class")


class USBTransport(TransportInterface):
    """implementation of the usb transport interface for spectrometers"""

    _required_init_kwargs = ("usb_product_id", "usb_endpoint_map", "usb_protocol")
    vendor_id = 0x2457
    product_ids = {}

    def __init__(self, usb_product_id, usb_endpoint_map, usb_protocol):
        super(USBTransport, self).__init__()
        self._product_id = usb_product_id
        self._endpoint_map = usb_endpoint_map
        self._protocol_cls = usb_protocol
        # internal settings
        self._default_read_size = {
            "low_speed": 64,
            "high_speed": 512,
            "high_speed_alt": 512,
        }
        self._read_endpoints = {
            "low_speed": "lowspeed_in",
            "high_speed": "highspeed_in",
            "high_speed_alt": "highspeed_in2",
        }
        self._default_read_endpoint = "low_speed"
        # internal state
        self._device = None
        self._opened = None
        self._protocol = None

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
        # This will initialize the communication protocol
        if self._opened:
            self._protocol = self._protocol_cls(self)

    @property
    def is_open(self):
        return self._opened or False

    def close_device(self):
        usb.util.dispose_resources(self._device)
        self._device.reset()
        self._device = None
        self._opened = False
        self._protocol = None

    def write(self, data, timeout_ms=None, **kwargs):
        if self._device is None:
            raise RuntimeError("device not opened")
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        return self._device.write(self._endpoint_map.ep_out, data, timeout=timeout_ms)

    def read(self, size=None, timeout_ms=None, mode=None, **kwargs):
        if self._device is None:
            raise RuntimeError("device not opened")
        mode = mode if mode is not None else self._default_read_endpoint
        endpoint = getattr(self._endpoint_map, self._read_endpoints[mode])
        if size is None:
            size = self._default_read_size[mode]
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        return self._device.read(endpoint, size, timeout=timeout_ms)

    @property
    def default_timeout_ms(self):
        return self._device.default_timeout if self._device else None

    @classmethod
    def list_devices(cls):
        """list pyusb devices for all available spectrometers

        Note: this includes spectrometers that are currently opened in other
        processes on the machine.

        Returns
        -------
        devices : list of usb.core.Device
            unique pyusb devices for each available spectrometer
        """
        # get all matching devices
        return usb.core.find(
            find_all=True,
            custom_match=lambda dev: (
                dev.idVendor == cls.vendor_id and dev.idProduct in cls.product_ids
            ),
        )

    @classmethod
    def register_model(cls, model_name, **kwargs):
        product_id = kwargs.get("usb_product_id")
        if product_id in cls.product_ids:
            raise ValueError(
                "product_id 0x{:04x} already in registry".format(product_id)
            )
        cls.product_ids[product_id] = model_name

    @classmethod
    def supported_model(cls, device):
        if not isinstance(device, usb.core.Device):
            return None
        # noinspection PyUnresolvedReferences
        return cls.product_ids[device.idProduct]

    @classmethod
    def specialize(cls, model_name, **kwargs):
        assert set(kwargs) == set(cls._required_init_kwargs)
        # usb transport register automatically on registration
        cls.register_model(model_name, **kwargs)
        specialized_class = type(
            "USBTransport{}".format(model_name),
            (cls,),
            {"__init__": partialmethod(cls.__init__, **kwargs)},
        )
        return specialized_class
