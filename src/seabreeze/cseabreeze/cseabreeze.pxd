# distutils: language = c

# The cython wrapper now uses seabreezeapi.h instead of the obsolete seabreezewrapper.h

cdef extern:
    void sbapi_initialize()
    void sbapi_shutdown()
    # int sbapi_add_RS232_device_location(char *deviceTypeName, char *deviceBusPath, unsigned int baud)
    int sbapi_probe_devices()
    int sbapi_get_number_of_device_ids()
    int sbapi_get_device_ids(long *ids, unsigned int max_ids)
    bint sbapi_open_device(long id, int *error_code)
    void sbapi_close_device(long id, int *error_code)

    const char *sbapi_get_error_string(int error_code)
    int sbapi_get_device_type(long id, int *error_code, char *buffer, unsigned int length)

    int sbapi_get_number_of_serial_number_features(long deviceID, int *error_code)
    int sbapi_get_serial_number_features(long deviceID, int *error_code, long *features, int max_features)
    int sbapi_get_serial_number(long deviceID, long featureID, int *error_code, char *buffer, int buffer_length)

    int sbapi_get_number_of_spectrometer_features(long deviceID, int *error_code)
    int sbapi_get_spectrometer_features(long deviceID, int *error_code, long *features, int max_features)
    void sbapi_spectrometer_set_trigger_mode(long deviceID, long featureID, int *error_code, int mode) nogil
    void sbapi_spectrometer_set_integration_time_micros(long deviceID, long featureID, int *error_code, unsigned long integration_time_micros) nogil
    long sbapi_spectrometer_get_minimum_integration_time_micros(long deviceID, long featureID, int *error_code) nogil
    int sbapi_spectrometer_get_formatted_spectrum_length(long deviceID, long featureID, int *error_code) nogil
    int sbapi_spectrometer_get_formatted_spectrum(long deviceID, long featureID, int *error_code, double* buffer, int buffer_length) nogil
    int sbapi_spectrometer_get_unformatted_spectrum_length(long deviceID, long featureID, int *error_code) nogil
    int sbapi_spectrometer_get_unformatted_spectrum(long deviceID, long featureID, int *error_code, unsigned char *buffer, int buffer_length) nogil
    int sbapi_spectrometer_get_wavelengths(long deviceID, long featureID, int *error_code, double *wavelengths, int length) nogil
    int sbapi_spectrometer_get_electric_dark_pixel_count(long deviceID, long featureID, int *error_code) nogil
    int sbapi_spectrometer_get_electric_dark_pixel_indices(long deviceID, long featureID, int *error_code, int *indices, int length) nogil

    int sbapi_get_number_of_shutter_features(long deviceID, int *error_code)
    int sbapi_get_shutter_features(long deviceID, int *error_code, long *features, int max_features)
    void sbapi_shutter_set_shutter_open(long deviceID, long featureID, int *error_code, unsigned char opened)

    int sbapi_get_number_of_light_source_features(long deviceID, int *error_code)
    int sbapi_get_light_source_features(long deviceID, int *error_code, long *features, int max_features)
    int sbapi_light_source_get_count(long deviceID, long featureID, int *error_code)
    unsigned char sbapi_light_source_has_enable(long deviceID, long featureID, int *error_code, int light_source_index)
    unsigned char sbapi_light_source_is_enabled(long deviceID, long featureID, int *error_code, int light_source_index)
    void sbapi_light_source_set_enable(long deviceID, long featureID, int *error_code, int light_source_index, unsigned char enable)
    unsigned char sbapi_light_source_has_variable_intensity(long deviceID, long featureID, int *error_code, int light_source_index)
    double sbapi_light_source_get_intensity(long deviceID, long featureID, int *error_code, int light_source_index)
    void sbapi_light_source_set_intensity(long deviceID, long featureID, int *error_code, int light_source_index, double intensity)

    int sbapi_get_number_of_continuous_strobe_features(long deviceID, int *error_code)
    int sbapi_get_continuous_strobe_features(long deviceID, int *error_code, long *features, int max_features)
    void sbapi_continuous_strobe_set_continuous_strobe_enable(long deviceID, long featureID, int *error_code, unsigned char enable)
    void sbapi_continuous_strobe_set_continuous_strobe_period_micros(long deviceID, long featureID, int *error_code, unsigned long period_micros)

    int sbapi_get_number_of_eeprom_features(long deviceID, int *error_code)
    int sbapi_get_eeprom_features(long deviceID, int *error_code, long *features, int max_features)
    int sbapi_eeprom_read_slot(long deviceID, long featureID, int *error_code, int slot_number, unsigned char *buffer, int buffer_length)

    int sbapi_get_number_of_irrad_cal_features(long deviceID, int *error_code)
    int sbapi_get_irrad_cal_features(long deviceID, int *error_code, long *features, int max_features)
    int sbapi_irrad_calibration_read(long deviceID, long featureID, int *error_code, float *buffer, int buffer_length)
    int sbapi_irrad_calibration_write(long deviceID, long featureID, int *error_code, float *buffer, int buffer_length)
    int sbapi_irrad_calibration_has_collection_area(long deviceID, long featureID, int *error_code)
    float sbapi_irrad_calibration_read_collection_area(long deviceID, long featureID, int *error_code)
    void sbapi_irrad_calibration_write_collection_area(long deviceID, long featureID, int *error_code, float area)

    int sbapi_get_number_of_thermo_electric_features(long deviceID, int *error_code)
    int sbapi_get_thermo_electric_features(long deviceID, int *error_code, long *features, int max_features)
    double sbapi_tec_read_temperature_degrees_C(long deviceID, long featureID, int *error_code)
    void sbapi_tec_set_temperature_setpoint_degrees_C(long deviceID, long featureID, int *error_code, double temperature_degrees_celsius)
    void sbapi_tec_set_enable(long deviceID, long featureID, int *error_code, unsigned char tec_enable)

    int sbapi_get_number_of_lamp_features(long deviceID, int *error_code)
    int sbapi_get_lamp_features(long deviceID, int *error_code, long *features, int max_features)
    void sbapi_lamp_set_lamp_enable(long deviceID, long featureID, int *error_code, unsigned char lamp_enable)

    int sbapi_get_number_of_nonlinearity_coeffs_features(long deviceID, int *error_code)
    int sbapi_get_nonlinearity_coeffs_features(long deviceID, int *error_code, long *features, int max_features)
    int sbapi_nonlinearity_coeffs_get(long deviceID, long featureID, int *error_code, double *buffer, int max_length)

    int sbapi_get_number_of_stray_light_coeffs_features(long deviceID, int *error_code)
    int sbapi_get_stray_light_coeffs_features(long deviceID, int *error_code, long *features, int max_features)
    int sbapi_stray_light_coeffs_get(long deviceID, long featureID, int *error_code, double *buffer, int max_length)

    int sbapi_get_number_of_spectrum_processing_features(long deviceID, int *error_code)
    int sbapi_get_spectrum_processing_features(long deviceID, int *error_code, long *features, int max_features)
    unsigned short int sbapi_spectrum_processing_scans_to_average_get(long deviceID, long featureID, int *error_code) nogil
    unsigned char sbapi_spectrum_processing_boxcar_width_get(long deviceID, long featureID, int *error_code) nogil
    void sbapi_spectrum_processing_scans_to_average_set(long deviceID, long featureID, int *error_code, unsigned short int scansToAverage) nogil
    void sbapi_spectrum_processing_boxcar_width_set(long deviceID, long featureID, int *error_code, unsigned char boxcarWidth) nogil
