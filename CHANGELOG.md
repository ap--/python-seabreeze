# seabreeze changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

| label  | description                         |
|-------:|:------------------------------------|
| *docs* | module documentation related        |
| *csb*  | refers to `seabreeze.cseabreeze`    |
| *psb*  | refers to `seabreeze.pyseabreeze`   |
| *spec* | refers to `seabreeze.spectrometers` |


## [Unreleased]
...

## [2.9.2] - 2024-06-19
### Fixed
- *spec* lazy SeaBreezeError import did not work in try/except block

## [2.9.1] - 2024-06-19
### Fixed
- *spec* importing SeaBreezeError does not trigger a backend warning anymore

## [2.9.0] - 2024-06-08
### Added
- *psb* ethernet support for HDX (thanks @hperrey)

### Changed
- allow `seabreeze.use` to be called after importing `seabreeze.spectrometers`

## [2.8.0] - 2024-06-01
### Added
- *psb* HR2 support (thanks @gabrielbenedikt)
- *psb* Support for HR4 (thanks @MicheleCotrufo)

## [2.8.0] - 2024-06-01
### Added
- *psb* added support for HR2 spectrometer (pyseabreeze backend) (thanks @gabrielbenedikt)
- *psb* added support for HR4 spectrometer (pyseabreeze backend) (thanks @MicheleCotrufo)

## [2.7.0] - 2024-02-21
### Added
- *csb* added support for get_fast_buffer_spectrum() (cseabreeze backend) (thanks @padalev)

## [2.6.0] - 2024-01-24
### Changed
- *psb* Use currently set integration time for timeout instead of the maximum integration time.

### Added
- *psb* add support for SR6 spectrometers (thanks @knaugh)
- *psb* added thermo_electric feature to the list of feature classes in NIRQUEST512 (thanks @magnium)

## [2.5.0] - 2023-11-01
### Added
- *psb* add support for SR2 spectrometers (thanks @avedillopy)
- added zenodo batch for citing seabreeze

### Fixed
- fixed issue with cython>=3 when installing from source

### Changed
- dropped support for Python 3.7

## [2.4.0] - 2023-06-11
### Added
- *psb* add dark pixel indices for ST spectrometer

### Fixed
- *psb* attempt to fix overflow error for some pyseabreeze devices

## [2.3.0] - 2023-05-04
### Added
- *psb* basic support for ST spectrometers

## [2.2.0] - 2023-04-15
### Added
- *repo* build aarch64 and osx arm64 wheels
- *psb* basic support for SR4 spectrometers

## [2.1.0] - 2023-03-28
### Added
- *psb* allow configuration of OBP protocol version checks
- *psb* EXPERIMENTAL support for ADC1000-USB

### Fixed
- *repo* multiple pre-commit updates
- *psb* fixed minor static typing issues
- *repo* reduce CI runs on pull_requests

### Changed
- *repo* rename `master` branch to `main`

## [2.0.3] - 2022-05-09
### Fixed
- *csb* fix array type in write calibration

## [2.0.2] - 2021-01-21
### Fixed
- *psb* fix regression in seabreeze protocol

## [2.0.1] - 2021-01-20
### Changed
- strip debug symbols only on linux
- small seabreeze.types improvements

### Fixed
- *ci* don't build pypy wheels
- *psb* fix regression in seabreeze transport
- fix regression due to distutils deprecation fix on windows
- *ci* fix deployment action

## [2.0.0] - 2021-01-18
### Added
- added `seabreeze.types`
- fully type annotated seabreeze (improved autocompletion in IDEs)
- add pre-commit and pre-commit.ci

### Changed
- drop support for Python 2.7 to 3.6 (require py37+)
- expose selecting `pyusb_backend` via `seabreeze.use` for `pyseabreeze`
- *ci* switch to cibuildwheels
- stop including debugging symbols for `cseabreeze` (reduces wheels size)
- removed deprecated `seabreeze.compat` module

### Fixed
- compile correctly under newer g++
- prevent deprecation warning due to distutils deprecation

## [1.3.0] - 2020-08-23
### Added
- *psb* more informative error output for seabreeze_os_setup
- *psb* 1st level support for Flame-S
- *psb* support trigger mode 4 for USB2000+ and Flame-S
- *psb* nonlinearity feature for USB2000+ and Flame-S
- warn if seabreeze.use is called after importing seabreeze.spectrometers

### Changed
- *psb* *csb* QE-Pro support min integration time 8ms

## [1.2.0] - 2020-08-09
### Added
- *psb* added support for HDX Spectrometer (thanks @Lavikjo)
- added HDX to udev rules
- added QE-Pro to udev rules (thanks @Tyrannican)
- added yaqd-seabreeze to docs (thanks @ksunden)
- *docs* added raw usb example to feature docstring
- *psb* USBTransport.protocol property (for use when implementing new features)
- switched to black formatting
- add python-3.8 testing to github actions

### Changed
- *psb* OBP pyseabreeze device error handling changed
- switch to post-release version scheme for setuptools_scm

### Fixed
- improved handling of backends in pytests
- *psb* fix usb full/high speed handling
- *psb* fix multiple deprecation warnings
- *csb* fix device cleanup after api shutdown
- *psb* multiple fixes regarding device cleanup for all pyusb backends
- *psb* fix dark pixel correction
- *psb* fix max integration time for USB4000
- *psb* fix list_devices closes opened devices bug
- *csb* fix list_devices closes opened devices bug
- *csb* fix compile warnings on linux
- fix pytest handling on continuous integration servers
- potential numpy dependency fix for python>3.5 pkgs on piwheels

