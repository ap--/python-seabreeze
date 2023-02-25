"""
SeaBreeze
=========

Python module for accessing Ocean Optics Spectrometers.
Returns spectra as NumPy Arrays and unifies the spectrometer
control for all supported spectrometer models.

How to use the module
---------------------
To measure a spectrum do::

  >>> import seabreeze
  >>> seabreeze.use("cseabreeze")
  >>> from seabreeze.spectrometers import Spectrometer
  >>> spec = Spectrometer.from_serial_number(serial=None)  # serial=None picks the first available
  >>> wavelengths = spec.wavelengths()
  >>> intensities = spec.intensities()

Available subpackages
---------------------
spectrometers
    Core interface to the spectrometers
backend
    internal backend loading functions
cseabreeze
    libseabreeze Cython wrapped backend
pyseabreeze
    pyusb implementation of the cseabreeze backend

Utilities
---------
utils
    -
system
    operating system related functionality

Operating System dependent setup
--------------------------------
Dependent on your operating system there's steps you have to take to make
the spectrometer visible to python-seabreeze.

Linux
    requires to install the udev rules
Osx
    should work ootb
Windows
    requires installing the drivers

"""
from seabreeze._version import __version__ as _version
from seabreeze.backends import use

__all__ = ["use"]
__version__ = _version
__author__ = "Andreas Poehlmann"
__email__ = "andreas@poehlmann.io"
