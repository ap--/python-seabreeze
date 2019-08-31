Installing python-seabreeze
===========================

.. _install-seabreeze:

Install via pip
---------------

.. Attention::
    Not working yet. Need publish new wheels for `seabreeze` on pypi first.

.. code:: bash

    # install only cseabreeze backend
    pip install seabreeze
    # install both cseabreeze and pyseabreeze backend
    pip install seabreeze[pyseabreeze]
    # install only pyseabreeze backend
    pip install seabreeze[pyseabreeze] --no-build-isolation --install-option="--without-cseabreeze"


Install via conda
-----------------

.. Attention::
    Not working yet. Need to add `conda-forge` or continue to build own packages (maybe via travis too?)


Operating System dependent setup
--------------------------------

Additional installation steps required by the specific operating system should be done by
running the script provided by python-seabreeze. After installing via pip, run:

.. code:: bash

    $ seabreeze_os_setup

For some additional information see
`os_support/readme.md <https://github.com/ap--/python-seabreeze/blob/master/os_support/readme.md>`_.
