"""
python-seabreeze
================

python module for OceanOptics spectrometers

Author: Andreas Poehlmann
Email: andreas@poehlmann.io

"""
import os
import platform
from distutils.sysconfig import customize_compiler

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

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
sources = ['src/seabreeze/cseabreeze/wrapper.pyx']
for root, subdirs, fns in os.walk('src/libseabreeze/src'):
    subdirs[:] = (d for d in subdirs if d not in ignore_subdirs)
    sources.extend((os.path.join(root, fn) for fn in fns))

# define extension
libseabreeze = Extension('seabreeze.cseabreeze.wrapper',
                         language='c++',
                         sources=sources,
                         include_dirs=['src/libseabreeze/include'],
                         libraries=libs)
extensions = [libseabreeze]


# prevent cpp compiler warning
# - see: https://stackoverflow.com/a/36293331
# - see: https://github.com/python/cpython/pull/7476/files
# noinspection PyPep8Naming
class sb_build_ext(build_ext):
    def build_extensions(self):
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        except (AttributeError, ValueError):
            pass
        build_ext.build_extensions(self)


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
    cmdclass={'build_ext': sb_build_ext},
    packages=find_packages(where='src'),
    package_dir={
        '': 'src'
    },
    description=('Python interface module for Ocean Optics spectrometers. '
                 'This software is not associated with Ocean Optics. '
                 'Use it at your own risk.'),
    long_description=open('README.md').read(),
    ext_modules=extensions,
)
