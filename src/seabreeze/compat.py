"""seabreeze.compat compatibility layer with 0.6.x seabreeze

seabreeze 0.6.x

"""
import warnings


def _deprecation_warning(old_method, new_feature_method, version):
    msg = "{old_method} will be deprecated in version {ver}, use {feature} feature via f.{feature}.{method} instead"
    feature, method = new_feature_method.split(".")
    warnings.warn(
        msg.format(old_method=old_method, feature=feature, method=method, ver=version),
        PendingDeprecationWarning,
        stacklevel=2,
    )


class DeprecatedSpectrometerMixin(object):
    @property
    def f(self):
        """implemented in class using mixin"""
        return None

    def close(self):
        raise NotImplementedError("implemented in class using mixin")

    @property
    def integration_time_micros_limits(self):
        """implemented in class using mixin"""
        return None, None

    # Compatibility Layer Methods
    # ===========================
    #
    # will all go away in version 2
    #
    def boxcar_width(self, boxcar_width):
        _deprecation_warning(
            "boxcar_width", "spectrum_processing.set_boxcar_width", version="2"
        )
        self.f.spectrum_processing.set_boxcar_width(boxcar_width)

    def scans_to_average(self, scans_to_average):
        _deprecation_warning(
            "scans_to_average", "spectrum_processing.set_scans_to_average", version="2"
        )
        self.f.spectrum_processing.set_scans_to_average(self, scans_to_average)

    def get_boxcar_width(self):
        _deprecation_warning(
            "get_boxcar_width", "spectrum_processing.get_boxcar_width", version="2"
        )
        return self.f.spectrum_processing.get_boxcar_width()

    def get_scans_to_average(self):
        _deprecation_warning(
            "get_scans_to_average",
            "spectrum_processing.get_scans_to_average",
            version="2",
        )
        return self.f.spectrum_processing.get_scans_to_average()

    @property
    def minimum_integration_time_micros(self):
        return self.integration_time_micros_limits[0]

    @property
    def light_sources(self):
        _deprecation_warning("light_sources", "light_source.xxx", version="2")
        N = self.f.light_source.get_count()
        return tuple(DeprecatedLightSource(self.f.light_source, i) for i in range(N))

    def eeprom_read_slot(self, slot):
        _deprecation_warning("eeprom_read_slot", "eeprom.eeprom_read_slot", version="2")
        return self.f.eeprom.eeprom_read_slot(slot)

    def tec_set_enable(self, enable):
        _deprecation_warning(
            "tec_set_enable", "thermo_electric.enable_tec", version="2"
        )
        self.f.thermo_electric.enable_tec(enable)

    # noinspection PyPep8Naming
    def tec_set_temperature_C(self, set_point_C):
        _deprecation_warning(
            "tec_set_temperature_C",
            "thermo_electric.set_temperature_setpoint_degrees_celsius",
            version="2",
        )
        self.f.thermo_electric.set_temperature_setpoint_degrees_celsius(set_point_C)

    # noinspection PyPep8Naming
    def tec_get_temperature_C(self):
        _deprecation_warning(
            "tec_get_temperature_C",
            "thermo_electric.read_temperature_degrees_celsius",
            version="2",
        )
        return self.f.thermo_electric.read_temperature_degrees_celsius()

    def lamp_set_enable(self, enable):
        _deprecation_warning("lamp_set_enable", "strobe_lamp.enable_lamp", version="2")
        self.f.strobe_lamp.enable_lamp(enable)

    def shutter_set_open(self, state):
        _deprecation_warning(
            "shutter_set_open", "shutter.set_shutter_open", version="2"
        )
        self.f.shutter.set_shutter_open(state)

    def stray_light_coeffs(self):
        _deprecation_warning(
            "stray_light_coeffs",
            "stray_light_coefficients.get_stray_light_coefficients",
            version="2",
        )
        return self.f.stray_light_coefficients.get_stray_light_coefficients()

    def irrad_calibration(self):
        _deprecation_warning(
            "irrad_calibration", "irrad_cal.read_calibration", version="2"
        )
        return self.f.irrad_cal.read_calibration()

    def irrad_calibration_collection_area(self):
        _deprecation_warning(
            "irrad_calibration_collection_area",
            "irrad_cal.read_collection_area",
            version="2",
        )
        return self.f.irrad_cal.read_collection_area()

    def continuous_strobe_set_enable(self, enable):
        _deprecation_warning(
            "continuous_strobe_set_enable", "continuous_strobe.set_enable", version="2"
        )
        self.f.continuous_strobe.set_enable(enable)

    def continuous_strobe_set_period_micros(self, period_micros):
        _deprecation_warning(
            "continuous_strobe_set_period_micros",
            "continuous_strobe.set_period_micros",
            version="2",
        )
        self.f.continuous_strobe.set_period_micros(period_micros)

    def __enter__(self):
        warnings.warn(
            "context manager functionality will be a noop in version 2",
            PendingDeprecationWarning,
        )
        return self

    # noinspection PyShadowingBuiltins
    def __exit__(self, type, value, traceback):
        warnings.warn(
            "context manager functionality will be a noop in version 2",
            PendingDeprecationWarning,
        )
        self.close()


class DeprecatedLightSource(object):
    def __init__(self, spectrometer, index):
        self._id = "{}:{}".format(spectrometer.model, spectrometer.serial_number)
        self._f, self._idx = spectrometer.f.light_source, index
        self._has_enable = self._f.has_enable(self._idx)
        self._has_varint = self._f.has_variable_intensity(self._idx)

    def set_enable(self, enable):
        if self._has_enable:
            self._f.set_enable(self._idx, enable)
        else:
            from seabreeze.spectrometers import SeaBreezeError

            raise SeaBreezeError("Light source #%d can't be enabled or disabled.")

    def set_intensity(self, intensity):
        if self._has_varint:
            self._f.set_intensity(self._idx, intensity)
        else:
            from seabreeze.spectrometers import SeaBreezeError

            raise SeaBreezeError("Light source #%d intensity can't be set.")

    def get_intensity(self):
        from seabreeze.spectrometers import SeaBreezeError

        try:
            return self._f.get_intensity(self._idx)
        except SeaBreezeError:
            raise

    def __repr__(self):
        return "<DeprecatedLightSource #%d at %s>" % (self._idx, self._id)
