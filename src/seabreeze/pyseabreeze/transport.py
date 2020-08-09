"""Implementation of the Seabreeze Transport layer.

Some spectrometers can support different transports (usb, network, rs232, etc.)

"""
import importlib
import inspect
import logging
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

    @property
    def protocol(self):
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


# encapsulate usb.core.USBError
class USBTransportError(Exception):
    def __init__(self, *args, **kwargs):
        super(USBTransportError, self).__init__(*args)
        self.errno = kwargs.pop("errno", None)
        self.backend_error_code = kwargs.pop("error_code", None)
        if kwargs:
            raise TypeError("USBTransportError got unexpected kwarg")

    @classmethod
    def from_usberror(cls, err):
        return cls(str(err), errno=err.errno, error_code=err.backend_error_code)


class USBTransportDeviceInUse(Exception):
    pass


# this can and should be opaque to pyseabreeze
class USBTransportHandle(object):
    def __init__(self, pyusb_device):
        """encapsulation for pyusb device classes

        Parameters
        ----------
        pyusb_device : usb.core.Device
        """
        self.pyusb_device = pyusb_device
        # noinspection PyUnresolvedReferences
        self.identity = (
            pyusb_device.idVendor,
            pyusb_device.idProduct,
            pyusb_device.bus,
            pyusb_device.address,
        )
        self.pyusb_backend = get_name_from_pyusb_backend(pyusb_device.backend)

    def close(self):
        try:
            self.pyusb_device.reset()
        except usb.core.USBError:
            logging.debug(
                "USBError while calling USBTransportHandle.close on {:04x}:{:04x}".format(
                    self.identity[0], self.identity[1]
                ),
                exc_info=True,
            )

    def __del__(self):
        if self.pyusb_backend == "libusb1":
            # have to check if .finalize() has been called
            # -> todo: maybe better to fix this in the api initialization of cseabreeze
            # -> todo: will probably have to check pyusb versions and only do this when necessary
            if not getattr(self.pyusb_device.backend, "_finalize_called", False):
                # if usb.core.Device.reset() gets called but the backend has been finalized already
                # (this happens only during interpreter shutdown)
                self.close()
        else:
            self.close()
        self.pyusb_device = None


class USBTransport(TransportInterface):
    """implementation of the usb transport interface for spectrometers"""

    _required_init_kwargs = ("usb_product_id", "usb_endpoint_map", "usb_protocol")
    vendor_id = 0x2457
    product_ids = {}

    # add logging
    _log = logging.getLogger(__name__)

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
        self._default_read_spectrum_endpoint = "high_speed"
        # internal state
        self._device = None
        self._opened = None
        self._protocol = None

    def open_device(self, device):
        if not isinstance(device, USBTransportHandle):
            raise TypeError("device needs to be a USBTransportHandle")
        # device.reset()
        self._device = device
        pyusb_device = self._device.pyusb_device
        try:
            if pyusb_device.is_kernel_driver_active(0):
                pyusb_device.detach_kernel_driver(0)
        except NotImplementedError:
            pass  # unavailable on some systems/backends
        try:
            pyusb_device.set_configuration()
        except usb.core.USBError as err:
            if err.errno == 16:
                # TODO: warn as in cseabreeze
                self._opened = True
                raise USBTransportDeviceInUse(
                    "device probably used by another thread/process"
                )
            raise USBTransportError.from_usberror(err)
        else:
            self._opened = True
        # This will initialize the communication protocol
        if self._opened:
            self._protocol = self._protocol_cls(self)

    @property
    def is_open(self):
        return self._opened or False

    def close_device(self):
        if self._device is not None:
            self._device.close()
            self._device = None
        self._opened = False
        self._protocol = None

    def write(self, data, timeout_ms=None, **kwargs):
        if self._device is None:
            raise RuntimeError("device not opened")
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        return self._device.pyusb_device.write(
            self._endpoint_map.ep_out, data, timeout=timeout_ms
        )

    def read(self, size=None, timeout_ms=None, mode=None, **kwargs):
        if self._device is None:
            raise RuntimeError("device not opened")
        mode = mode if mode is not None else self._default_read_endpoint
        endpoint = getattr(self._endpoint_map, self._read_endpoints[mode])
        if size is None:
            size = self._default_read_size[mode]
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        return self._device.pyusb_device.read(endpoint, size, timeout=timeout_ms)

    @property
    def default_timeout_ms(self):
        return self._device.pyusb_device.default_timeout if self._device else None

    @property
    def protocol(self):
        return self._protocol

    @classmethod
    def list_devices(cls, **kwargs):
        """list pyusb devices for all available spectrometers

        Note: this includes spectrometers that are currently opened in other
        processes on the machine.

        Yields
        ------
        devices : USBTransportHandle
            unique pyusb devices for each available spectrometer
        """
        # check if a specific pyusb backend is requested
        _pyusb_backend = kwargs.get("_pyusb_backend", None)
        # get all matching devices
        try:
            pyusb_devices = usb.core.find(
                find_all=True,
                custom_match=lambda dev: (
                    dev.idVendor == cls.vendor_id and dev.idProduct in cls.product_ids
                ),
                backend=get_pyusb_backend_from_name(name=_pyusb_backend),
            )
        except usb.core.NoBackendError:
            raise RuntimeError("No pyusb backend found")
        # encapsulate
        for pyusb_device in pyusb_devices:
            yield USBTransportHandle(pyusb_device)

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
        """return supported model

        Parameters
        ----------
        device : USBTransportHandle
        """
        if not isinstance(device, USBTransportHandle):
            return None
        # noinspection PyUnresolvedReferences
        return cls.product_ids[device.pyusb_device.idProduct]

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

    @classmethod
    def initialize(cls, **_kwargs):
        for device in cls.list_devices(**_kwargs):
            try:
                device.pyusb_device.reset()
                # usb.util.dispose_resources(device)  <- already done by device.reset()
            except Exception as err:
                cls._log.debug(
                    "initialize failed: {}('{}')".format(
                        err.__class__.__name__, getattr(err, "message", "no message")
                    )
                )

    @classmethod
    def shutdown(cls, **_kwargs):
        # dispose usb resources
        for device in cls.list_devices(**_kwargs):
            try:
                usb.util.dispose_resources(device.pyusb_device)
            except Exception as err:
                cls._log.debug(
                    "shutdown failed: {}('{}')".format(
                        err.__class__.__name__, getattr(err, "message", "no message")
                    )
                )


_pyusb_backend_instances = {}


def get_pyusb_backend_from_name(name):
    """internal: allow requesting a specific pyusb backend for testing"""
    if name is None:
        # default is pick first that works: ('libusb1', 'libusb0', 'openusb0')
        _backend = None
    else:
        try:
            _backend = _pyusb_backend_instances[name]
        except KeyError:
            m = importlib.import_module("usb.backend.{}".format(name))
            # noinspection PyUnresolvedReferences
            _backend = m.get_backend()
            # raise if a pyusb backend was requested but can't be loaded
            if _backend is None:
                raise RuntimeError("pyusb '{}' backend failed to load")
            _pyusb_backend_instances[name] = _backend
    return _backend


def get_name_from_pyusb_backend(backend):
    """internal: return backend name from loaded backend"""
    module = inspect.getmodule(backend)
    if not module:
        return None
    return module.__name__.split(".")[-1]
