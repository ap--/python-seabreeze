Troubleshooting
===============

.. _troubleshoot:

Some common issues you might have with seabreeze

No spectrometer is detected
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here's some possible causes for why your spectrometer is not detected:

1. You did not run `seabreeze_os_setup`

   .. code:: bash

       # please run the script
       seabreeze_os_setup

2. Your spectrometer is not supported by the backend you are using

   .. code:: python

       # check that it's supported
       >>> import seabreeze.cseabreeze as csb
       >>> api = csb.SeaBreezeAPI()
       >>> api.supported_models()
       [u'FlameNIR', ..., u'Ventana']
       >>> import seabreeze.pyseabreeze as psb
       >>> api = psb.SeaBreezeAPI()
       >>> api.supported_models()
       [u'Apex', ..., u'Ventana']

3. Your spectrometer is not working

   .. code:: bash

       # Test your spectrometer with the original software provided by OceanOptics
