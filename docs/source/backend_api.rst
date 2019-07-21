#####################
Backend API Reference
#####################

.. _backend_api:



python-seabreeze provides two backend interfaces that allow you to communicate with
your spectrometers:

.. rubric:: cseabreeze

The `cseabreeze`_ backend is a wrapper around the seabreeze library
provided by Ocean Optics and is the default backend interface used by python-seabreeze.
But it's hard to extend if you're unfamiliar with C++ and cython.

.. rubric:: pyseabreeze

The `pyseabreeze`_ backend uses `pyUSB`_ to communicate with the spectrometer and implements
the whole communication layer for Ocean Optics spectrometers. (It implements the
``Ocean Optics Interface (???)`` and the ``Ocean Binary Protocol``) While `pyseabreeze`_ is
not as feature complete as `cseabreeze`_ it should be a lot easier to extend for someone
familiar with python.

.. _pyUSB: http://pyusb.github.io/pyusb/

-------------------------------------------------------------------------------

cseabreeze
==========

.. currentmodule:: seabreeze.cseabreeze

The C++ libseabreeze API is fully wrapped and exposed to python. If you intend to
directly use it you start with a :class:`SeaBreezeAPI` instance
and retrieve :class:`SeaBreezeDevice` instances via
:func:`SeaBreezeAPI.list_devices`. These instances offer all feature functionality via their
``features`` property or their ``f`` attribute for easy access.


SeaBreezeAPI
------------

.. autoclass:: SeaBreezeAPI

    .. automethod:: SeaBreezeAPI.list_devices

    .. automethod:: SeaBreezeAPI.add_rs232_device_location

    .. automethod:: SeaBreezeAPI.add_ipv4_device_location

    .. automethod:: SeaBreezeAPI.initialize

    .. automethod:: SeaBreezeAPI.shutdown

SeaBreezeDevice
---------------

.. autoclass:: SeaBreezeDevice
    :members:

    .. autoattribute:: SeaBreezeDevice.model

    .. autoattribute:: SeaBreezeDevice.serial_number

SeaBreezeFeatures
-----------------

.. hint::

    :class:`SeaBreezeFeature` classes are not instantiated directly, but provided via
    :attr:`SeaBreezeDevice.features` or :attr:`SeaBreezeDevice.f`. Their individual
    class interfaces are documented here:

.. autoclass:: SeaBreezeFeature
    :members:

.. autoclass:: SeaBreezeRawUSBBusAccessFeature
    :members:

.. autoclass:: SeaBreezeSpectrometerFeature
    :members:

.. autoclass:: SeaBreezePixelBinningFeature
    :members:

.. autoclass:: SeaBreezeThermoElectricFeature
    :members:

.. autoclass:: SeaBreezeIrradCalFeature
    :members:

.. autoclass:: SeaBreezeEthernetConfigurationFeature
    :members:

.. autoclass:: SeaBreezeMulticastFeature
    :members:

.. autoclass:: SeaBreezeIPv4Feature
    :members:

.. autoclass:: SeaBreezeDHCPServerFeature
    :members:

.. autoclass:: SeaBreezeNetworkConfigurationFeature
    :members:

.. autoclass:: SeaBreezeWifiConfigurationFeature
    :members:

.. autoclass:: SeaBreezeGPIOFeature
    :members:

.. autoclass:: SeaBreezeEEPROMFeature
    :members:

.. autoclass:: SeaBreezeLightSourceFeature
    :members:

.. autoclass:: SeaBreezeStrobeLampFeature
    :members:

.. autoclass:: SeaBreezeContinuousStrobeFeature
    :members:

.. autoclass:: SeaBreezeShutterFeature
    :members:

.. autoclass:: SeaBreezeNonlinearityCoefficientsFeature
    :members:

.. autoclass:: SeaBreezeTemperatureFeature
    :members:

.. autoclass:: SeaBreezeIntrospectionFeature
    :members:

.. autoclass:: SeaBreezeSpectrumProcessingFeature
    :members:

.. autoclass:: SeaBreezeRevisionFeature
    :members:

.. autoclass:: SeaBreezeOpticalBenchFeature
    :members:

.. autoclass:: SeaBreezeStrayLightCoefficientsFeature
    :members:

.. autoclass:: SeaBreezeDataBufferFeature
    :members:

.. autoclass:: SeaBreezeFastBufferFeature
    :members:

.. autoclass:: SeaBreezeAcquisitionDelayFeature
    :members:

.. autoclass:: SeaBreezeI2CMasterFeature
    :members:



pyseabreeze
===========

The
