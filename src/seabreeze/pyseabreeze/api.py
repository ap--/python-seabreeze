"""This is the pyusb implementation of the seabreeze library

Author: Andreas Poehlmann

most of this code is based on implementations from
    http://github.com/ap--/python-oceanoptics

"""
import logging
import weakref

from seabreeze.pyseabreeze.devices import SeaBreezeDevice, _model_class_registry
from seabreeze.pyseabreeze.transport import (
    USBTransport,
    USBTransportDeviceInUse,
    USBTransportError,
    USBTransportHandle,
)

__all__ = ["SeaBreezeAPI"]


class SeaBreezeAPI(object):
    """SeaBreeze API interface"""

    _log = logging.getLogger(__name__)

    def __init__(self, initialize=True):
        if initialize:
            self.initialize()

    @staticmethod
    def initialize():
        """initialize the api backend

        normally this function does not have to be called directly by the user.
        it resets all usb devices on load
        """
        USBTransport.initialize()

    @staticmethod
    def shutdown():
        """shutdown the api backend

        normally this function does not have to be called directly by the user
        """
        # dispose usb resources
        USBTransport.shutdown()

    @staticmethod
    def add_rs232_device_location(device_type, bus_path, baudrate):
        """add RS232 device location"""
        # RS232Transport.register_device(device_type, bus_path, baudrate)
        raise NotImplementedError("rs232 communication not implemented for pyseabreeze")

    @staticmethod
    def add_ipv4_device_location(device_type, ip_address, port):
        """add ipv4 device location"""
        # IPV4Transport.register_device(device_type, ip_address, port)
        raise NotImplementedError("ipv4 communication not implemented for pyseabreeze")

    @staticmethod
    def list_devices():
        """returns available SeaBreezeDevices

        list all connected Ocean Optics devices supported
        by libseabreeze.

        Returns
        -------
        devices: list of SeaBreezeDevice
            connected Spectrometer instances
        """
        # get all matching devices
        devices = []
        for usb_dev in USBTransport.list_devices():
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
            devices.append(dev)
        return devices

    @staticmethod
    def supported_models():
        """returns SeaBreezeDevices supported by the backend

        models supported by the backend

        Returns
        -------
        devices: list of str
            list of model names that are supported by this backend
        """
        return [x for x in sorted(_model_class_registry.keys())]


# create only one SeaBreezeDevice instance per handle
_seabreeze_device_instance_registry = weakref.WeakValueDictionary()


def _seabreeze_device_factory(device):
    """return existing instances instead of creating temporary ones

    Parameters
    ----------
    device : USBTransportHandle

    Returns
    -------
    dev : SeaBreezeDevice
    """
    if not isinstance(device, USBTransportHandle):
        raise TypeError("needs to be instance of USBTransportHandle")
    ident = device.identity
    try:
        return _seabreeze_device_instance_registry[ident]
    except KeyError:
        dev = _seabreeze_device_instance_registry[ident] = SeaBreezeDevice(device)
        return dev
