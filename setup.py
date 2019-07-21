"""
python-seabreeze
================

python module for OceanOptics spectrometers

Author: Andreas Poehlmann
Email: andreas@poehlmann.io

"""
import logging
import os
import platform
import sys
from distutils.sysconfig import customize_compiler

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

try:
    import Cython
except ImportError:
    WARN_NO_CYTHON = True
else:
    WARN_NO_CYTHON = False

SEABREEZE_VERSION = "0.6.1"

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
sources = ['src/seabreeze/cseabreeze/c_seabreeze_wrapper.pyx']
for root, subdirs, fns in os.walk('src/libseabreeze/src'):
    subdirs[:] = (d for d in subdirs if d not in ignore_subdirs)
    sources.extend((os.path.join(root, fn) for fn in fns))

# define extension
libseabreeze = Extension('seabreeze.cseabreeze._wrapper',
                         language='c++',
                         sources=sources,
                         include_dirs=['src/libseabreeze/include'],
                         libraries=libs)

# TODO: detect if running on rtd?
building_sphinx_documentation = True
libseabreeze.cython_directives = {
    'binding': building_sphinx_documentation,  # fix class method parameters for sphinx
    'embedsignature': not building_sphinx_documentation,  # add function signature to docstring for ipython
}
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


if WARN_NO_CYTHON:
    logging.basicConfig(stream=sys.stderr, level=logging.INFO)
    log = logging.getLogger('setup')
    log.info("if error `unknown file type '.pyx'` occurs try `pip install cython` and rerun")

setup(
    name='seabreeze',
    version=SEABREEZE_VERSION,
    author='Andreas Poehlmann',
    author_email='andreas@poehlmann.io',
    setup_requires=[
        'setuptools>=18.0',
        'cython>=0.18',
        "numpy<1.17 ; python_version<'3.6'",  # numpy support for <3.6 dropped with 1.17
        "numpy ; python_version>='3.6'",
    ],
    install_requires=[
        "numpy<1.17 ; python_version<'3.6'",  # numpy support for <3.6 dropped with 1.17
        "numpy ; python_version>='3.6'", 'numpy'
    ],
    extras_require={
        'cseabreeze': [],
        'pyseabreeze': ['pyusb >=1.0']
    },
    cmdclass={
        'build_ext': sb_build_ext
    },
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
