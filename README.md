**This software is not associated with Ocean Optics. Use it at your own risk.**

## SeaBreeze python module ##

This is a cython wrapper for the [SeaBreeze](http://oceanoptics.com/product/seabreeze/)
library for [OceanOptics](http://www.oceanoptics.com/) spectrometers.

It's missing a _setup.py_ and some stuff that [python-oceanoptics](https://github.com/ap--/python-oceanoptics)
can do, but at least it should support all spectrometers that [SeaBreeze](http://oceanoptics.com/product/seabreeze/)
can handle:

## Should work with: ##

* HR2000
* HR2000PLUS
* HR4000
* JAZ
* MAYA2000
* MAYA2000PRO
* NIRQUEST256
* NIRQUEST512
* QE65000
* QE-PRO
* STS
* TORUS
* USB2000
* USB2000PLUS
* USB4000

## How to install ##

**This should only work on linux! (at least for now...)**

Install seabreeze C-library:
```
./install_libseabreeze.sh
```

Create the python module:
```
make
```

Test it (test.py, requires matplotlib):
```
python test/testcapi.py
python test/test.py
```

## Requirements ##

I am using _Cython 0.20.2_.

## License ##

Files in this repository are released under the [MIT license](LICENSE.md).
