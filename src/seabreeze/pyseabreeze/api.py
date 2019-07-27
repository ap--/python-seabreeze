"""This is the pyusb implementation of the seabreeze library

Author: Andreas Poehlmann

most of this code is based on implementations from
    http://github.com/ap--/python-oceanoptics

"""
import usb.core
import usb.util
# from .interfaces import defines
# from .interfaces import SeaBreezeError, USBInterfaces
from seabreeze.pyseabreeze.devices import is_ocean_optics_usb_device, SeaBreezeDevice
from seabreeze.pyseabreeze.exceptions import SeaBreezeError


class SeaBreezeAPI(object):
    """SeaBreeze API interface"""

    def __init__(self, initialize=True):
        if initialize:
            self.initialize()

    def initialize(self):
        """initialize the api backend

        normally this function does not have to be called directly by the user.
        it resets all usb devices on load
        """
        # create the handles and associate protocols
        for device in self._list_pyusb_devices():
            try:
                device.reset()
                usb.util.dispose_resources(device)
            except:
                pass

    def shutdown(self):
        """shutdown the api backend

        normally this function does not have to be called directly by the user
        """
        # dispose usb resources
        for device in self._list_pyusb_devices():
            try:
                usb.util.dispose_resources(device)
            except:
                pass

    def add_rs232_device_location(self, device_type, bus_path, baudrate):
        """add RS232 device location"""
        raise NotImplementedError("rs232 communication not implemented for pyseabreeze")

    def add_ipv4_device_location(self, device_type, ip_address, port):
        """add ipv4 device location"""
        raise NotImplementedError("ipv4 communication not implemented for pyseabreeze")

    def _list_pyusb_devices(self):
        """list pyusb devices for all available spectrometers

        Note: this includes spectrometers that are currently opened in other
        processes on the machine.

        Returns
        -------
        devices : list of usb.core.Device
            unique pyusb devices for each available spectrometer
        """
        # get all matching devices
        return usb.core.find(find_all=True,
                             custom_match=is_ocean_optics_usb_device)

    def list_devices(self):
        """returns available SeaBreezeDevices

        list all connected Ocean Optics devices supported
        by libseabreeze.

        Returns
        -------
        devices: list of SeaBreezeDevice
            connected Spectrometer instances
        """
        # Probe Devices on all Buses
        device_ids = self._list_device_ids()
        devices = []
        for handle in device_ids:
            dev = SeaBreezeDevice(handle)
            if dev.is_open:
                was_open_before = True
            else:
                was_open_before = False
                try:
                    dev.open()
                except SeaBreezeError as err:
                    if err.error_code == _ErrorCode.NO_DEVICE:
                        # device used by another thread?
                        continue
            model = dev.model
            serial = dev.serial_number
            if not was_open_before:
                dev.close()
            devices.append(dev)
        return devices

    def device_list_devices(self):
        """returns available SeaBreezeDevices

        list all connected Ocean Optics devices supported
        by libseabreeze.

        Returns
        -------
        devices: list of SeaBreezeDevice
            connected Spectrometer instances
        """
        # get all matching devices
        # create the handles and associate protocols
        devices = []
        for dev in self._list_pyusb_devices():
            # get the correct communication interface
            dev = SeaBreezeDevice(dev)
            if dev.is_open:
                was_open_before = True
            else:
                was_open_before = False
                try:
                    dev.open()
                except SeaBreezeError as err:
                    if err.error_code == _ErrorCode.NO_DEVICE:
                        # device used by another thread?
                        continue
            model = dev.model
            serial = dev.serial_number
            if not was_open_before:
                dev.close()
            devices.append(dev)


            interface = USBInterfaces[dev.idProduct]()
            # open the device and get the serial and model numbers
            try:
                interface.open_device(dev)
            except usb.USBError as usberr:
                if usberr.errno == 16:
                    wasopen = True
                else:
                    raise
            else:
                wasopen = False
            model = interface.get_model()
            serial = interface.get_serial_number()
            # close and create list of SeaBreezeDevices
            if not wasopen:
                interface.close_device()
            devices.append(SeaBreezeDevice(dev, model, serial, interface))
        return devices
