# -*- coding: utf-8 -*-
from setuptools import setup
from distutils.extension import Extension

try:
    from Cython.Build import cythonize
except ImportError:
    cythonize = lambda x: x
    fn_ext = "c"
else:
    fn_ext = "pyx"

import platform
import numpy

if platform.system() == "Windows":
    libs = ['seabreeze', 'winusb']
else:
    libs = ['seabreeze', 'usb']

extensions = [Extension('seabreeze.cseabreeze.wrapper',
                        ['./seabreeze/cseabreeze/wrapper.%s' % fn_ext],
                        include_dirs=[numpy.get_include()],
                        libraries=libs,
                      )]

setup(
    name='seabreeze',
    version='0.4.0',
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
    ext_modules=cythonize(extensions),
)
