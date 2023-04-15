"""Implementation of the Seabreeze Transport layer.

Some spectrometers can support different transports (usb, network, rs232, etc.)

"""
from __future__ import annotations

import importlib
import inspect
import logging
import warnings
from functools import partialmethod
from typing import TYPE_CHECKING
from typing import Any
from typing import Iterable
from typing import Tuple

import usb.backend
import usb.core
import usb.util

from seabreeze.pyseabreeze.types import PySeaBreezeProtocol
from seabreeze.pyseabreeze.types import PySeaBreezeTransport

if TYPE_CHECKING:
    from seabreeze.pyseabreeze.devices import EndPointMap


# encapsulate usb.core.USBError
class USBTransportError(Exception):
    def __init__(
        self, *args: Any, errno: int | None = None, error_code: int | None = None
    ) -> None:
        super().__init__(*args)
        self.errno = errno
        self.backend_error_code = error_code

    @classmethod
    def from_usberror(cls, err: usb.core.USBError) -> USBTransportError:
        return cls(str(err), errno=err.errno, error_code=err.backend_error_code)


class USBTransportDeviceInUse(Exception):
    pass


DeviceIdentity = Tuple[int, int, int, int]


# this can and should be opaque to pyseabreeze
class USBTransportHandle:
    def __init__(self, pyusb_device: usb.core.Device) -> None:
        """encapsulation for pyusb device classes

        Parameters
        ----------
        pyusb_device
        """
        self.pyusb_device: usb.core.Device = pyusb_device
        # noinspection PyUnresolvedReferences
        self.identity: DeviceIdentity = (
            pyusb_device.idVendor,
            pyusb_device.idProduct,
            pyusb_device.bus,
            pyusb_device.address,
        )
        self.pyusb_backend = get_name_from_pyusb_backend(pyusb_device.backend)

    def close(self) -> None:
        try:
            self.pyusb_device.reset()
        except usb.core.USBError:
            logging.debug(
                "USBError while calling USBTransportHandle.close on {:04x}:{:04x}".format(
                    self.identity[0], self.identity[1]
                ),
                exc_info=True,
            )

    def __del__(self) -> None:
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


