# -*- coding: utf-8 -*-
from distutils.core import setup
try:
    from Cython.Build import cythonize
    USE_CYTHON = True
except ImportError:
    USE_CYTHON = False

if USE_CYTHON:
    ext_modules = cythonize('./seabreeze/cseabreeze/wrapper.pyx')
else:
    ext_modules = []
    print "WARNING: Cython not found! Not compiling seabreeze C/C++ library wrapper!"

setup(
    name='seabreeze',
    version='0.3.3',
    author='Andreas Poehlmann',
    author_email='mail@andreaspoehlmann.de',
    packages=['seabreeze',
              'seabreeze.cseabreeze',
              'seabreeze.pyseabreeze',
              'seabreeze.pyseabreeze.interfaces'],
    scripts=['scripts/seabreeze-compare'],
    description=('Python interface module for oceanoptics spectrometers. '
                 'This software is not associated with Ocean Optics. '
                 'Use it at your own risk.'),
    long_description=open('README.md').read(),
    requires=['python (>= 2.7)', 'pyusb (>= 1.0)', 'numpy'],
    ext_modules=ext_modules
)
