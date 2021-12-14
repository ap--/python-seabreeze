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
``features`` property or their shorthand ``f`` property for easy access.


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

.. rubric:: Summary of all features

.. autosummary::

    SeaBreezeFeature
    SeaBreezeRawUSBBusAccessFeature
    SeaBreezeSpectrometerFeature
    SeaBreezePixelBinningFeature
    SeaBreezeThermoElectricFeature
    SeaBreezeIrradCalFeature
    SeaBreezeEthernetConfigurationFeature
    SeaBreezeMulticastFeature
    SeaBreezeIPv4Feature
    SeaBreezeDHCPServerFeature
    SeaBreezeNetworkConfigurationFeature
    SeaBreezeWifiConfigurationFeature
    SeaBreezeGPIOFeature
    SeaBreezeEEPROMFeature
    SeaBreezeLightSourceFeature
    SeaBreezeStrobeLampFeature
    SeaBreezeContinuousStrobeFeature
    SeaBreezeShutterFeature
    SeaBreezeNonlinearityCoefficientsFeature
    SeaBreezeTemperatureFeature
    SeaBreezeIntrospectionFeature
    SeaBreezeSpectrumProcessingFeature
    SeaBreezeRevisionFeature
    SeaBreezeOpticalBenchFeature
    SeaBreezeStrayLightCoefficientsFeature
    SeaBreezeDataBufferFeature
    SeaBreezeFastBufferFeature
    SeaBreezeAcquisitionDelayFeature
    SeaBreezeI2CMasterFeature

.. rubric:: Feature class interfaces

.. autoclass:: SeaBreezeFeature

    .. method:: get_feature_class_registry()

        return a class registry dictionary *internal use only*

        creates a dictionary of all derived classes of `SeaBreezeFeature` and
        returns a {SeaBreezeFeature.identifier: DerivedSeabreezeFeature} mapping.

    .. attribute:: feature_id

        assigned feature id for cseabreeze feature *internal use only*

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

.. currentmodule:: seabreeze.pyseabreeze

`pyseabreeze` is a compatible pure python implementation of `cseabreeze`. If you intend to
directly use it you start with a :class:`SeaBreezeAPI` instance
and retrieve :class:`SeaBreezeDevice` instances via
:func:`SeaBreezeAPI.list_devices`. These instances offer all feature functionality via their
``features`` property or their shorthand ``f`` property for easy access.


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

.. rubric:: Summary of all features

.. autosummary::

    SeaBreezeFeature
    SeaBreezeRawUSBBusAccessFeature
    SeaBreezeSpectrometerFeature
    SeaBreezePixelBinningFeature
    SeaBreezeThermoElectricFeature
    SeaBreezeIrradCalFeature
    SeaBreezeEthernetConfigurationFeature
    SeaBreezeMulticastFeature
    SeaBreezeIPv4Feature
    SeaBreezeDHCPServerFeature
    SeaBreezeNetworkConfigurationFeature
    SeaBreezeWifiConfigurationFeature
    SeaBreezeGPIOFeature
    SeaBreezeEEPROMFeature
    SeaBreezeLightSourceFeature
    SeaBreezeStrobeLampFeature
    SeaBreezeContinuousStrobeFeature
    SeaBreezeShutterFeature
    SeaBreezeNonlinearityCoefficientsFeature
    SeaBreezeTemperatureFeature
    SeaBreezeIntrospectionFeature
    SeaBreezeSpectrumProcessingFeature
    SeaBreezeRevisionFeature
    SeaBreezeOpticalBenchFeature
    SeaBreezeStrayLightCoefficientsFeature
    SeaBreezeDataBufferFeature
    SeaBreezeFastBufferFeature
    SeaBreezeAcquisitionDelayFeature
    SeaBreezeI2CMasterFeature

.. rubric:: Feature class interfaces

.. autoclass:: SeaBreezeFeature

    .. method:: get_feature_class_registry()

        return a class registry dictionary *internal use only*

        creates a dictionary of all derived classes of `SeaBreezeFeature` and
        returns a {SeaBreezeFeature.identifier: DerivedSeabreezeFeature} mapping.

    .. method:: supports_protocol(protocol)

        *internal use only*

    .. method:: specialize(**kwargs)

        *internal use only*

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
