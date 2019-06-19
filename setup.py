# -*- coding: utf-8 -*-
import os.path
import platform
import sys

from setuptools import setup, Extension, find_packages

SEABREEZE_VERSION = "0.6.0"

if "--without-cseabreeze" in sys.argv:
    sys.argv.remove("--without-cseabreeze")  # this is a hack...
    # user requests to not install cython wrapper
    _extensions = []
else:
    system = platform.system()

    # cseabreeze source files
    sources = ['src/seabreeze/cseabreeze/wrapper.pyx']
    for root, dirnames, filenames in os.walk('src/libseabreeze'):
        for filename in filenames:
            if os.path.splitext(filename)[1] in {'.c', '.cpp'}:
                if 'native' in root:
                    if root.endswith("windows") and system != 'Windows':
                        continue
                    elif root.endswith("winusb") and system != 'Windows':
                        continue
                    elif root.endswith("linux") and system in {'Darwin', 'Windows'}:
                        continue
                    elif root.endswith("osx") and system != 'Darwin':
                        continue
                    elif root.endswith("posix") and system == 'Windows':
                        continue
                sources.append(os.path.join(root, filename))

    # link
    if platform.system() == "Windows":
        libs = ['winusb']
    elif platform.system() == "Darwin":
        libs = []
    else:
        libs = ['usb']
    # define extension
    libseabreeze = Extension('seabreeze.cseabreeze.wrapper',
                             sources=sources,
                             include_dirs=['src/libseabreeze/include'],
                             libraries=libs)
    _extensions = [libseabreeze]

setup(
    name='seabreeze',
    version=SEABREEZE_VERSION,
    author='Andreas Poehlmann',
    author_email='andreas@poehlmann.io',
    package_dir={'': 'src'},
    packages=find_packages(where='src'),
    setup_requires=[
        # Setuptools 18.0 properly handles Cython extensions.
        'setuptools>=18.0',
        'cython>=0.18',
        'numpy'
    ],
    scripts=[
        'scripts/seabreeze-compare'
    ],
    description=('Python interface module for oceanoptics spectrometers. '
                 'This software is not associated with Ocean Optics. '
                 'Use it at your own risk.'),
    long_description=open('README.md').read(),
    install_requires=[
        'pyusb>=1.0',
        'numpy'
    ],
    ext_modules=_extensions,
)
