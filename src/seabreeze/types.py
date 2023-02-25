"""seabreeze.types

common types used to teach mypy about seabreeze
"""
from __future__ import annotations

import sys
from typing import TYPE_CHECKING
from typing import Any
from typing import TypeVar

if sys.version_info >= (3, 8):
    from typing import Literal
    from typing import Protocol
    from typing import TypedDict
    from typing import runtime_checkable
else:
    from typing_extensions import Literal
    from typing_extensions import Protocol
    from typing_extensions import TypedDict
    from typing_extensions import runtime_checkable

if TYPE_CHECKING:
    import numpy as np
    from numpy.typing import NDArray

__all__ = [
    "SeaBreezeAPI",
    "SeaBreezeBackend",
    "SeaBreezeDevice",
    "SeaBreezeFeatureAccessor",
    "SeaBreezeFeatureDict",
]


@runtime_checkable
class SeaBreezeDevice(Protocol):
    serial_number: str
    model: str  # fixme
    is_open: bool

    features: SeaBreezeFeatureDict
    f: SeaBreezeFeatureAccessor

    def open(self) -> None:
        ...

    def close(self) -> None:
        ...


class SeaBreezeAPI(Protocol):
    """SeaBreeze API interface"""

    def __init__(self, initialize: bool = True, **_kwargs: Any) -> None:
        raise TypeError(f"Type {type(self).__name__} cannot be instantiated")

    def initialize(self) -> None:
        ...

    def shutdown(self) -> None:
        ...

    def add_rs232_device_location(
        self, device_type: str, bus_path: str, baudrate: int
    ) -> None:
        ...

    def add_ipv4_device_location(
        self, device_type: str, ip_address: str, port: int
    ) -> None:
        ...

    def list_devices(self) -> list[SeaBreezeDevice]:
        ...

    @staticmethod
    def supported_models() -> list[str]:
        ...


FT = TypeVar("FT", bound="SeaBreezeFeature")


class SeaBreezeFeature(Protocol):
    identifier: str

    def __repr__(self) -> str:
        ...

    @classmethod
    def get_feature_class_registry(cls) -> dict[str, FT]:
        ...

    @classmethod
    def supports_protocol(cls, protocol: Any) -> bool:
        ...

    @classmethod
    def specialize(
        cls: type[SeaBreezeFeature], model_name: str, **kwargs: str
    ) -> type[FT]:
        ...


class SeaBreezeAcquisitionDelayFeature(SeaBreezeFeature, Protocol):
    def set_delay_microseconds(self, delay_usec: int) -> None:
        ...

    def get_delay_microseconds(self) -> int:
        ...

    delay_microseconds: int

    def get_delay_increment_microseconds(self) -> int:
        ...

    def get_maximum_delay_microseconds(self) -> int:
        ...

    def get_minimum_delay_microseconds(self) -> int:
        ...


class SeaBreezeContinuousStrobeFeature(SeaBreezeFeature, Protocol):
    def set_enable(self, strobe_enable: bool) -> None:
        ...

    def set_period_micros(self, period_micros: int) -> None:
        ...


class SeaBreezeDataBufferFeature(SeaBreezeFeature, Protocol):
    def clear(self) -> None:
        ...

    def remove_oldest_spectra(self, number_of_spectra: int) -> None:
        ...

    def get_number_of_elements(self) -> int:
        ...

    def get_buffer_capacity(self) -> int:
        ...

    def set_buffer_capacity(self, capacity: int) -> None:
        ...

    def get_buffer_capacity_maximum(self) -> int:
        ...

    def get_buffer_capacity_minimum(self) -> int:
        ...


class SeaBreezeDHCPServerFeature(SeaBreezeFeature, Protocol):
    def get_address(self, interface_index: int) -> str:
        ...

    def set_address(self, interface_index: int, server_address: str) -> None:
        ...

    def get_enable_state(self, interface_index: int) -> bool:
        ...

    def set_enable_state(self, interface_index: int, enable_state: bool) -> None:
        ...


class SeaBreezeEEPROMFeature(SeaBreezeFeature, Protocol):
    def eeprom_read_slot(self, slot_number: int, strip_zero_bytes: bool = False) -> str:
        ...


