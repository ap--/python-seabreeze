# -*- coding: utf-8 -*-
from setuptools import setup, Extension, find_packages
import os
import platform

SEABREEZE_VERSION = "0.6.0"


# Platform specific libraries and source files
if platform.system() == "Windows":
    libs = ['winusb']
    ignore_subdirs = {'linux', 'osx', 'posix'}
elif platform.system() == "Darwin":
    libs = []
    ignore_subdirs = {'linux', 'winusb', 'windows'}
else:
    libs = ['usb']
    ignore_subdirs = {'osx', 'winusb', 'windows'}

# Collect all source files for cseabreeze backend
srcs = ['src/seabreeze/cseabreeze/wrapper.pyx']
for root, subdirs, fns in os.walk('src/libseabreeze/src'):
    subdirs[:] = (d for d in subdirs if d not in ignore_subdirs)
    for fn in fns:
        print os.path.join(root, fn)
    srcs.extend((os.path.join(root, fn) for fn in fns))

# define extension
libseabreeze = Extension('seabreeze.cseabreeze.wrapper',
                         language='c++',
                         sources=srcs,
                         include_dirs=['src/libseabreeze/include'],
                         libraries=libs)

extensions = [libseabreeze]

setup(
    name='seabreeze',
    version=SEABREEZE_VERSION,
    author='Andreas Poehlmann',
    author_email='andreas@poehlmann.io',
    setup_requires=[
        'cython>=0.18',
        'numpy'
    ],
    install_requires=[
        'numpy',
        'pyusb>=1.0'
    ],
    packages=find_packages(where='src'),
    package_dir={
        '': 'src'
    },
    description=('Python interface module for oceanoptics spectrometers. '
                 'This software is not associated with Ocean Optics. '
                 'Use it at your own risk.'),
    long_description=open('README.md').read(),
    ext_modules=extensions,
)
