**This software is not associated with Ocean Optics. Use it at your own risk.**

**Info:** If this is not working for you, checkout [python-oceanoptics](https://github.com/ap--/python-oceanoptics).

## SeaBreeze python module ##

This is a python module for [OceanOptics](http://www.oceanoptics.com/) spectrometers. It has two different backends. One is using the [SeaBreeze](http://oceanoptics.com/product/seabreeze/) library, and the other one is going to emulate the seabreeze behaviour using **pyusb**

In the longrun all code from [python-oceanoptics](https://github.com/ap--/python-oceanoptics) will be merged into the pyusb seabreeze backend.

But right now, this is all work in progress.

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

This is tested under ubuntu 12.04.

To install seabreeze C-library run:
```
./misc/install_libseabreeze.sh
```

To install the udev rules run:
```
./misc/install_udev_rules.sh
```

To install the python module run:
```
python setup.py install
```

## Requirements ##

You'll need numpy, pyusb, cython. I am using _Cython 0.20.2_.

Under Ubuntu
```
apt-get install build-essential python-numpy python-pip cython libusb-1.0-0
pip install pyusb
```

## Usage ##

With seabreeze C backend:

```{python}
>>> import seabreeze.spectrometers as sbs
>>> devices = sbs.list_spectrometers()
>>> print devices
[<SeaBreezeDevice USB2000PLUS:USB2+H02749>, <SeaBreezeDevice USB2000PLUS:USB2+H02751>]
>>> spec = sbs.Spectrometer(devices[0])
>>> spec.serial
'USB2+H02751'
>>> spec.model
'USB2000PLUS'
>>> spec.integration_time_microsec(12000)
>>> spec.wavelengths()
array([  340.32581   ,   340.70321186,   341.08058305, ...,  1024.84940994,
        1025.1300678 ,  1025.4106617 ])
>>> spec.intensities()
array([  1.58187931e+01,   2.66704852e+04,   6.80208103e+02, ...,
         6.53090172e+02,   6.35011552e+02,   6.71168793e+02])
>>>
```

For the pyusb seabreeze backend, you only need to run **use** before importing **seabreeze.spectrometers** (this is WIP):

```{python}
>>> import seabreeze
>>> seabreeze.use('pyseabreeze')
>>> import seabreeze.spectrometers as sbs
>>>
```

## License ##

Files in this repository are released under the [MIT license](LICENSE.md).
