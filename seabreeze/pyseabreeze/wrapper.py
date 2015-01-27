"""This is the pyusb implementation of the seabreeze library

Author: Andreas Poehlmann

most of this code is based on implementations from
    http://github.com/ap--/python-oceanoptics

"""
import usb.core
import usb.util
from .interfaces import defines
from .interfaces import SeaBreezeError, USBInterfaces


class SeaBreezeDevice(object):
    def __init__(self, handle, model, serial, interface):
        self.handle = handle
        self.model = model
        self.serial = serial
        # this will be set by list_spectrometers
        self.interface = interface

    def __repr__(self):
        return "<SeaBreezeDevice %s:%s>" % (self.model, self.serial)


def reset_spectrometers():
    # get all matching devices
    match = lambda dev: (dev.idVendor == defines.VendorId and
                         dev.idProduct in defines.ProductIds)
    devices = usb.core.find(find_all=True, custom_match=match)
    devices = list(devices)  # some pyusb versions return a generator
    # create the handles and associate protocols
    out = []
    for dev in devices:
        try:
            dev.reset()
            usb.util.dispose_resources(dev)
        except:
            pass
    return


def list_spectrometers():
    # get all matching devices
    match = lambda dev: (dev.idVendor == defines.VendorId and
                         dev.idProduct in defines.ProductIds)
    devices = usb.core.find(find_all=True, custom_match=match)
    devices = list(devices)  # some pyusb versions return a generator
    # create the handles and associate protocols
    out = []
    for dev in devices:
        # get the correct communication interface
        interface = USBInterfaces[dev.idProduct]()
        # open the device and get the serial and model numbers
        try:
            interface.open_device(dev)  # Ignore device if already opened
        except usb.core.USBError:
            continue
        model = interface.get_model()
        serial = interface.get_serial_number()
        # close and create list of SeaBreezeDevices
        interface.close_device()
        out.append(SeaBreezeDevice(dev, model, serial, interface))
    return out

# SPECTROMETERFEATURE
def open_spectrometer(device):
    return device.interface.open_spectrometer(device)

def close_spectrometer(device):
    return device.interface.close_spectrometer()

def get_model(device):
    return device.interface.get_model()

def set_trigger_mode(device, mode):
    return device.interface.set_trigger_mode(mode)

def set_integration_time_microsec(device, integration_time_micros):
    return device.interface.set_integration_time_microsec(integration_time_micros)

def get_min_integration_time_microsec(device):
    return device.interface.get_min_integration_time_microsec()

# SHUTTERFEATURE
def set_shutter_open(device, opened):
    return device.interface.set_shutter_open(opened)

# STROBE...
def set_strobe_enable(device, strobe_enable):
    return device.interface.set_strobe_enable(strobe_enable)

# LIGHTSOURCEFEATURE
def get_light_source_count(device):
    return device.interface.get_light_source_count()

def set_light_source_enable(device, light_index, enable):
    return device.interface.set_light_source_enable(light_index, enable)

def set_light_source_intensity(device, light_index, intensity):
    return device.interface.set_light_source_intensity(light_index, intensity)

# EEPROMFEATURE
def read_eeprom_slot(device, slot_number):
    return device.interface.read_eeprom_slot(slot_number)

def write_eeprom_slot(device, slot_number, data):
    return device.interface.write_eeprom_slot(slot_number, data)

# IRRADCALFEATURE
def read_irrad_calibration(device):
    return device.interface.read_irrad_calibration()

def write_irrad_calibration(device, data):
    return device.interface.write_irrad_calibration(data)

def has_irrad_collection_area(device):
    return device.interface.has_irrad_collection_area()

def read_irrad_collection_area(device):
    return device.interface.read_irrad_collection_area()

def write_irrad_collection_area(device, area):
    return device.interface.write_irrad_collection_area(area)

# TECFEATURE
def read_tec_temperature(device):
    return device.interface.read_tec_temperature()

def set_tec_temperature(device, temperature_degrees_celsius):
    return device.interface.set_tec_temperature(temperature_degrees_celsius)

def set_tec_enable(device, tec_enable):
    return device.interface.set_tec_enable(tec_enable)

def set_tec_fan_enable(device, tec_fan_enable):
    return device.interface.set_tec_fan_enable(tec_fan_enable)

# SPECTROMETERFEATURE
def get_unformatted_spectrum(device, out):
    return device.interface.get_unformatted_spectrum(out)

def get_formatted_spectrum(device, out):
    return device.interface.get_formatted_spectrum(out)

def get_unformatted_spectrum_length(device):
    return device.interface.get_unformatted_spectrum_length()

def get_formatted_spectrum_length(device):
    return device.interface.get_formatted_spectrum_length()

def get_wavelengths(device, out):
    return device.interface.get_wavelengths(out)

# SERIALNUMBERFEATURE
def get_serial_number(device):
    return device.interface.get_serial_number()

# SPECTROMETERFEATURE
def get_electric_dark_pixel_indices(device):
    return device.interface.get_electric_dark_pixel_indices()

# CONTINOUSSTROBEFEATURE
def set_continuous_strobe_period_microsec(device, strobe_id, period_usec):
    return device.interface.set_continuous_strobe_period_microsec(strobe_id, period_usec)

