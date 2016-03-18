# python-seabreeze documentation #

Currently only some features of the module are documented. Look at the
Spectrometer class in [spectrometers.py](../seabreeze/spectrometers.py) for
more detailed information.

#### class SeabreezeDevice

This class abstracts the device handling. You'll never create your own
instances of a SeabreezeDevice. Calling `list_devices()` returns one instance
for every connected spectrometer. You can use the `SeabreezeDevice.serial` and
`SeabreezeDevice.model` properties to filter through them.

```python
import seabreeze.spectrometers as sb
devices = sb.list_devices()
# select all USB2000+ spectrometers
usb2000plus = [d for d in devices if d.model == "USB2000PLUS"]
```

#### class Spectrometer

This is the most important class of the module. You interface any spectrometer
through this class. Instanciate it by providing a SeabreezeDevice:

```python
import seabreeze.spectrometers as sb
devices = sb.list_devices()
spec = sb.Spectrometer(devices[0])
```

or by specifying the serial number of your spectrometer:

```python
import seabreeze.spectrometers as sb
spec = sb.Spectrometer.from_serial_number("USB2+H02749")
```

The basic features that you might need are:

```python
# serial number
print spec.serial_number
# model
print spec.model
# number of pixels (as returned by seabreeze)
print spec.pixels
# set the integration time in microseconds
spec.integration_time_micros(10000)
# return an array containing all wavelengths
spec.wavelengths()
# return the newest aquired spectrum (with dark count and nonlinearity correction)
spec.intensities(correct_dark_counts=True, correct_nonlinearity=True)
```


#### The documentation still needs work

```python
class Spectrometer(object):
    def __init__(self, device):
    @classmethod
    def from_serial_number(cls, serial=None):
    def wavelengths(self):
    def intensities(self, correct_dark_counts=False, correct_nonlinearity=False):
    def spectrum(self, correct_dark_counts=False, correct_nonlinearity=False):
    def integration_time_micros(self, integration_time_micros):
    def trigger_mode(self, mode):
    @property
    def serial_number(self):
    @property
    def model(self):
    @property
    def pixels(self):
    @property
    def minimum_integration_time_micros(self):
    @property
    def light_sources(self):
    def eeprom_read_slot(self, slot):
    def tec_set_enable(self, enable):
    def tec_set_temperature_C(self, set_point_C):
    def tec_get_temperature_C(self):
    def lamp_set_enable(self, enable):
    def shutter_set_open(self, state):
    def stray_light_coeffs(self):
    def irrad_calibration(self):
    def irrad_calibration_collection_area(self):
    def continuous_strobe_set_enable(self, enable):
    def continuous_strobe_set_period_micros(self, period_micros):
    def close(self):
    def __enter__(self):
    def __exit__(self, type, value, traceback):
    def __repr__(self):

class LightSource(object):
    def __init__(self, device, featureId, index):
    def set_enable(self, enable):
    def set_intensity(self, intensity):
    def get_intensity(self):
    def __repr__(self):
```
