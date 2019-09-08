=====
tl;dr
=====

You know how to install python modules with binary extensions and you know how to
access usb devices from user space with your operating system of choice. Here's
all you need to know to read a spectrum:

.. code:: bash

    $ pip install seabreeze


Do the os specific setup:

.. admonition:: Note

    The `seabreeze_os_setup` script is installed with python-seabreeze.
    It installs `udev rules <https://github.com/ap--/python-seabreeze/blob/master/os_support/10-oceanoptics.rules>`_
    on linux and
    `spectrometer drivers <https://github.com/ap--/python-seabreeze/blob/master/os_support/windows-driver-files.zip>`_
    on windows

.. code:: bash

    $ seabreeze_os_setup


Access the spectrometer:

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
