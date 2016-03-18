# Python module for [OceanOptics](http://www.oceanoptics.com/) spectrometers

[![AppVeyor](https://img.shields.io/appveyor/ci/ap--/python-seabreeze.svg?label=windows)](https://ci.appveyor.com/project/ap--/python-seabreeze)
[![Travis](https://img.shields.io/travis/ap--/python-seabreeze.svg?label=macosx)](https://travis-ci.org/ap--/python-seabreeze)
[![Github All Releases](https://img.shields.io/github/downloads/ap--/python-seabreeze/total.svg)]()
[![MIT license](http://img.shields.io/badge/license-MIT-yellowgreen.svg)](http://opensource.org/licenses/MIT)
[![GitHub issues](https://img.shields.io/github/issues/ap--/python-seabreeze.svg)](https://github.com/ap--/python-seabreeze/issues)
[![Paypal donate](http://img.shields.io/badge/paypal-donate-blue.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=4W26DFASSCE9A)

Python-seabreeze is the easy way to access your OceanOptics spectrometers from
python. It wraps the [SeaBreeze](http://oceanoptics.com/product/seabreeze/)
library provided by OceanOptics to communicate with the spectrometer. If
SeaBreeze is not available it can fallback to a pyUSB based reimplementation.
**This software is not associated with OceanOptics. Use it at your own risk.**

## Requirements

- [NumPy](http://www.numpy.org)
- One of the backend libraries:
  - [SeaBreeze - Embedded Open-Source Device Driver](http://oceanoptics.com/products/seabreeze/)
  - [pyUSB==1.0.0b2](https://walac.github.io/pyusb/) 

## Installation

Python-seabreeze is available for Python versions _2.7.x_, _3.3.x_, _3.4.x_ and
_3.5.x_ on all platforms. Before installing the module, you need to decide
which backend you want to use and install it. Your options are the cseabreeze
backend (recommended) and the pyseabreeze backend (click
[here](docs/BACKENDS.md) for more information on the backends).  Please follow
the installation instructions for your platform.

* [Linux](docs/LINUX_INSTALL.md)
* [MacOSX](docs/MACOSX_INSTALL.md)
* [Windows](docs/WINDOWS_INSTALL.md)

I have confirmation from different people, that this module runs on several
different Linux distributions, on different OSX versions and on Windows (from
XP up to 8, I haven't heard from someone who uses 10 yet). It also runs on
different architectures x86, x64 and arm. The installation instructions
provided here are detailed enough to get the module up and running. If you
think that you can't install this module because there is something wrong with
it, read through the installation instructions again. Then try to look for a
solution for your problem on the internet at least three times. If nothing
helped, read the contributing guidelines, file an issue on github and be nice.
Please note, that I am not an OceanOptics employee and am maintaining this
module in my free time.


## Usage

The following example shows how simple it is to acquire a spectrum with
python-seabreeze through the model independent _Spectrometer_ class. For a more
detailed description read the [documentation](docs/DOCUMENTATION.md).:

```{python}
>>> import seabreeze.spectrometers as sb
>>> devices = sb.list_devices()
>>> print devices
[<SeaBreezeDevice USB2000PLUS:USB2+H02749>, <SeaBreezeDevice USB2000PLUS:USB2+H02751>]
>>> spec = sb.Spectrometer(devices[0])
>>> spec.integration_time_micros(12000)
>>> spec.wavelengths()
array([  340.32581   ,   340.70321186,   341.08058305, ...,  1024.84940994,
        1025.1300678 ,  1025.4106617 ])
>>> spec.intensities()
array([  1.58187931e+01,   2.66704852e+04,   6.80208103e+02, ...,
         6.53090172e+02,   6.35011552e+02,   6.71168793e+02])
```

To use the pyseabreeze backend simply run this before importing seabreeze.spectrometers:

```{python}
>>> import seabreeze
>>> seabreeze.use('pyseabreeze')
>>> import seabreeze.spectrometers as sb
```

## Supported Devices

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
| SPARK        |     x      |             |


## Contributing Guidelines

If you run into any problems, file an issue and be sure to include the
following in your report:

- Operating system (Linux distribution, Windows version, OSX version) and
  archictecture (32bit, 64bit, arm)
- Python version and arch (i.e. Python 2.7.10 64bit)
- python-seabreeze version

If you want a feature implemented, please file an issue, or create a pull
request when you implement it yourself. And if you would like to support me via
paypal, click on the paypal donate button on top of this README.

 
## License

Files in this repository are released under the [MIT license](LICENSE.md).

