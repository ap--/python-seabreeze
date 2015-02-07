# pyseabreeze todo-list

To have the exact same interface, and minimize code duplication, we'll implement the seabreeze interface in pyseabreeze. This requires implementing the following Features:

## Features

Feel free to implement any of the missing ones! Or if you have one of the untested spectrometers report back if it works.

### Library specific
- [x] initialize
- [x] shutdown

### DeviceFeatures
- [x] device_list_devices
- [x] device_open
- [x] device_is_open
- [x] device_close
- [x] device_get_model
- [x] device_get_serial_number
- [x] device_get_spectrometer_feature_id
- [x] device_get_shutter_feature_id
- [x] device_get_light_source_feature_id
- [x] device_get_continuous_strobe_feature_id
- [x] device_get_eeprom_feature_id
- [x] device_get_irrad_calibration_feature_id
- [x] device_get_tec_feature_id
- [x] device_get_lamp_feature_id
- [x] device_get_nonlinearity_coeffs_feature_id
- [x] device_get_stray_light_coeffs_feature_id

### SpectrometerFeature
- [x] spectrometer_set_trigger_mode
- [x] spectrometer_set_integration_time_micros
- [x] spectrometer_get_minimum_integration_time_micros
- [x] spectrometer_get_formatted_spectrum_length
- [x] spectrometer_get_formatted_spectrum
- [x] spectrometer_get_unformatted_spectrum_length
- [x] spectrometer_get_unformatted_spectrum
- [x] spectrometer_get_wavelengths
- [x] spectrometer_get_electric_dark_pixel_indices

### ShutterFeature (this seems obsolete)
- [ ] shutter_set_shutter_open

### LightsourceFeature
- [ ] light_source_get_count
- [ ] light_source_has_enable
- [ ] light_source_is_enabled
- [ ] light_source_set_enable
- [ ] light_source_has_variable_intensity
- [ ] light_source_get_intensity
- [ ] light_source_set_intensity

### EEPROMFeature
- [x] eeprom_read_slot

### IrradCalFeature
- [ ] irrad_calibration_read
- [ ] irrad_calibration_write
- [ ] irrad_calibration_has_collection_area
- [ ] irrad_calibration_read_collection_area
- [ ] irrad_calibration_write_collection_area

### TecFeature
- [x] tec_read_temperature_degrees_C
- [x] tec_set_temperature_setpoint_degrees_C
- [x] tec_set_enable

### ContinousStrobeFeature
- [ ] continuous_strobe_set_enable
- [ ] continuous_strobe_set_period_micros

### LampFeature
- [ ] lamp_set_lamp_enable

### NonlinearityFeature
- [x] nonlinearity_coeffs_get

### StrayLightFeature
- [ ] stray_light_coeffs_get


# Feature Table

- **It** supported, implemented, tested
- **Iu** supported, implemented, but untested
- **Pt** supported, partially implemented, tested
- **Pu** supported, partially implemented, but untested
- **N** supported, but not yet implemented
- **X** not supported
- **?** don't know

| Device      | Dev | Spec| Shut| LS  | EE  | IC  | Tec | CS  | Lamp|  NL |  SL |
|:------------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| USB2000     |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  X  |  X  |  Iu |  X  |
| HR2000      |  Iu |  Iu |  X  |  N  |  Iu |  X  |  X  |  X  |  X  |  Iu |  X  |
| HR4000      |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  N  |  X  |  Iu |  X  |
| USB650      |  Iu |  Iu |  ?  |  ?  |  Iu |  ?  |  ?  |  ?  |  ?  |  Iu |  ?  |
| HR2000PLUS  |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  N  |  X  |  Iu |  X  |
| QE65000     |  Iu |  Iu |  X  |  N  |  Iu |  N  |  Iu |  N  |  X  |  Iu |  X  |
| USB2000PLUS |  It |  It |  X  |  N  |  It |  N  |  X  |  N  |  X  |  It |  X  |
| USB4000     |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  N  |  X  |  Iu |  X  |
| NIRQUEST512 |  Iu |  Iu |  X  |  N  |  Iu |  N  |  Iu |  N  |  X  |  Iu |  X  |
| NIRQUEST256 |  Iu |  Iu |  X  |  N  |  Iu |  N  |  Iu |  N  |  X  |  Iu |  X  |
| MAYA2000PRO |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  X  |  X  |  Iu |  X  |
| MAYA2000    |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  N  |  X  |  Iu |  X  |
| TORUS       |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  N  |  X  |  Iu |  X  |
| APEX        |  Iu |  Iu |  X  |  X  |  Iu |  N  |  X  |  X  |  X  |  Iu |  X  |
| MAYALSL     |  Iu |  Iu |  X  |  N  |  Iu |  N  |  X  |  N  |  X  |  Iu |  X  |
| JAZ         |  Iu |  Iu |  X  |  N  |  Iu |  X  |  X  |  X  |  X  |  Iu |  X  |
| STS         |  Iu |  Iu |  x  |  X  |  X  |  N  |  X  |  N  |  x  |  Iu |  x  |
| QEPRO       |  Iu |  Iu |  X  |  X  |  x  |  N  |  Iu |  N  |  X  |  Iu |  X  |
| VENTANA     |  Iu |  Iu |  X  |  N  |  X  |  X  |  Iu |  X  |  X  |  Iu |  X  |


