python-seabreeze
================

python-seabreeze is the easy way to access your Ocean Optics spectrometers from
python. It wraps the `SeaBreeze`_ library provided by Ocean Optics to communicate
with the spectrometer. If SeaBreeze is not available it can fallback to a `pyUSB`_
based reimplementation.

.. _SeaBreeze: http://oceanoptics.com/product/seabreeze/
.. _pyUSB: http://pyusb.github.io/pyusb/

.. Attention::
    This software is not associated with Ocean Optics.
    Use it at your own risk.

It's recommended to read the :ref:`Installing python-seabreeze section <install-seabreeze>`,
the :ref:`Quickstart Guide <quickstart>` and the :ref:`API documentation <api>`. Also check
out the :ref:`Contributing Guide <contrib>` if you want to learn how to contribute to the project.


TL;DR;
------

If you know how to install python modules with binary extensions and you know how to
access usb devices from user space with your operating system of choice, here's
all you need to know to read a spectrum::

    $ pip install python-seabreeze

.. admonition:: if windows

    force your spectrometer to use a driver from :module:`system/windows-driver-files.zip`

.. admonition:: if linux

    install udev rules from :module:`system/10-oceanoptics.rules`

.. code-block:: python

    >>> import seabreeze.spectrometers as sb
    >>> spec = sb.Spectrometer.from_serial_number()
    >>> spec.integration_time_micros(20000)
    >>> spec.wavelengths()
    array([  340.32581   ,   340.70321186,   341.08058305, ...,  1024.84940994,
            1025.1300678 ,  1025.4106617 ])
    >>> spec.intensities()
    array([  1.58187931e+01,   2.66704852e+04,   6.80208103e+02, ...,
             6.53090172e+02,   6.35011552e+02,   6.71168793e+02])




.. toctree::
   :maxdepth: 3
   :caption: Contents:

   install.rst
   quickstart.rst
   api.rst
   contributing.rst

Indices and tables
==================
* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
