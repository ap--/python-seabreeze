**This software is not associated with Ocean Optics. Use it at your own risk.**

**Info:** If this is not working for you, checkout [python-oceanoptics](https://github.com/ap--/python-oceanoptics).

windows wheels: [![Build status](https://ci.appveyor.com/api/projects/status/xxoqwq9nnl1if9rr?svg=true)](https://ci.appveyor.com/project/ap--/python-seabreeze)

## SeaBreeze python module ##

This is a python module for [OceanOptics](http://www.oceanoptics.com/) spectrometers. It has two different backends. One is using the [SeaBreeze](http://oceanoptics.com/product/seabreeze/) library, and the other one is going to emulate the seabreeze behaviour using **pyusb**.

## Backends ##

### cseabreeze ###

wraps the [SeaBreeze](http://oceanoptics.com/product/seabreeze/) library with [Cython](http://cython.org). It uses the _SeaBreeze 2.0 API_ ("SeaBreezeAPI.h" instead of "SeaBreezeWrapper.h"). If installed, it is the default backend for **python-seabreeze**.

Since the seabreeze-c-library is developed by OceanOptics, it should work nicely with all listed Spectrometers.

### pyseabreeze ###

is an implementation of **cseabreeze** using [pyusb](https://github.com/walac/pyusb) as a backend. It originated from [python-oceanoptics](https://github.com/ap--/python-oceanoptics). Since it doesn't require compilation, it's easily installable on any system.

It supports all basic spectrometer features (check [this list](seabreeze/pyseabreeze/TODO.md) for what works and what doesn't.)

### Supported Devices: ###

| Spectrometer | cseabreeze | pyseabreeze |
|:-------------|:----------:|:-----------:|
| HR2000       |     x      |      x      |
| HR2000PLUS   |     x      |      x      |
| HR4000       |     x      |      x      |
| JAZ          |     x      |      x      |
| MAYA2000     |     x      |      x      |
| MAYA2000PRO  |     x      |      x      |
| MAYALSL      |     x      |      x      |
| NIRQUEST256  |     x      |      x      |
| NIRQUEST512  |     x      |      x      |
| QE65000      |     x      |      x      |
| QE-PRO       |     x      |      x      |
| STS          |     x      |      x      |
| TORUS        |     x      |      x      |
| USB2000      |     x      |      x      |
| USB2000PLUS  |     x      |      x      |
| USB4000      |     x      |      x      |
| USB650       |            |      x      |

# Installation #

### Ubuntu 12.04 (and probably all other debian based distros) ###

If you know which backend you want to use, you can skip the installation of the other. cseabreeze is the recommended backend.

1. **cseabreeze backend** requires _(cython is optional, the provided C file was generated with 0.21.2)_:
   ```
   apt-get install build-essential python-numpy libusb-0.1-4 cython
   ```
   
   install seabreeze C-library:
   ```
   ./misc/install_libseabreeze.sh
   ```

2. **pyseabreeze backend** requires _(libusb-0.1-4, or anything supported by pyusb should also work)_:
   ```
   apt-get install python-numpy python-pip libusb-1.0-0
   pip install pyusb==1.0.0b1
   ```


3. **both** need the udev rules:
   ```
   ./misc/install_udev_rules.sh
   ```

4. **To install the python module**:
   ```
   python setup.py install
   ```

   Or if you don't want to install the C-library and the cseabreeze backend:
   ```
   python setup.py install --without-cseabreeze
   ```


### Windows ###

On windows you need to install the compiled C library and the python wheel. The wheels are compiled with Cython-0.22 against numpy-1.8.2.

1. Download and install the correct prebuilt libseabreeze installer from [here](https://github.com/ap--/libseabreeze/releases). **These installers are UNOFFICIAL!**

2. Make sure you have pip installed: ([how-to-install-pip-on-windows](http://stackoverflow.com/questions/4750806/how-to-install-pip-on-windows))

3. Then download the correct python-seabreeze wheel from this repository ([here](https://github.com/ap--/python-seabreeze/releases)) and install via
   ```
   pip install <filename of downloaded wheel>
   ```

# Usage #

With seabreeze C backend:

```{python}
>>> import seabreeze.spectrometers as sb
>>> devices = sb.list_devices()
>>> print devices
[<SeaBreezeDevice USB2000PLUS:USB2+H02749>, <SeaBreezeDevice USB2000PLUS:USB2+H02751>]
>>> spec = sb.Spectrometer(devices[0])
>>> spec.serial_number
'USB2+H02751'
>>> spec.model
'USB2000PLUS'
>>> spec.integration_time_microsec(12000)
>>> spec.wavelengths()
array([  340.32581   ,   340.70321186,   341.08058305, ...,  1024.84940994,
        1025.1300678 ,  1025.4106617 ])
>>> spec.intensities()
array([  1.58187931e+01,   2.66704852e+04,   6.80208103e+02, ...,
         6.53090172e+02,   6.35011552e+02,   6.71168793e+02])
>>>
```

For the pyusb seabreeze backend, you only need to run **use** before importing **seabreeze.spectrometers** (this is WIP):

```{python}
>>> import seabreeze
>>> seabreeze.use('pyseabreeze')
>>> import seabreeze.spectrometers as sb
>>>
```

### Interface: ###

Currently there is almost no documentation of the module, sorry... Look at the Spectrometer class in [spectrometers.py](seabreeze/spectrometers.py). This is the class interface for any spectrometer:
```{python}

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
```

# Contributing #

If you own any of the spectrometers, run on a different setup (Windows, MacOS, different Linux distribution, or a different Architecture) and it works for you, or doesn't: feel free to write me a mail, so that I can update the supported list or help. Also feel free to file issues or pull requests on github.

# License #

Files in this repository are released under the [MIT license](LICENSE.md).
