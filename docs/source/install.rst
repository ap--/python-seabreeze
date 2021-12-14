Installing seabreeze
====================

.. _install-seabreeze:

Installing via pip
------------------

.. code:: bash

    # install only cseabreeze backend
    pip install seabreeze
    # install both cseabreeze and pyseabreeze backend
    pip install seabreeze[pyseabreeze]
    # install only pyseabreeze backend
    pip install seabreeze[pyseabreeze] --no-use-pep517 --no-build-isolation --install-option="--without-cseabreeze"


Installing via conda
--------------------

.. code:: bash

    # install via conda
    conda install -c conda-forge seabreeze

.. note::
    seabreeze is now on `conda-forge`. The `poehlmann` channel will still be available for a
    while, but slowly phased out.

.. attention::
    - Before version 1 conda packages for seabreeze were named *python-seabreeze*. This has now
      changed to be consistent with the pypi package naming.
    - It's recommended to install the conda-forge package when in a conda environment, instead
      of installing via pip.

Building seabreeze manually
---------------------------

If you want to build python-seabreeze manually you have to install some build dependencies.

Step (1) - Linux
^^^^^^^^^^^^^^^^

Linux requires a compiler and libusb development headers:

.. code:: bash

    # run the appropriate command for your distribution flavor

    # if your distro is a deb flavor (ubuntu, etc...)
    sudo apt-get install git-all build-essential libusb-dev

    # if your distro is a rpm flavor
    sudo yum install git-all gcc gcc-c++ make libusb-devel

    # if your distro is a arch flavor
    sudo pacman -S base-devel libusb libusb-compat libusb-devel make gcc

Step (1) - OSX
^^^^^^^^^^^^^^

OSX should already have everything installed. It's possible that you have to install the latest XCode though.

Step (1) - Windows
^^^^^^^^^^^^^^^^^^

Windows needs a build environment and the Windows Driver Kit. The following instructions are for
builing python-seabreeze for python 3.5 and above. It uses `chocolatey <https://chocolatey.org/>`_
to install everything needed:

.. code:: bat

    REM assuming you already have chocolatey installed
    choco install visualcpp-build-tools
    choco install windows-sdk-10.1
    choco install windowsdriverkit10


Step (2) - prepare pip
^^^^^^^^^^^^^^^^^^^^^^

Now we can install the things required for installing via pip. For that we should make sure that the
latest pip version is installed. So with the python interpreter of your choice in your path do:

.. code:: bash

    python -m pip install --upgrade pip


Step (3) - clone and install
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Finally we can clone and install python-seabreeze:

.. code:: bash

    git clone https://github.com/ap--/python-seabreeze.git python-seabreeze
    cd python-seabreeze
    python -m pip install .

This should install seabreeze in your python environment.


Operating System dependent setup
--------------------------------

Additional installation steps required by the specific operating system should be done by
running the script provided by python-seabreeze. After installing via pip or conda or manually, run:

.. code:: bash

    seabreeze_os_setup

For some additional information see
`os_support/readme.md <https://github.com/ap--/python-seabreeze/blob/master/os_support/readme.md>`_.


Installing on a Raspberry Pi
----------------------------

Rasperry Pi wheels are automagically provided by the wonderful
`piwheels package repository <https://www.piwheels.org/project/seabreeze/>`_. Follow the
instructions `here <https://www.piwheels.org/>`_ on how to add piwheels as an extra index
to pip and then you should be able to install via:

.. code:: bash

    pip3 install seabreeze
