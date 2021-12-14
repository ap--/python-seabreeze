#############
API Reference
#############

.. _api:

The public api of `seabreeze` is provided by the `seabreeze.spectrometers` submodule.
The basic features to acquire a spectrum are provided for all spectrometer models independent
of the backend.

spectrometers
=============

.. currentmodule:: seabreeze.spectrometers


list_devices
------------

Provides a list of available instances of `SeaBreezeDevice`

.. autofunction:: list_devices

Spectrometer
------------

The `Spectrometer` class is used to access spectrometer features.

.. autoclass:: Spectrometer
    :members:
    :member-order: bysource

    .. automethod:: __init__


SeaBreezeError
--------------

`SeaBreezeError` is the default exception raised by the backend libraries.

.. autoclass:: SeaBreezeError
