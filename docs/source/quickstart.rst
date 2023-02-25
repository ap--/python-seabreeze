Quickstart Guide
================

.. _quickstart:

(Optional) Select a backend (defaults to cseabreeze):

.. code:: python

    # explicitly request cseabreeze
    >>> import seabreeze
    >>> seabreeze.use('cseabreeze')
    >>> from seabreeze.spectrometers import list_devices, Spectrometer

    # explicitly request pyseabreeze
    >>> import seabreeze
    >>> seabreeze.use('pyseabreeze')
    >>> from seabreeze.spectrometers import list_devices, Spectrometer


Open a spectrometer:

.. code:: python

    # open a spectrometer
    # ===================

    # option 1
    >>> from seabreeze.spectrometers import Spectrometer
    >>> spec = Spectrometer.from_first_available()
    >>> spec
    <Spectrometer USB2000PLUS:USB2+F01234>

    # option 2
    >>> from seabreeze.spectrometers import Spectrometer, list_devices
    >>> devices = list_devices()
    >>> devices
    [<SeaBreezeDevice USB2000PLUS:USB2+F01234>, <SeaBreezeDevice SPARK:00001>]
    >>> spec = Spectrometer(devices[1])
    >>> spec
    <Spectrometer SPARK:00001>

    # option 3
    >>> from seabreeze.spectrometers import Spectrometer
    >>> spec = Spectrometer.from_serial_number("F01234")
    >>> spec
    <Spectrometer USB2000PLUS:USB2+F01234>


Acquire a spectrum:

.. code:: python

    >>> spec = Spectrometer.from_serial_number("F01234")

    # set integration time
    >>> spec.integration_time_micros(100000)  # 0.1 seconds

    # get wavelengths
    >>> wavelengths = spec.wavelengths()
    >>> wavelengths
    array([356.70501709, 357.06904128, 357.43303326, ..., 711.60684758,
       711.93799556, 712.26911135])

    # get intensities
    >>> intensities = spec.intensities()
    >>> intensities
    array([1485., 1493., 1487., ..., 1496., 1497., 1493.])
