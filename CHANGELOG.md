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

[Unreleased]: https://github.com/ap--/python-seabreeze/compare/v1.0.1...HEAD
[1.0.1]: https://github.com/ap--/python-seabreeze/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/ap--/python-seabreeze/compare/v1.0.0rc4...v1.0.0
[1.0.0rc4]: https://github.com/ap--/python-seabreeze/compare/v1.0.0rc3...v1.0.0rc4
[1.0.0rc3]: https://github.com/ap--/python-seabreeze/compare/v1.0.0rc2...v1.0.0rc3
[1.0.0rc2]: https://github.com/ap--/python-seabreeze/compare/python-seabreeze-v0.6.0...v1.0.0rc2
[0.6.0]: https://github.com/ap--/python-seabreeze/compare/python-seabreeze-v0.5.3...python-seabreeze-v0.6.0
[0.5.3]: https://github.com/ap--/python-seabreeze/tree/python-seabreeze-v0.5.3