class SeaBreezeEthernetConfigurationFeature(SeaBreezeFeature, Protocol):
    def get_mac_address(self, interface_index: int) -> str:
        ...

    def set_mac_address(self, interface_index: int, mac_address: str) -> None:
        ...

    def get_gbe_enable_status(self, interface_index: int) -> bool:
        ...

    def set_gbe_enable_status(self, interface_index: int, enable_state: bool) -> None:
        ...


class SeaBreezeFastBufferFeature(SeaBreezeFeature, Protocol):
    def get_buffering_enable(self) -> bool:
        ...

    def set_buffering_enable(self, is_enabled: bool) -> None:
        ...

    def get_consecutive_sample_count(self) -> int:
        ...

    def set_consecutive_sample_count(self, consecutive_sample_count: int) -> None:
        ...


class SeaBreezeGPIOFeature(SeaBreezeFeature, Protocol):
    modes: dict[str, int]

    def get_number_of_gpio_pins(self) -> int:
        ...

    def get_gpio_output_enable_vector(self) -> int:
        ...

    def set_gpio_output_enable_vector(
        self, output_enable_vector: int, bit_mask: int
    ) -> None:
        ...

    def get_gpio_value_vector(self) -> int:
        ...

    def set_gpio_value_vector(self, value_vector: int, bit_mask: int) -> None:
        ...

    def get_number_of_egpio_pins(self) -> int:
        ...

    def get_egpio_available_modes(self, pin_number: int) -> int:
        ...

    def get_egpio_current_mode(self, pin_number: int) -> int:
        ...

    def set_egpio_mode(self, pin_number: int, mode: int, value: float = 0.0) -> None:
        ...

    def get_egpio_output_vector_vector(self) -> int:
        ...

    def set_egpio_output_vector(self, output_vector: int, bit_mask: int) -> None:
        ...

    def get_egpio_value(self, pin_number: int) -> int:
        ...

    def set_egpio_value(self, pin_number: int, value: int) -> None:
        ...


class SeaBreezeI2CMasterFeature(SeaBreezeFeature, Protocol):
    def get_number_of_buses(self) -> int:
        ...

    def read_bus(
        self, bus_index: int, slave_address: int, buffer_length: int = 1024
    ) -> bytes:
        ...

    def write_bus(self, bus_index: int, slave_address: int, data: bytes) -> None:
        ...


class SeaBreezeIntrospectionFeature(SeaBreezeFeature, Protocol):
    def number_of_pixels(self) -> int:
        ...

    def get_active_pixel_ranges(self) -> tuple[tuple[int, int], ...]:
        ...

    def get_optical_dark_pixel_ranges(self) -> tuple[tuple[int, int], ...]:
        ...

    def get_electric_dark_pixel_ranges(self) -> tuple[tuple[int, int], ...]:
        ...


class SeaBreezeIPv4Feature(SeaBreezeFeature, Protocol):
    def get_dhcp_enable_state(self, interface_index: int) -> bool:
        ...

    def set_dhcp_enable_state(self, interface_index: int, is_enabled: bool) -> None:
        ...

    def get_number_of_ipv4_addresses(self, interface_index: int) -> int:
        ...

    def get_ipv4_address(self, interface_index: int, address_index: int) -> str:
        ...

    def get_default_gateway(self, interface_index: int) -> str:
        ...

    def set_default_gateway(
        self, interface_index: int, default_gateway_address: str
    ) -> None:
        ...

    def add_ipv4_address(self, interface_index: int, ipv4_address: str) -> None:
        ...

    def delete_ipv4_address(self, interface_index: int, address_index: int) -> None:
        ...


class SeaBreezeIrradCalFeature(SeaBreezeFeature, Protocol):
    def has_collection_area(self) -> bool:
        ...

    def read_collection_area(self) -> float:
        ...

    def write_collection_area(self, area: float) -> None:
        ...

    def read_calibration(self) -> NDArray[np.int_]:
        ...

    def write_calibration(self, calibration_array: NDArray[np.int_]) -> None:
        ...


class SeaBreezeLightSourceFeature(SeaBreezeFeature, Protocol):
    def get_count(self) -> int:
        ...

    def has_enable(self, light_source_index: int) -> bool:
        ...

    def is_enabled(self, light_source_index: int) -> bool:
        ...

    def set_enable(self, light_source_index: int, enable: bool) -> None:
        ...

    def has_variable_intensity(self, light_source_index: int) -> bool:
        ...

    def get_intensity(self, light_source_index: int) -> int:
        ...

    def set_intensity(self, light_source_index: int, intensity: int) -> None:
        ...