class USBTransport(PySeaBreezeTransport[USBTransportHandle]):
    """implementation of the usb transport interface for spectrometers"""

    _required_init_kwargs = (
        "usb_vendor_id",
        "usb_product_id",
        "usb_endpoint_map",
        "usb_protocol",
    )
    vendor_product_ids: dict[tuple[int, int], str] = {}

    # add logging
    _log = logging.getLogger(__name__)

    def __init__(
        self,
        usb_vendor_id: int,
        usb_product_id: int,
        usb_endpoint_map: EndPointMap,
        usb_protocol: type[PySeaBreezeProtocol],
    ) -> None:
        super().__init__()
        self._vendor_id = usb_vendor_id
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
        if self._endpoint_map.lowspeed_in is not None:
            self._default_read_endpoint = "low_speed"
        else:
            self._default_read_endpoint = "high_speed"
        self._default_read_spectrum_endpoint = "high_speed"
        # internal state
        self._device: USBTransportHandle | None = None
        self._opened: bool | None = None
        self._protocol: PySeaBreezeProtocol | None = None

    def open_device(self, device: USBTransportHandle) -> None:
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
    def is_open(self) -> bool:
        return self._opened or False

    def close_device(self) -> None:
        if self._device is not None:
            self._device.close()
            self._device = None
        self._opened = False
        self._protocol = None

    def write(self, data: bytes, timeout_ms: int | None = None, **kwargs: Any) -> int:
        if self._device is None:
            raise RuntimeError("device not opened")
        if kwargs:
            warnings.warn(f"kwargs provided but ignored: {kwargs}")
        return self._device.pyusb_device.write(  # type: ignore
            self._endpoint_map.ep_out, data, timeout=timeout_ms
        )

    def read(
        self,
        size: int | None = None,
        timeout_ms: int | None = None,
        mode: str | None = None,
        **kwargs: Any,
    ) -> bytes:
        if self._device is None:
            raise RuntimeError("device not opened")
        mode = mode if mode is not None else self._default_read_endpoint
        endpoint = getattr(self._endpoint_map, self._read_endpoints[mode])
        if size is None:
            size = self._default_read_size[mode]
        if kwargs:
            warnings.warn(f"kwargs provided but ignored: {kwargs}")
        ret: bytes = self._device.pyusb_device.read(
            endpoint, size, timeout=timeout_ms
        ).tobytes()
        return ret

    @property
    def default_timeout_ms(self) -> int:
        if not self._device:
            raise RuntimeError("no protocol instance available")
        return self._device.pyusb_device.default_timeout  # type: ignore

    @property
    def protocol(self) -> PySeaBreezeProtocol:
        if self._protocol is None:
            raise RuntimeError("no protocol instance available")
        return self._protocol

    @classmethod
    def list_devices(cls, **kwargs: Any) -> Iterable[USBTransportHandle]:
        """list pyusb devices for all available spectrometers

        Note: this includes spectrometers that are currently opened in other
        processes on the machine.

        Yields
        ------
        devices : USBTransportHandle
            unique pyusb devices for each available spectrometer
        """
        # check if a specific pyusb backend is requested
        _pyusb_backend = kwargs.get("pyusb_backend", None)
        # get all matching devices
        try:
            pyusb_devices = usb.core.find(
                find_all=True,
                custom_match=lambda dev: (
                    (dev.idVendor, dev.idProduct) in cls.vendor_product_ids
                ),
                backend=get_pyusb_backend_from_name(name=_pyusb_backend),
            )
        except usb.core.NoBackendError:
            raise RuntimeError("No pyusb backend found")
        # encapsulate
        for pyusb_device in pyusb_devices:
            yield USBTransportHandle(pyusb_device)

    @classmethod
    def register_model(cls, model_name: str, **kwargs: Any) -> None:
        vendor_id = kwargs.get("usb_vendor_id")
        if not isinstance(vendor_id, int):
            raise TypeError(f"vendor_id {vendor_id:r} not an integer")
        product_id = kwargs.get("usb_product_id")
        if not isinstance(product_id, int):
            raise TypeError(f"product_id {product_id:r} not an integer")
        if (vendor_id, product_id) in cls.vendor_product_ids:
            raise ValueError(
                f"vendor_id:product_id {vendor_id:04x}:{product_id:04x} already in registry"
            )
        cls.vendor_product_ids[(vendor_id, product_id)] = model_name

    @classmethod
    def supported_model(cls, device: USBTransportHandle) -> str | None:
        """return supported model

        Parameters
        ----------
        device : USBTransportHandle
        """
        if not isinstance(device, USBTransportHandle):
            return None
        # noinspection PyUnresolvedReferences
        return cls.vendor_product_ids[
            (device.pyusb_device.idVendor, device.pyusb_device.idProduct)
        ]

    @classmethod
    def specialize(cls, model_name: str, **kwargs: Any) -> type[USBTransport]:
        assert set(kwargs) == set(cls._required_init_kwargs)
        # usb transport register automatically on registration
        cls.register_model(model_name, **kwargs)
        specialized_class = type(
            f"USBTransport{model_name}",
            (cls,),
            {"__init__": partialmethod(cls.__init__, **kwargs)},
        )
        return specialized_class

    @classmethod
    def initialize(cls, **_kwargs: Any) -> None:
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
    def shutdown(cls, **_kwargs: Any) -> None:
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


_pyusb_backend_instances: dict[str, usb.backend.IBackend] = {}


def get_pyusb_backend_from_name(name: str) -> usb.backend.IBackend:
    """internal: allow requesting a specific pyusb backend for testing"""
    if name is None:
        # default is pick first that works: ('libusb1', 'libusb0', 'openusb')
        _backend = None
    else:
        try:
            _backend = _pyusb_backend_instances[name]
        except KeyError:
            try:
                m = importlib.import_module(f"usb.backend.{name}")
            except ImportError:
                raise RuntimeError(f"unknown pyusb backend: {name!r}")
            # noinspection PyUnresolvedReferences
            _backend = m.get_backend()
            # raise if a pyusb backend was requested but can't be loaded
            if _backend is None:
                raise RuntimeError(f"pyusb backend failed to load: {name!r}")
            _pyusb_backend_instances[name] = _backend
    return _backend


def get_name_from_pyusb_backend(backend: usb.backend.IBackend) -> str | None:
    """internal: return backend name from loaded backend"""
    module = inspect.getmodule(backend)
    if not module:
        return None
    return module.__name__.split(".")[-1]