## [1.1.0] - 2020-05-22
### Added
- *psb* Continuous Strobe support for USB650 **thanks @moritzj29**
- *psb* Continuous Strobe implement set_period_micros
- *psb* Continuous Strobe support for USB2000+
- add python-3.8 support

### Fixed
- *docs* fix instructions to install pyseabreeze only

## [1.0.2] - 2020-05-04
### Fixed
- *csb* SeaBreezeTemperatureFeature return temperature as float
- *csb* SeaBreezeTemperatureFeature fix temperature_get_all
- fix udev rules by finalizing MODE setting

## [1.0.1] - 2019-11-16
### Fixed
- *spec* fix nonlinearity coefficients regression

### Changed
- *docs* update conda install instructions

## [1.0.0] - 2019-09-08
### Added
- *docs* install via [piwheels](https://www.piwheels.hostedpi.com/project/seabreeze/)

### Changed
- *docs* install via conda
- *docs* minor layout fixes and python-seabreeze vs seabreeze renaming

### Fixed
- `seabreeze_os_setup` fix py3 error when udev rules differed on linux
- fix links in changelog

## [1.0.0rc4] - 2019-09-07
### Added
- added this changelog

### Fixed
- *psb* fix `trigger_mode` for Spark
- *psb* fix `OBPProtocol` implementation for py3
- *psb* py2/py3 compatibility fix when reading serial number
- `seabreeze.backend` fix regression with error logging
- fixed test suites if backend not installed or not available on ci

## [1.0.0rc3] - 2019-09-04
### Added
- *docs* added note regarding pip install for prereleases

### Fixed
- *csb* fix wheels for OSX
- `seabreeze_os_setup` fix elevating to admin shell
- `seabreeze_os_setup` fix download of drivers on py27

## [1.0.0rc2] - 2019-09-01
### Added
- *docs* extensive documentation via sphinx on [readthedocs](https://python-seabreeze.readthedocs.io)
- `seabreeze_os_setup` entry point for doing udev rule (linux) and driver setup (windows)
- tests for basic module functionality and potentially connected spectrometers

### Changed
- complete rewrite of the backend interfaces
- *psb* refactored to be more easily extendable
- *csb* now wrapping the cpp interface of libseabreeze and exposing ALL features

## [0.6.0] - 2017-05-18
### Added
- initial docs for backends and public api
- install instructions for linux, osx and windows
- seabreeze.cseabreeze add spectrum_processing feature

### Fixed
- `seabreeze.backends` fix Exception.message deprecated
- *psb* fix nonlinearity\_coeffs feature if 0 coefficients defined

### Changed
- *csb* recompiled wrapper with newer cython

## [0.5.3] - 2016-02-16
### Added
- oldest version I'll go back to in this changelog

### Changed
- Don't require numpy during build of cseabreeze cython extension

[Unreleased]: https://github.com/ap--/python-seabreeze/compare/v2.9.2...HEAD
[2.9.2]: https://github.com/ap--/python-seabreeze/compare/v2.9.1...v2.9.2
[2.9.1]: https://github.com/ap--/python-seabreeze/compare/v2.9.0...v2.9.1
[2.9.0]: https://github.com/ap--/python-seabreeze/compare/v2.8.0...v2.9.0
[2.8.0]: https://github.com/ap--/python-seabreeze/compare/v2.7.0...v2.8.0
[2.7.0]: https://github.com/ap--/python-seabreeze/compare/v2.6.0...v2.7.0
[2.6.0]: https://github.com/ap--/python-seabreeze/compare/v2.5.0...v2.6.0
[2.5.0]: https://github.com/ap--/python-seabreeze/compare/v2.4.0...v2.5.0
[2.4.0]: https://github.com/ap--/python-seabreeze/compare/v2.3.0...v2.4.0
[2.3.0]: https://github.com/ap--/python-seabreeze/compare/v2.2.0...v2.3.0
[2.2.0]: https://github.com/ap--/python-seabreeze/compare/v2.1.0...v2.2.0
[2.1.0]: https://github.com/ap--/python-seabreeze/compare/v2.0.3...v2.1.0
[2.0.3]: https://github.com/ap--/python-seabreeze/compare/v2.0.2...v2.0.3
[2.0.2]: https://github.com/ap--/python-seabreeze/compare/v2.0.1...v2.0.2
[2.0.1]: https://github.com/ap--/python-seabreeze/compare/v2.0.0...v2.0.1
[2.0.0]: https://github.com/ap--/python-seabreeze/compare/v1.3.0...v2.0.0
[1.3.0]: https://github.com/ap--/python-seabreeze/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/ap--/python-seabreeze/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/ap--/python-seabreeze/compare/v1.0.2...v1.1.0
[1.0.2]: https://github.com/ap--/python-seabreeze/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/ap--/python-seabreeze/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/ap--/python-seabreeze/compare/v1.0.0rc4...v1.0.0
[1.0.0rc4]: https://github.com/ap--/python-seabreeze/compare/v1.0.0rc3...v1.0.0rc4
[1.0.0rc3]: https://github.com/ap--/python-seabreeze/compare/v1.0.0rc2...v1.0.0rc3
[1.0.0rc2]: https://github.com/ap--/python-seabreeze/compare/python-seabreeze-v0.6.0...v1.0.0rc2
[0.6.0]: https://github.com/ap--/python-seabreeze/compare/python-seabreeze-v0.5.3...python-seabreeze-v0.6.0
[0.5.3]: https://github.com/ap--/python-seabreeze/tree/python-seabreeze-v0.5.3