class SeaBreezeMulticastFeature(SeaBreezeFeature, Protocol):
    def get_multicast_enable_state(self, interface_index: int) -> bool:
        ...

    def set_multicast_enable_state(
        self, interface_index: int, enable_state: bool
    ) -> None:
        ...


class SeaBreezeNetworkConfigurationFeature(SeaBreezeFeature, Protocol):
    def get_number_of_network_interfaces(self) -> int:
        ...

    def get_network_interface_connection_type(self, interface_index: int) -> int:
        ...

    def get_network_interface_enable_state(self, interface_index: int) -> bool:
        ...

    def set_network_interface_enable_state(
        self, interface_index: int, enable_state: bool
    ) -> None:
        ...

    def run_network_interface_self_test(self, interface_index: int) -> bool:
        ...

    def save_network_interface_connection_settings(self, interface_index: int) -> None:
        ...


class SeaBreezeNonlinearityCoefficientsFeature(SeaBreezeFeature, Protocol):
    def get_nonlinearity_coefficients(self) -> list[float]:
        ...


class SeaBreezeOpticalBenchFeature(SeaBreezeFeature, Protocol):
    def get_fiber_diameter_microns(self) -> int:
        ...

    def get_slit_width_microns(self) -> int:
        ...

    def get_id(self) -> str:
        ...

    def get_serial_number(self) -> str:
        ...

    def get_coating(self) -> str:
        ...

    def get_filter(self) -> str:
        ...

    def get_grating(self) -> str:
        ...


class SeaBreezePixelBinningFeature(SeaBreezeFeature, Protocol):
    def set_binning_factor(self, factor: int) -> None:
        ...

    def get_binning_factor(self) -> int:
        ...

    def get_max_binning_factor(self) -> int:
        ...

    binning_factor: int
    max_binning_factor: int

    def set_default_binning_factor(self, factor: int) -> None:
        ...

    def get_default_binning_factor(self) -> int:
        ...

    default_binning_factor: int


class SeaBreezeRawUSBBusAccessFeature(SeaBreezeFeature, Protocol):
    def raw_usb_write(
        self, data: bytes, endpoint: Literal["primary_out", "secondary_out"]
    ) -> None:
        ...

    def raw_usb_read(
        self,
        endpoint: Literal["primary_in", "secondary_in", "secondary_in2"],
        buffer_length: int | None = None,
    ) -> bytes:
        ...


class SeaBreezeRevisionFeature(SeaBreezeFeature, Protocol):
    def hardware_revision(self) -> str:
        ...

    def revision_firmware_get(self) -> str:
        ...


class SeaBreezeShutterFeature(SeaBreezeFeature, Protocol):
    def set_shutter_open(self, state: bool) -> None:
        ...


class SeaBreezeSpectrumProcessingFeature(SeaBreezeFeature, Protocol):
    def get_boxcar_width(self) -> int:
        ...

    def set_boxcar_width(self, boxcar_width: int) -> None:
        ...

    boxcar_width: int

    def get_scans_to_average(self) -> int:
        ...

    def set_scans_to_average(self, scans_to_average: int) -> None:
        ...

    scans_to_average: int


class SeaBreezeStrayLightCoefficientsFeature(SeaBreezeFeature, Protocol):
    def get_stray_light_coefficients(self) -> list[float]:
        ...


class SeaBreezeStrobeLampFeature(SeaBreezeFeature, Protocol):
    def enable_lamp(self, state: bool) -> None:
        ...


class SeaBreezeTemperatureFeature(SeaBreezeFeature, Protocol):
    def count_temperatures(self) -> int:
        ...

    def read_temperature(self, index: int) -> float:
        ...

    def temperature_get_all(self) -> list[float]:
        ...


class SeaBreezeThermoElectricFeature(SeaBreezeFeature, Protocol):
    def read_temperature_degrees_celsius(self) -> float:
        ...

    def set_temperature_setpoint_degrees_celsius(self, temperature: float) -> None:
        ...

    def enable_tec(self, state: bool) -> None:
        ...


