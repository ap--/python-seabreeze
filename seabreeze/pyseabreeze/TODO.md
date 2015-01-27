# pyseabreeze todo-list

To have the exact same interface, and minimize code duplication, we'll implement the seabreeze interface in pyseabreeze. This requires implementing the following Features:

**Note:** In the SpectrometerFeature, get_wavelengths relies on the EEPromFeature for all spectrometers, that support the EEPromFeature. That's why we'll diverge from the seabreeze structure a little bit and seperate the WavelengthFeature from the SpectrometerFeature too.


### SpectrometerFeature
[ ] open_spectrometer(handle)
[ ] close_spectrometer(handle)
[ ] get_model(handle)
[ ] set_trigger_mode(handle, mode)
[ ] set_integration_time_microsec(handle, integration_time_micros)
[ ] get_min_integration_time_microsec(handle)
[ ] get_unformatted_spectrum(handle)
[ ] get_formatted_spectrum(handle)
[ ] get_unformatted_spectrum_length(handle)
[ ] get_formatted_spectrum_length(handle)
[ ] get_electric_dark_pixel_indices(handle)

### SerialNumberFeature
[ ] get_serial_number(handle)

### WavelengthFeature
[ ] get_wavelength_coefficients(handle)

### ShutterFeature
[ ] set_shutter_open(handle, opened)

### LightsourceFeature
[ ] get_light_source_count(handle)
[ ] set_light_source_enable(handle, light_index, enable)
[ ] set_light_source_intensity(handle, light_index, intensity)

### EEPROMFeature
[ ] read_eeprom_slot(handle, slot_number)
[ ] write_eeprom_slot(handle, slot_number, data)

### IrradCalFeature
[ ] read_irrad_calibration(handle)
[ ] write_irrad_calibration(handle, data)
[ ] has_irrad_collection_area(handle)
[ ] read_irrad_collection_area(handle)
[ ] write_irrad_collection_area(handle, area)

### TecFeature
[ ] read_tec_temperature(handle)
[ ] set_tec_temperature(handle, temperature_degrees_celsius)
[ ] set_tec_enable(handle, tec_enable)
[ ] set_tec_fan_enable(handle, tec_fan_enable)

### ContinousStrobeFeature
[ ] set_continuous_strobe_period_microsec(handle, strobe_id, period_usec)



# Feature Table

- **It** supported, implemented, tested
- **Iu** supported, implemented, but untested
- **Pt** supported, partially implemented, tested
- **Pu** supported, partially implemented, but untested
- **N** supported, but not yet implemented
- **X** not supported
- **?** don't know

| Device      | SpF | SNF | ShF | LSF | EEF | ICF | TeF | CSF |
|:------------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| USB2000     |  Pt |  It |  X  |  N  |  Pt |  N  |  X  |  X  |
| HR2000      |  N  |  N  |  X  |  N  |  N  |  X  |  X  |  X  |
| HR4000      |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  N  |
| HR2000PLUS  |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  N  |
| QE65000     |  N  |  N  |  X  |  N  |  N  |  N  |  N  |  N  |
| USB2000PLUS |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  N  |
| USB4000     |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  N  |
| NIRQUEST512 |  N  |  N  |  X  |  N  |  N  |  N  |  N  |  N  |
| NIRQUEST256 |  N  |  N  |  X  |  N  |  N  |  N  |  N  |  N  |
| MAYA2000PRO |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  X  |
| MAYA2000    |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  N  |
| TORUS       |  N  |  N  |  X  |  N  |  N  |  N  |  X  |  N  |
| APEX        |  N  |  N  |  X  |  X  |  N  |  N  |  X  |  X  |
| JAZ         |  N  |  N  |  X  |  N  |  N  |  X  |  X  |  X  |
| STS         |  N  |  N  |  N  |  X  |  X  |  N  |  X  |  N  |
| QEPRO       |  N  |  N  |  N  |  X  |  N  |  N  |  N  |  N  |
| VENTANA     |  N  |  N  |  X  |  N  |  X  |  X  |  N  |  X  |
| USB650      |  ?  |  ?  |  ?  |  ?  |  ?  |  ?  |  ?  |  ?  |






