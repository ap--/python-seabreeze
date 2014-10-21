
import seabreeze.sbcapi as csb
import numpy as np

IDX = 0

print "Opening spectrometer"
csb.open_spectrometer(IDX)

print "Opening spectrometer twice"
try:
    csb.open_spectrometer(IDX)
except csb.SeaBreezeError as e:
    print "!> %s" % e.message

print "Getting model"
model = csb.get_model(IDX)
print "> %s" % model

print "Set trigger mode"
csb.set_trigger_mode(IDX, 0)

print "Set integration time microseconds"
csb.set_integration_time_microsec(IDX, 17000)

print "Get minimum integration time"
mt = csb.get_min_integration_time_microsec(IDX)
print "> %d" % mt

print "Set shutter open"
try:
    csb.set_shutter_open(IDX, False)
except csb.SeaBreezeError as e:
    print "!> %s" % e.message

print "Set strobe"
csb.set_strobe_enable(IDX, False)

print "Get light source count"
try:
    csb.get_light_source_count(IDX)
except csb.SeaBreezeError as e:
    print "!> %s" % e.message

print "Set light source enable"
try:
    csb.set_light_source_enable(IDX, 0, False)
except csb.SeaBreezeError as e:
    print "!> %s" % e.message

print "read_eeprom slot 0"
es = csb.read_eeprom_slot(IDX, 0)
print "> %s" % es

print "Has irrad collection area"
hc = csb.has_irrad_collection_area(IDX)
print ">", hc

print "Read irrad_collection_area"
try:
    ar = csb.read_irrad_collection_area(IDX)
except csb.SeaBreezeError as e:
    print "!> %s" % e.message

print "Read TEC temperature"
try:
    temp = csb.read_tec_temperature(IDX)
    print "> %f" % temp
except csb.SeaBreezeError as e:
    print "!> %s" % e.message

print "Get unformatted spectrum"
ufs_len = csb.get_unformatted_spectrum_length(IDX)
out = np.zeros((ufs_len,), dtype=np.ubyte)
csb.get_unformatted_spectrum(IDX, out)
print out

print "Get formatted spectrum"
fs_len = csb.get_formatted_spectrum_length(IDX)
out = np.zeros((fs_len,), dtype=np.double)
csb.get_formatted_spectrum(IDX, out)
print out

print "Get wavelengths"
out = np.zeros((fs_len,), dtype=np.double)
csb.get_wavelengths(IDX, out)
print out

print "Read irrad calib"
out = np.zeros((fs_len,), dtype=np.float32)
csb.read_irrad_calibration(IDX, out)
print out

print "Get serial number"
sn = csb.get_serial_number(IDX)
print "> %s" % sn

print "Get dark pixel indices"
dpi = csb.get_electric_dark_pixel_indices(IDX)
print dpi

print "Get API version"
print ">", csb.get_api_version_string()









