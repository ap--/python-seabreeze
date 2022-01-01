"""This is the pyusb implementation of the seabreeze library

Author: Andreas Poehlmann

most of this code is based on implementations from
    http://github.com/ap--/python-oceanoptics

"""
from __future__ import annotations

import logging
import weakref
from typing import TYPE_CHECKING
from typing import Any

from seabreeze.pyseabreeze.devices import SeaBreezeDevice
from seabreeze.pyseabreeze.devices import _model_class_registry
from seabreeze.pyseabreeze.transport import DeviceIdentity
from seabreeze.pyseabreeze.transport import USBTransport
from seabreeze.pyseabreeze.transport import USBTransportDeviceInUse
from seabreeze.pyseabreeze.transport import USBTransportError
from seabreeze.pyseabreeze.transport import USBTransportHandle
from seabreeze.types import SeaBreezeAPI as _SeaBreezeAPIProtocol

if TYPE_CHECKING:
    from seabreeze.types import SeaBreezeDevice as _SeaBreezeDevice


__all__ = ["SeaBreezeAPI"]

# create only one SeaBreezeDevice instance per handle
_seabreeze_device_instance_registry: weakref.WeakValueDictionary[
    DeviceIdentity, SeaBreezeDevice
] = weakref.WeakValueDictionary()


def _seabreeze_device_factory(device: USBTransportHandle) -> SeaBreezeDevice:
    """return existing instances instead of creating temporary ones

    Parameters
    ----------
    device : USBTransportHandle

    Returns
    -------
    dev : SeaBreezeDevice
    """
    global _seabreeze_device_instance_registry
    if not isinstance(device, USBTransportHandle):
        raise TypeError("needs to be instance of USBTransportHandle")
    ident = device.identity
    try:
        return _seabreeze_device_instance_registry[ident]
    except KeyError:
        dev = _seabreeze_device_instance_registry[ident] = SeaBreezeDevice(device)
        return dev


class SeaBreezeAPI(_SeaBreezeAPIProtocol):
    """SeaBreeze API interface"""

    _log = logging.getLogger(__name__)

    def __init__(self, initialize: bool = True, **_kwargs: Any) -> None:
        self._kwargs = _kwargs  # allow passing additional kwargs to transports
        if initialize:
            self.initialize()

    def initialize(self) -> None:
        """initialize the api backend

        normally this function does not have to be called directly by the user.
        it resets all usb devices on load
        """
        USBTransport.initialize(**self._kwargs)

    def shutdown(self) -> None:
        """shutdown the api backend

        normally this function does not have to be called directly by the user
        """
        # dispose usb resources
        _seabreeze_device_instance_registry.clear()
        USBTransport.shutdown(**self._kwargs)

    def add_rs232_device_location(
        self, device_type: str, bus_path: str, baudrate: int
    ) -> None:
        """add RS232 device location"""
        # RS232Transport.register_device(device_type, bus_path, baudrate)
        raise NotImplementedError("rs232 communication not implemented for pyseabreeze")

    def add_ipv4_device_location(
        self, device_type: str, ip_address: str, port: int
    ) -> None:
        """add ipv4 device location"""
        # IPV4Transport.register_device(device_type, ip_address, port)
        raise NotImplementedError("ipv4 communication not implemented for pyseabreeze")

    def list_devices(self) -> list[_SeaBreezeDevice]:
        """returns available SeaBreezeDevices

        list all connected Ocean Optics devices supported
        by libseabreeze.

        Returns
        -------
        devices:
            connected Spectrometer instances
        """
        # get all matching devices
        devices: list[_SeaBreezeDevice] = []
        for usb_dev in USBTransport.list_devices(**self._kwargs):
            # get the correct communication interface
            dev = _seabreeze_device_factory(usb_dev)
            if not dev.is_open:
                # opening the device will populate its serial number
                try:
                    dev.open()
                except USBTransportDeviceInUse:
                    # device used by another thread? -> exclude
                    continue
                except USBTransportError:
                    # todo: investigate if there are ways to recover from here
                    raise
                else:
                    dev.close()
            devices.append(dev)  # type: ignore
        return devices

    # note: to be fully consistent with cseabreeze this shouldn't be a staticmethod
    @staticmethod
    def supported_models() -> list[str]:
        """returns SeaBreezeDevices supported by the backend

        models supported by the backend

        Returns
        -------
        devices: list of str
            list of model names that are supported by this backend
        """
        return [
            x for x in sorted(_model_class_registry.keys()) if not x.startswith("_")
        ]
