# Backends

python-seabreeze interfaces the spectrometers on of two dofferent backends.

### cseabreeze (recommended) ###

wraps the [SeaBreeze](http://oceanoptics.com/product/seabreeze/) library with
[Cython](http://cython.org). It uses the _SeaBreeze 2.0 API_ ("SeaBreezeAPI.h"
instead of "SeaBreezeWrapper.h"). If installed, it is the default backend for
python-seabreeze.

Since the seabreeze-c-library is developed by OceanOptics, it should work
nicely with all listed Spectrometers.

### pyseabreeze ###

is an implementation of cseabreeze using
[pyusb](https://github.com/walac/pyusb) as a backend. It originated from
[python-oceanoptics](https://github.com/ap--/python-oceanoptics). Since it
doesn't require compilation, it's easily installable on any system.

It supports all basic spectrometer features (check [this
list](seabreeze/pyseabreeze/TODO.md) for what works and what doesn't.)

