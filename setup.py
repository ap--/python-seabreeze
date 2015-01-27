# -*- coding: utf-8 -*-
from distutils.core import setup
from Cython.Build import cythonize

setup(
    name='seabreeze',
    version='0.1.0',
    author='Andreas Poehlmann',
    author_email='mail@andreaspoehlmann.de',
    packages=['seabreeze',
              'seabreeze.cseabreeze',
              'seabreeze.pyseabreeze',
              'seabreeze.pyseabreeze.interfaces'],
    scripts=[],
    description=('Python interface module for oceanoptics spectrometers. '
                 'This software is not associated with Ocean Optics. '
                 'Use it at your own risk.'),
    long_description=open('README.md').read(),
    requires=['python (>= 2.7)', 'pyusb (>= 1.0)', 'numpy'],
    ext_modules=cythonize('seabreeze/cseabreeze/wrapper.pyx')
)