class SeaBreezeWifiConfigurationFeature(SeaBreezeFeature, Protocol):
    def get_wifi_mode(self, interface_index: int) -> int:
        ...

    def set_wifi_mode(self, interface_index: int, wifi_mode: int) -> None:
        ...

    def get_wifi_security_type(self, interface_index: int) -> int:
        ...

    def set_wifi_security_type(self, interface_index: int, security_type: int) -> None:
        ...

    def get_wifi_ssid(self, interface_index: int) -> str:
        ...

    def set_wifi_ssid(self, interface_index: int, ssid: str) -> None:
        ...

    def set_wifi_pass_phrase(self, interface_index: int, pass_phrase: str) -> None:
        ...


class SeaBreezeSpectrometerFeature(SeaBreezeFeature, Protocol):
    def set_trigger_mode(self, mode: int) -> None:
        ...

    def set_integration_time_micros(self, integration_time_micros: int) -> None:
        ...

    def get_integration_time_micros_limits(self) -> tuple[int, int]:
        ...

    def get_maximum_intensity(self) -> float:
        ...

    def get_electric_dark_pixel_indices(self) -> list[int]:
        ...

    _spectrum_length: int

    def get_wavelengths(self) -> NDArray[np.float_]:
        ...

    def get_intensities(self) -> NDArray[np.float_]:
        ...

    def _get_spectrum_raw(self) -> bytes:
        ...

    def get_fast_buffer_spectrum(self) -> Any:
        ...  # fixme


class SeaBreezeFeatureAccessor(Protocol):
    acquisition_delay: SeaBreezeAcquisitionDelayFeature
    continuous_strobe: SeaBreezeContinuousStrobeFeature
    data_buffer: SeaBreezeDataBufferFeature
    dhcp_server: SeaBreezeDHCPServerFeature
    eeprom: SeaBreezeEEPROMFeature
    ethernet_configuration: SeaBreezeEthernetConfigurationFeature
    fast_buffer: SeaBreezeFastBufferFeature
    gpio: SeaBreezeGPIOFeature
    i2c_master: SeaBreezeI2CMasterFeature
    introspection: SeaBreezeIntrospectionFeature
    ipv4: SeaBreezeIPv4Feature
    irrad_cal: SeaBreezeIrradCalFeature
    light_source: SeaBreezeLightSourceFeature
    multicast: SeaBreezeMulticastFeature
    network_configuration: SeaBreezeNetworkConfigurationFeature
    nonlinearity_coefficients: SeaBreezeNonlinearityCoefficientsFeature
    optical_bench: SeaBreezeOpticalBenchFeature
    pixel_binning: SeaBreezePixelBinningFeature
    raw_usb_bus_access: SeaBreezeRawUSBBusAccessFeature
    revision: SeaBreezeRevisionFeature
    shutter: SeaBreezeShutterFeature
    spectrometer: SeaBreezeSpectrometerFeature
    spectrum_processing: SeaBreezeSpectrumProcessingFeature
    stray_light_coefficients: SeaBreezeStrayLightCoefficientsFeature
    strobe_lamp: SeaBreezeStrobeLampFeature
    temperature: SeaBreezeTemperatureFeature
    thermo_electric: SeaBreezeThermoElectricFeature
    wifi_configuration: SeaBreezeWifiConfigurationFeature


