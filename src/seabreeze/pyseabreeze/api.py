"""This is the pyusb implementation of the seabreeze library

Author: Andreas Poehlmann

most of this code is based on implementations from
    http://github.com/ap--/python-oceanoptics

"""
import logging

import usb.core
import usb.util
from seabreeze.pyseabreeze.devices import SeaBreezeDevice, _model_class_registry
from seabreeze.pyseabreeze.transport import USBTransport

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
        # create the handles and associate protocols
        for device in USBTransport.list_devices():
            try:
                device.reset()
                usb.util.dispose_resources(device)
            except Exception as err:
                SeaBreezeAPI._log.debug(
                    "initialize failed: {}('{}')".format(
                        err.__class__.__name__, err.message
                    )
                )

    @staticmethod
    def shutdown():
        """shutdown the api backend

        normally this function does not have to be called directly by the user
        """
        # dispose usb resources
        for device in USBTransport.list_devices():
            try:
                usb.util.dispose_resources(device)
            except Exception as err:
                SeaBreezeAPI._log.debug(
                    "shutdown failed: {}('{}')".format(
                        err.__class__.__name__, err.message
                    )
                )

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
        for pyusb_dev in USBTransport.list_devices():
            # get the correct communication interface
            dev = SeaBreezeDevice(pyusb_dev)
            if dev.is_open:
                was_open_before = True
            else:
                was_open_before = False
                try:
                    dev.open()
                except usb.core.USBError as usb_error:
                    if usb_error.errno == 16:
                        # device used by another thread?
                        continue
                    else:
                        raise
            if not was_open_before:
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
