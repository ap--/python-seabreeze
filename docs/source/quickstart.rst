Quickstart Guide
================



TL;DR;
------

.. code-block:: bash

    conda install -c poehlmann python-seabreeze

.. admonition:: if windows

    force your spectrometer to use a driver from :module:`misc/windows-driver-files.zip`

.. admonition:: if linux

    install udev rules from :module:`misc/10-oceanoptics.rules`


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



