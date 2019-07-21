================
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


.. figure:: _static/overview.png
    :figwidth: image
    :align: center

    python-seabreeze module structure



python-seabreeze in the wild
============================

python-seabreeze is used in many labs around the world. One of the cool experiments I know of was
a `student project measuring fluorescence spectra (#17)
<https://www.spacestationexplorers.org/new-student-experiments-reach-iss-on-spacex-crs-14>`_ with a
``Spark`` spectrometer controlled by a raspberry pi zero, which was sent to the International
Space Station on the SpaceX CRS-14 resupply mission. *(space proof)*

.. tip::

    If you want me to add your project here, let me know. Happy to add it.

.. rubric:: projects

- SeaBreeze_ - Ocean Optics' SeaBreeze C library.
- libseabreeze_ - github clone of the C library. *internal use only* (has pre-built libraries if you know what you're doing)
- python-seabreeze-feedstock_ - anaconda feedstock for automated package deployment. *internal use only*

.. _libseabreeze: https://github.com/ap--/libseabreeze
.. _python-seabreeze-feedstock: https://github.com/ap--/python-seabreeze


---------------------------------------------------------------------------------------------------

.. sidebar:: A note from the author
    :subtitle: (ノ\*-_-\*)ノ┴—┴

    Over the years, maintaining this project made me realize that supporting the authors of
    open source projects is a crucial (but mostly ignored) part of the open source community.
    If python-seabreeze_ saved you some development time, consider contributing to the
    project, or send me an email and tell me about your project, or consider buying me coffee,
    or support me via paypal. Something small like that puts a smile on my face for weeks, and
    it's extremely motivating.

    .. rubric:: Anyways, enough ranting (^_^") let's science!

.. _python-seabreeze: https://github.com/ap--/python-seabreeze


========
Contents
========

.. toctree::
   :caption: Introduction:
   :maxdepth: 3

   tldr.rst
   install.rst
   quickstart.rst

.. toctree::
   :caption: Development:

   api.rst
   backend_api.rst

.. toctree::
   :caption: Support:

   contributing.rst



Indices and tables
==================
* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
