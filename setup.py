# -*- coding: utf-8 -*-
from setuptools import setup
from distutils.extension import Extension
from Cython.Build import cythonize

import platform
import numpy

if platform.system() == "Windows":
    libs = ['seabreeze', 'winusb']
else:
    libs = ['seabreeze', 'usb']

extensions = [Extension('seabreeze.cseabreeze.wrapper',
                                    ['./seabreeze/cseabreeze/wrapper.pyx'],
                        include_dirs=[numpy.get_include()],
                        libraries=libs,
                      )]

setup(
    name='seabreeze',
    version='0.3.4',
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