class SeaBreezeFeatureDict(TypedDict):
    acquisition_delay: list[SeaBreezeAcquisitionDelayFeature]
    continuous_strobe: list[SeaBreezeContinuousStrobeFeature]
    data_buffer: list[SeaBreezeDataBufferFeature]
    dhcp_server: list[SeaBreezeDHCPServerFeature]
    eeprom: list[SeaBreezeEEPROMFeature]
    ethernet_configuration: list[SeaBreezeEthernetConfigurationFeature]
    fast_buffer: list[SeaBreezeFastBufferFeature]
    gpio: list[SeaBreezeGPIOFeature]
    i2c_master: list[SeaBreezeI2CMasterFeature]
    introspection: list[SeaBreezeIntrospectionFeature]
    ipv4: list[SeaBreezeIPv4Feature]
    irrad_cal: list[SeaBreezeIrradCalFeature]
    light_source: list[SeaBreezeLightSourceFeature]
    multicast: list[SeaBreezeMulticastFeature]
    network_configuration: list[SeaBreezeNetworkConfigurationFeature]
    nonlinearity_coefficients: list[SeaBreezeNonlinearityCoefficientsFeature]
    optical_bench: list[SeaBreezeOpticalBenchFeature]
    pixel_binning: list[SeaBreezePixelBinningFeature]
    raw_usb_bus_access: list[SeaBreezeRawUSBBusAccessFeature]
    revision: list[SeaBreezeRevisionFeature]
    shutter: list[SeaBreezeShutterFeature]
    spectrometer: list[SeaBreezeSpectrometerFeature]
    spectrum_processing: list[SeaBreezeSpectrumProcessingFeature]
    stray_light_coefficients: list[SeaBreezeStrayLightCoefficientsFeature]
    strobe_lamp: list[SeaBreezeStrobeLampFeature]
    temperature: list[SeaBreezeTemperatureFeature]
    thermo_electric: list[SeaBreezeThermoElectricFeature]
    wifi_configuration: list[SeaBreezeWifiConfigurationFeature]


class SeaBreezeBackend(Protocol):
    """SeaBreeze Backend interface"""

    _backend_: str
    _api_kwargs: dict[str, Any]

    SeaBreezeAPI: type[SeaBreezeAPI]
    SeaBreezeDevice: type[SeaBreezeDevice]
    SeaBreezeError: type[Exception]
    SeaBreezeFeature: type[SeaBreezeFeature]

    SeaBreezeAcquisitionDelayFeature: type[SeaBreezeAcquisitionDelayFeature]
    SeaBreezeContinuousStrobeFeature: type[SeaBreezeContinuousStrobeFeature]
    SeaBreezeDataBufferFeature: type[SeaBreezeDataBufferFeature]
    SeaBreezeDHCPServerFeature: type[SeaBreezeDHCPServerFeature]
    SeaBreezeEEPROMFeature: type[SeaBreezeEEPROMFeature]
    SeaBreezeEthernetConfigurationFeature: type[SeaBreezeEthernetConfigurationFeature]
    SeaBreezeFastBufferFeature: type[SeaBreezeFastBufferFeature]
    SeaBreezeGPIOFeature: type[SeaBreezeGPIOFeature]
    SeaBreezeI2CMasterFeature: type[SeaBreezeI2CMasterFeature]
    SeaBreezeIntrospectionFeature: type[SeaBreezeIntrospectionFeature]
    SeaBreezeIPv4Feature: type[SeaBreezeIPv4Feature]
    SeaBreezeIrradCalFeature: type[SeaBreezeIrradCalFeature]
    SeaBreezeLightSourceFeature: type[SeaBreezeLightSourceFeature]
    SeaBreezeMulticastFeature: type[SeaBreezeMulticastFeature]
    SeaBreezeNetworkConfigurationFeature: type[SeaBreezeNetworkConfigurationFeature]
    SeaBreezeNonlinearityCoefficientsFeature: type[
        SeaBreezeNonlinearityCoefficientsFeature
    ]
    SeaBreezeNumFeaturesError: type[Exception]
    SeaBreezeOpticalBenchFeature: type[SeaBreezeOpticalBenchFeature]
    SeaBreezePixelBinningFeature: type[SeaBreezePixelBinningFeature]
    SeaBreezeRawUSBBusAccessFeature: type[SeaBreezeRawUSBBusAccessFeature]
    SeaBreezeRevisionFeature: type[SeaBreezeRevisionFeature]
    SeaBreezeShutterFeature: type[SeaBreezeShutterFeature]
    SeaBreezeSpectrometerFeature: type[SeaBreezeSpectrometerFeature]
    SeaBreezeSpectrumProcessingFeature: type[SeaBreezeSpectrumProcessingFeature]
    SeaBreezeStrayLightCoefficientsFeature: type[SeaBreezeStrayLightCoefficientsFeature]
    SeaBreezeStrobeLampFeature: type[SeaBreezeStrobeLampFeature]
    SeaBreezeTemperatureFeature: type[SeaBreezeTemperatureFeature]
    SeaBreezeThermoElectricFeature: type[SeaBreezeThermoElectricFeature]
    SeaBreezeWifiConfigurationFeature: type[SeaBreezeWifiConfigurationFeature]
