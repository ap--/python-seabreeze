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


def initialize():
    # reset all devices on load
    match = lambda dev: (dev.idVendor == defines.VendorId and
                         dev.idProduct in defines.ProductIds)
    devices = usb.core.find(find_all=True, custom_match=match)
    # create the handles and associate protocols
    for dev in devices:
        try:
            dev.reset()
            usb.util.dispose_resources(dev)
        except:
            pass

def shutdown():
    # dispose usb resources
    match = lambda dev: (dev.idVendor == defines.VendorId and
                         dev.idProduct in defines.ProductIds)
    devices = usb.core.find(find_all=True, custom_match=match)
    for dev in devices:
        try:
            usb.util.dispose_resources(dev)
        except:
            pass


def device_list_devices():
    # get all matching devices
    match = lambda dev: (dev.idVendor == defines.VendorId and
                         dev.idProduct in defines.ProductIds)
    devices = usb.core.find(find_all=True, custom_match=match)
    # create the handles and associate protocols
    out = []
    for dev in devices:
        # get the correct communication interface
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
        out.append(SeaBreezeDevice(dev, model, serial, interface))
    return out

def device_open(device):
    return device.interface.open(device)

def device_is_open(device):
    return device.interface.is_open()

def device_close(device):
    return device.interface.close()

def device_get_model(device):
    return device.interface.get_model()

def device_get_serial_number(device):
    return device.interface.get_serial_number()


# SPECTROMETERFEATURE
def device_get_spectrometer_feature_id(device):
    return device.interface.has_spectrometer_feature()

def spectrometer_set_trigger_mode(device, has_feature, mode):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_trigger_mode(mode)

def spectrometer_set_integration_time_micros(device, has_feature, integration_time_micros):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_integration_time_microsec(integration_time_micros)

def spectrometer_get_minimum_integration_time_micros(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_min_integration_time_microsec()

def spectrometer_get_formatted_spectrum_length(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_formatted_spectrum_length()

def spectrometer_get_formatted_spectrum(device, has_feature, out):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_formatted_spectrum(out)

def spectrometer_get_unformatted_spectrum_length(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_unformatted_spectrum_length()

def spectrometer_get_unformatted_spectrum(device, has_feature, out):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_unformatted_spectrum(out)

def spectrometer_get_wavelengths(device, has_feature, out):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_wavelengths(out)

def spectrometer_get_electric_dark_pixel_indices(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_electric_dark_pixel_indices()


# SHUTTERFEATURE
def device_get_shutter_feature_id(device):
    return device.interface.has_shutter_feature()

def shutter_set_shutter_open(device, has_feature, opened):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_shutter_open(opened)


# LIGHTSOURCEFEATURE
def device_get_light_source_feature_id(device):
    return device.interface.has_light_source_feature()

def light_source_get_count(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_light_source_count()

def light_source_has_enable(device, has_feature, light_index):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def light_source_is_enabled(device, has_feature, light_index):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def light_source_set_enable(device, has_feature, light_index, enable):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_light_source_enable(light_index, enable)

def light_source_has_variable_intensity(device, has_feature, light_index):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def light_source_get_intensity(device, has_feature, light_index):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def light_source_set_intensity(device, has_feature, light_index, intensity):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_light_source_intensity(light_index, intensity)

# STROBE...
def device_get_continuous_strobe_feature_id(device):
    return device.interface.has_continuous_strobe_feature()

def continuous_strobe_set_enable(device, has_feature, strobe_enable):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_strobe_enable(strobe_enable)

def continuous_strobe_set_period_micros(device, has_feature, period_micros):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_continuous_strobe_period_microsec(period_micros)

# EEPROMFEATURE
def device_get_eeprom_feature_id(device):
    return device.interface.has_eeprom_feature()

def eeprom_read_slot(device, has_feature, slot_number):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.read_eeprom_slot(slot_number)

# IRRADCALFEATURE
def device_get_irrad_calibration_feature_id(device):
    return device.interface.has_irrad_calibration_feature()

def irrad_calibration_read(device, has_feature, out):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.read_irrad_calibration(out)

def irrad_calibration_write(device, has_feature, data):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.write_irrad_calibration(data)

def irrad_calibration_has_collection_area(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.has_irrad_collection_area()

def irrad_calibration_read_collection_area(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.read_irrad_collection_area()

def irrad_calibration_write_collection_area(device, has_feature, area):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.write_irrad_collection_area(area)


# TECFEATURE
def device_get_tec_feature_id(device):
    return device.interface.has_tec_feature()

def tec_read_temperature_degrees_C(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.read_tec_temperature()

def tec_set_temperature_setpoint_degrees_C(device, has_feature, temperature_degrees_celsius):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_tec_temperature(temperature_degrees_celsius)

def tec_set_enable(device, has_feature, tec_enable):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.set_tec_enable(tec_enable)


# LAMPFEATURE
def device_get_lamp_feature_id(device):
    return device.interface.has_lamp_feature()

def lamp_set_lamp_enable(device, has_feature, lamp_enable):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

# NONLINEARITY COEFFS FEATURE
def device_get_nonlinearity_coeffs_feature_id(device):
    return device.interface.has_nonlinearity_coeffs_feature()

def nonlinearity_coeffs_get(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    return device.interface.get_nonlinearity_coeffs()

# STRAYLIGHT COEFFS FEATURE
def device_get_stray_light_coeffs_feature_id(device):
    return device.interface.has_stray_light_feature()

def stray_light_coeffs_get(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

# SPECTRUM PROCESSING
def device_get_spectrum_processing_feature_id(device):
    return device.interface.has_spectrum_processing_feature()

def spectrum_processing_set_boxcar_width(device, has_feature, boxcar_width):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def spectrum_processing_set_scans_to_average(device, has_feature, scans_to_average):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def spectrum_processing_get_boxcar_width(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError

def spectrum_processing_get_scans_to_average(device, has_feature):
    if not has_feature:
        raise SeaBreezeError("Error: No such feature on device")
    raise NotImplementedError
