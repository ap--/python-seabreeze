
import logging
import traceback

# cseabreeze will always raise SeaBreezeErrors, so we need to convert them for
# pyseabreeze too. We'll also add some logging for debugging.
def get_pyseabreeze_decorator(name):
    logger = logging.getLogger('python-seabreeze.pyseabreeze.%s' % name)
    def convert_exceptions(msg):
        """Decorator for logging tracebacks"""
        def decorator(func):
            def decorated_func(*args, **kwargs):
                try:
                    return func(*args, **kwargs)
                except NotImplementedError:
                    raise
                except Exception as e:
                    # logger.debug(traceback.format_exc())
                    print traceback.format_exc()
                    if isinstance(e, SeaBreezeError):
                        raise
                    else:
                        raise SeaBreezeError(msg)
            return decorated_func
        return decorator
    return convert_exceptions


class SeaBreezeError(Exception):
    pass


class NotImplementedWrapper(object):

    def open_spectrometer(self):
        raise NotImplementedError

    def close_spectrometer(self):
        raise NotImplementedError

    def get_model(self):
        raise NotImplementedError

    def set_trigger_mode(self, mode):
        raise NotImplementedError

    def set_integration_time_microsec(self, integration_time_micros):
        raise NotImplementedError

    def get_min_integration_time_microsec(self):
        raise NotImplementedError

    def get_unformatted_spectrum(self):
        raise NotImplementedError

    def get_formatted_spectrum(self):
        raise NotImplementedError

    def get_unformatted_spectrum_length(self):
        raise NotImplementedError

    def get_formatted_spectrum_length(self):
        raise NotImplementedError

    def get_wavelengths(self):
        raise NotImplementedError

    def get_electric_dark_pixel_indices(self):
        raise NotImplementedError

    def set_shutter_open(self, opened):
        raise NotImplementedError

    def read_irrad_calibration(self):
        raise NotImplementedError

    def write_irrad_calibration(self, data):
        raise NotImplementedError

    def has_irrad_collection_area(self):
        raise NotImplementedError

    def read_irrad_collection_area(self):
        raise NotImplementedError

    def write_irrad_collection_area(self, area):
        raise NotImplementedError

    def get_light_source_count(self):
        raise NotImplementedError

    def set_light_source_enable(self, light_index, enable):
        raise NotImplementedError

    def set_light_source_intensity(self, light_index, intensity):
        raise NotImplementedError

    def read_eeprom_slot(self, slot_number):
        raise NotImplementedError

    def write_eeprom_slot(self, slot_number, data):
        raise NotImplementedError

    def read_tec_temperature(self):
        raise NotImplementedError

    def set_tec_temperature(self, temperature_degrees_celsius):
        raise NotImplementedError

    def set_tec_enable(self, tec_enable):
        raise NotImplementedError

    def set_tec_fan_enable(self, tec_fan_enable):
        raise NotImplementedError

    def get_serial_number(self):
        raise NotImplementedError

    def set_continuous_strobe_period_microsec(self, strobe_id, period_usec):
        raise NotImplementedError


class NoSpectrometerFeature(object):

    def open_spectrometer(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def close_spectrometer(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_model(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_trigger_mode(self, mode):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_integration_time_microsec(self, integration_time_micros):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_min_integration_time_microsec(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_unformatted_spectrum(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_formatted_spectrum(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_unformatted_spectrum_length(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_formatted_spectrum_length(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_wavelengths(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_electric_dark_pixel_indices(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def get_serial_number(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")


class NoShutterFeature(object):

    def set_shutter_open(self, opened):
        raise SeaBreezeError("Feature not supported by spectrometer model")


class NoIrradCalFeature(object):

    def read_irrad_calibration(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def write_irrad_calibration(self, data):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def has_irrad_collection_area(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def read_irrad_collection_area(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def write_irrad_collection_area(self, area):
        raise SeaBreezeError("Feature not supported by spectrometer model")


class NoLightSourceFeature(object):

    def get_light_source_count(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_light_source_enable(self, light_index, enable):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_light_source_intensity(self, light_index, intensity):
        raise SeaBreezeError("Feature not supported by spectrometer model")


class NoEEPromFeature(object):

    def read_eeprom_slot(self, slot_number):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def write_eeprom_slot(self, slot_number, data):
        raise SeaBreezeError("Feature not supported by spectrometer model")


class NoTecFeature(object):

    def read_tec_temperature(self):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_tec_temperature(self, temperature_degrees_celsius):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_tec_enable(self, tec_enable):
        raise SeaBreezeError("Feature not supported by spectrometer model")

    def set_tec_fan_enable(self, tec_fan_enable):
        raise SeaBreezeError("Feature not supported by spectrometer model")

class NoContinousStrobeFeature(object):

    def set_continuous_strobe_period_microsec(self, strobe_id, period_usec):
        raise SeaBreezeError("Feature not supported by spectrometer model")

