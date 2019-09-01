# Python module for [Ocean Optics](http://www.oceanoptics.com/) spectrometers

[![Anaconda](https://anaconda.org/poehlmann/python-seabreeze/badges/version.svg)](https://anaconda.org/poehlmann/python-seabreeze)
[![CondaDownload](https://anaconda.org/poehlmann/python-seabreeze/badges/downloads.svg)](https://anaconda.org/poehlmann/python-seabreeze/files)
[![AppVeyor](https://img.shields.io/appveyor/ci/ap--/python-seabreeze.svg?label=win-whl&style=flat-square)](https://ci.appveyor.com/project/ap--/python-seabreeze)
[![Travis](https://img.shields.io/travis/ap--/python-seabreeze.svg?label=osx-whl&style=flat-square)](https://travis-ci.org/ap--/python-seabreeze)
[![Github All Releases](https://img.shields.io/github/downloads/ap--/python-seabreeze/total.svg?style=flat-square)](https://github.com/ap--/python-seabreeze/releases)
[![MIT license](http://img.shields.io/badge/license-MIT-yellowgreen.svg?style=flat-square)](http://opensource.org/licenses/MIT)
[![GitHub issues](https://img.shields.io/github/issues/ap--/python-seabreeze.svg?style=flat-square)](https://github.com/ap--/python-seabreeze/issues)
[![Paypal donate](http://img.shields.io/badge/paypal-donate-blue.svg?style=flat-square)](https://paypal.me/apoehlmann)

Python-seabreeze is the easy way to access your Ocean Optics spectrometers from
python. It wraps the [SeaBreeze](http://oceanoptics.com/product/seabreeze/)
library provided by Ocean Optics to communicate with the spectrometer. If
SeaBreeze is not available it can fallback to a pyUSB based reimplementation.
**This software is not associated with Ocean Optics. Use it at your own risk.**

## DOCUMENTATION ON RTD

You'll find the documentation here [https://python-seabreeze.readthedocs.io/en/latest/](https://python-seabreeze.readthedocs.io/en/latest/)


## Usage

The following example shows how simple it is to acquire a spectrum with
python-seabreeze through the model independent _Spectrometer_ class. For a more
detailed description read the [documentation](https://python-seabreeze.readthedocs.io/en/latest/):

```python
>>> from seabreeze.spectrometers import Spectrometer
>>> spec = Spectrometer.from_first_available()
>>> spec.integration_time_micros(20000)
>>> spec.wavelengths()
array([  340.32581   ,   340.70321186,   341.08058305, ...,  1024.84940994,
        1025.1300678 ,  1025.4106617 ])
>>> spec.intensities()
array([  1.58187931e+01,   2.66704852e+04,   6.80208103e+02, ...,
         6.53090172e+02,   6.35011552e+02,   6.71168793e+02])
```

To use the pyseabreeze backend simply run this before importing `seabreeze.spectrometers`.

```python
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
| USB650       |            | [Issue #47](https://github.com/ap--/python-seabreeze/issues/47) |
| SPARK        |     x      |      x      |


## Known Issues

- USB2000 spectrometers cause `Data transfer error` due to old firmware [Issue #48](https://github.com/ap--/python-seabreeze/issues/48)
- USB650 not supported [Issue #47](https://github.com/ap--/python-seabreeze/issues/47)
- No conda packages for armv6 (RPI version 1) [Issue #46](https://github.com/ap--/python-seabreeze/issues/46)

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


## Related Repositories

If you want me to add your project here, let me know. Happy to add it.

- [SeaBreeze](https://sourceforge.net/projects/seabreeze/) - Ocean Optics' SeaBreeze C library.
- [libseabreeze](https://github.com/ap--/libseabreeze) - github clone of the C library. _internal use only_ (has pre-built libraries if you know what you're doing)
- [python-seabreeze-feedstock](https://github.com/ap--/python-seabreeze) - anaconda feedstock for automated package deployment. _internal use only_
