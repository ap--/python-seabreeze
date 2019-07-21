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

The C++ libseabreeze API is fully wrapped and exposed to python. If you intend to
directly use it you start with a :class:`seabreeze.cseabreeze.SeaBreezeAPI` instance
and retrieve :class:`seabreeze.cseabreeze.SeaBreezeDevice` instances via
:func:`seabreeze.cseabreeze.SeaBreezeAPI.list_devices`. These instances offer all feature functionality via their
``features`` property or their ``f`` attribute for easy access.


SeaBreezeAPI
------------

.. autoclass:: seabreeze.cseabreeze.SeaBreezeAPI

    .. automethod:: seabreeze.cseabreeze.SeaBreezeAPI.list_devices

    .. automethod:: seabreeze.cseabreeze.SeaBreezeAPI.add_rs232_device_location

    .. automethod:: seabreeze.cseabreeze.SeaBreezeAPI.add_ipv4_device_location

    .. automethod:: seabreeze.cseabreeze.SeaBreezeAPI.initialize

    .. automethod:: seabreeze.cseabreeze.SeaBreezeAPI.shutdown

SeaBreezeDevice
---------------

.. autoclass:: seabreeze.cseabreeze.SeaBreezeDevice
    :members:

SeaBreezeFeatures
-----------------

.. autoclass:: seabreeze.cseabreeze.SeaBreezeFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeRawUSBBusAccessFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeSpectrometerFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezePixelBinningFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeThermoElectricFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeIrradCalFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeEthernetConfigurationFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeMulticastFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeIPv4Feature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeDHCPServerFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeNetworkConfigurationFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeWifiConfigurationFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeGPIOFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeEEPROMFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeLightSourceFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeStrobeLampFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeContinuousStrobeFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeShutterFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeNonlinearityCoefficientsFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeTemperatureFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeIntrospectionFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeSpectrumProcessingFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeRevisionFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeOpticalBenchFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeStrayLightCoefficientsFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeDataBufferFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeFastBufferFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeAcquisitionDelayFeature
    :members:

.. autoclass:: seabreeze.cseabreeze.SeaBreezeI2CMasterFeature
    :members:



pyseabreeze
===========

The
