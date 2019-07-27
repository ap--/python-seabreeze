class _SeaBreezeFeatureInterface(object):
    pass


class _SeaBreezeRawUSBBusAccessFeatureInterface(_SeaBreezeFeatureInterface):
    def raw_usb_read(self, endpoint, buffer_length=1024):
        raise NotImplementedError("implement in derived class")

    def raw_usb_write(self, data, endpoint):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeSpectrometerFeatureInterface(_SeaBreezeFeatureInterface):
    def set_trigger_mode(self, mode):
        raise NotImplementedError("implement in derived class")

    def set_integration_time_micros(self, integration_time_micros):
        raise NotImplementedError("implement in derived class")

    def get_integration_time_micros_limits(self):
        raise NotImplementedError("implement in derived class")

    def get_maximum_intensity(self):
        raise NotImplementedError("implement in derived class")

    def get_electric_dark_pixel_indices(self):
        raise NotImplementedError("implement in derived class")

    def _spectrum_length(self):
        raise NotImplementedError("implement in derived class")

    def get_wavelengths(self):
        raise NotImplementedError("implement in derived class")

    def get_intensities(self):
        raise NotImplementedError("implement in derived class")

    def _get_spectrum_raw(self):
        raise NotImplementedError("implement in derived class")

    def _get_fast_buffer_spectrum(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezePixelBinningFeatureInterface(_SeaBreezeFeatureInterface):
    def set_binning_factor(self, factor):
        raise NotImplementedError("implement in derived class")

    def get_binning_factor(self):
        raise NotImplementedError("implement in derived class")

    def get_max_binning_factor(self):
        raise NotImplementedError("implement in derived class")

    binning_factor = property(get_binning_factor, set_binning_factor)
    max_binning_factor = property(get_max_binning_factor)

    def set_default_binning_factor(self, factor):
        raise NotImplementedError("implement in derived class")

    def get_default_binning_factor(self):
        raise NotImplementedError("implement in derived class")

    default_binning_factor = property(get_default_binning_factor, set_default_binning_factor)


class _SeaBreezeThermoElectricFeatureInterface(_SeaBreezeFeatureInterface):
    def read_temperature_degrees_celsius(self):
        raise NotImplementedError("implement in derived class")

    def set_temperature_setpoint_degrees_celsius(self, temperature):
        raise NotImplementedError("implement in derived class")

    def enable_tec(self, state):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeIrradCalFeatureInterface(_SeaBreezeFeatureInterface):
    def has_collection_area(self):
        raise NotImplementedError("implement in derived class")

    def read_collection_area(self):
        raise NotImplementedError("implement in derived class")

    def write_collection_area(self, area):
        raise NotImplementedError("implement in derived class")

    def read_calibration(self):
        raise NotImplementedError("implement in derived class")

    def write_calibration(self, calibration_array):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeEthernetConfigurationFeatureInterface(_SeaBreezeFeatureInterface):
    def get_mac_address(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_mac_address(self, interface_index, mac_address):
        raise NotImplementedError("implement in derived class")

    def get_gbe_enable_status(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_gbe_enable_status(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeMulticastFeatureInterface(_SeaBreezeFeatureInterface):
    def get_multicast_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_multicast_enable_state(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeIPv4FeatureInterface(_SeaBreezeFeatureInterface):
    def get_dhcp_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_dhcp_enable_state(self, interface_index, is_enabled):
        raise NotImplementedError("implement in derived class")

    def get_number_of_ipv4_addresses(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def get_ipv4_address(self, interface_index, address_index):
        raise NotImplementedError("implement in derived class")

    def get_default_gateway(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_default_gateway(self, interface_index, default_gateway_address):
        raise NotImplementedError("implement in derived class")

    def add_ipv4_address(self, interface_index, ipv4_address):
        raise NotImplementedError("implement in derived class")

    def delete_ipv4_address(self, interface_index, address_index):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeDHCPServerFeatureInterface(_SeaBreezeFeatureInterface):
    def get_address(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_address(self, interface_index, server_address):
        raise NotImplementedError("implement in derived class")

    def get_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_enable_state(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeNetworkConfigurationFeatureInterface(_SeaBreezeFeatureInterface):
    def get_number_of_network_interfaces(self):
        raise NotImplementedError("implement in derived class")

    def get_network_interface_connection_type(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def get_network_interface_enable_state(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_network_interface_enable_state(self, interface_index, enable_state):
        raise NotImplementedError("implement in derived class")

    def run_network_interface_self_test(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def save_network_interface_connection_settings(self, interface_index):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeWifiConfigurationFeatureInterface(_SeaBreezeFeatureInterface):
    def get_wifi_mode(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_wifi_mode(self, interface_index, wifi_mode):
        raise NotImplementedError("implement in derived class")

    def get_wifi_security_type(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_wifi_security_type(self, interface_index, security_type):
        raise NotImplementedError("implement in derived class")

    def get_wifi_ssid(self, interface_index):
        raise NotImplementedError("implement in derived class")

    def set_wifi_ssid(self, interface_index, ssid):
        raise NotImplementedError("implement in derived class")

    def set_wifi_pass_phrase(self, interface_index, pass_phrase):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeGPIOFeatureInterface(_SeaBreezeFeatureInterface):
    def get_number_of_gpio_pins(self):
        raise NotImplementedError("implement in derived class")

    def get_gpio_output_enable_vector(self):
        raise NotImplementedError("implement in derived class")

    def set_gpio_output_enable_vector(self, output_enable_vector, bit_mask):
        raise NotImplementedError("implement in derived class")

    def get_gpio_value_vector(self):
        raise NotImplementedError("implement in derived class")

    def set_gpio_value_vector(self, value_vector, bit_mask):
        raise NotImplementedError("implement in derived class")

    def get_number_of_egpio_pins(self):
        raise NotImplementedError("implement in derived class")

    def get_egpio_available_modes(self, pin_number):
        raise NotImplementedError("implement in derived class")

    def get_egpio_current_mode(self, pin_number):
        raise NotImplementedError("implement in derived class")

    def set_egpio_mode(self, pin_number, mode, value=0.0):
        raise NotImplementedError("implement in derived class")

    def get_egpio_output_vector_vector(self):
        raise NotImplementedError("implement in derived class")

    def set_egpio_output_vector(self, output_vector, bit_mask):
        raise NotImplementedError("implement in derived class")

    def get_egpio_value(self, pin_number):
        raise NotImplementedError("implement in derived class")

    def set_egpio_value(self, pin_number, value):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeEEPROMFeatureInterface(_SeaBreezeFeatureInterface):
    def eeprom_read_slot(self, slot_number, strip_zero_bytes=False):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeLightSourceFeatureInterface(_SeaBreezeFeatureInterface):
    def get_count(self):
        raise NotImplementedError("implement in derived class")

    def has_enable(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def is_enabled(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def set_enable(self, light_source_index, enable):
        raise NotImplementedError("implement in derived class")

    def has_variable_intensity(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def get_intensity(self, light_source_index):
        raise NotImplementedError("implement in derived class")

    def set_intensity(self, light_source_index, intensity):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeStrobeLampFeatureInterface(_SeaBreezeFeatureInterface):
    def enable_lamp(self, state):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeContinuousStrobeFeatureInterface(_SeaBreezeFeatureInterface):
    def set_enable(self, strobe_enable):
        raise NotImplementedError("implement in derived class")

    def set_period_micros(self, period_micros):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeShutterFeatureInterface(_SeaBreezeFeatureInterface):
    def set_shutter_open(self, state):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeNonlinearityCoefficientsFeatureInterface(_SeaBreezeFeatureInterface):
    def get_nonlinearity_coefficients(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeTemperatureFeatureInterface(_SeaBreezeFeatureInterface):
    def count_temperatures(self):
        raise NotImplementedError("implement in derived class")

    def read_temperature(self, index):
        raise NotImplementedError("implement in derived class")

    def temperature_get_all(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeIntrospectionFeatureInterface(_SeaBreezeFeatureInterface):
    def number_of_pixels(self):
        raise NotImplementedError("implement in derived class")

    def get_active_pixel_ranges(self):
        raise NotImplementedError("implement in derived class")

    def get_optical_dark_pixel_ranges(self):
        raise NotImplementedError("implement in derived class")

    def get_electric_dark_pixel_ranges(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeSpectrumProcessingFeatureInterface(_SeaBreezeFeatureInterface):
    def get_boxcar_width(self):
        raise NotImplementedError("implement in derived class")

    def set_boxcar_width(self, boxcar_width):
        raise NotImplementedError("implement in derived class")

    boxcar_width = property(get_boxcar_width, set_boxcar_width)

    def get_scans_to_average(self):
        raise NotImplementedError("implement in derived class")

    def set_scans_to_average(self, scans_to_average):
        raise NotImplementedError("implement in derived class")

    scans_to_average = property(get_scans_to_average, set_scans_to_average)


class _SeaBreezeRevisionFeatureInterface(_SeaBreezeFeatureInterface):
    def hardware_revision(self):
        raise NotImplementedError("implement in derived class")

    def revision_firmware_get(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeOpticalBenchFeatureInterface(_SeaBreezeFeatureInterface):
    def get_fiber_diameter_microns(self):
        raise NotImplementedError("implement in derived class")

    def get_slit_width_microns(self):
        raise NotImplementedError("implement in derived class")

    def get_id(self):
        raise NotImplementedError("implement in derived class")

    def get_serial_number(self):
        raise NotImplementedError("implement in derived class")

    def get_coating(self):
        raise NotImplementedError("implement in derived class")

    def get_filter(self):
        raise NotImplementedError("implement in derived class")

    def get_grating(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeStrayLightCoefficientsFeatureInterface(_SeaBreezeFeatureInterface):
    def get_stray_light_coefficients(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeDataBufferFeatureInterface(_SeaBreezeFeatureInterface):
    def clear(self):
        raise NotImplementedError("implement in derived class")

    def remove_oldest_spectra(self, number_of_spectra):
        raise NotImplementedError("implement in derived class")

    def get_number_of_elements(self):
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity(self):
        raise NotImplementedError("implement in derived class")

    def set_buffer_capacity(self, capacity):
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity_maximum(self):
        raise NotImplementedError("implement in derived class")

    def get_buffer_capacity_minimum(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeFastBufferFeatureInterface(_SeaBreezeFeatureInterface):
    def get_buffering_enable(self):
        raise NotImplementedError("implement in derived class")

    def set_buffering_enable(self, is_enabled):
        raise NotImplementedError("implement in derived class")

    def get_consecutive_sample_count(self):
        raise NotImplementedError("implement in derived class")

    def set_consecutive_sample_count(self, consecutive_sample_count):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeAcquisitionDelayFeatureInterface(_SeaBreezeFeatureInterface):
    def set_delay_microseconds(self, delay_usec):
        raise NotImplementedError("implement in derived class")

    def get_delay_microseconds(self):
        raise NotImplementedError("implement in derived class")

    delay_microseconds = property(get_delay_microseconds, set_delay_microseconds)

    def get_delay_increment_microseconds(self):
        raise NotImplementedError("implement in derived class")

    def get_maximum_delay_microseconds(self):
        raise NotImplementedError("implement in derived class")

    def get_minimum_delay_microseconds(self):
        raise NotImplementedError("implement in derived class")


class _SeaBreezeI2CMasterFeatureInterface(_SeaBreezeFeatureInterface):
    def get_number_of_buses(self):
        raise NotImplementedError("implement in derived class")

    def read_bus(self, bus_index, slave_address, buffer_length=1024):
        raise NotImplementedError("implement in derived class")

    def write_bus(self, bus_index, slave_address, data):
        raise NotImplementedError("implement in derived class")
