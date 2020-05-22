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
from distutils.util import strtobool

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

try:
    import Cython
except ImportError:
    WARN_NO_CYTHON = True
else:
    WARN_NO_CYTHON = False

if "--without-cseabreeze" in sys.argv:
    # user requests to not install cython wrapper
    sys.argv.remove("--without-cseabreeze")  # this is a hack...
    # The correct way to do this, would be to:
    # - make cseabreeze it's own python module and make python-seabreeze depend on
    #   it as an extra dependency
    # - by default ship pyseabreeze but keep pyseabreeze as an extra so that installing
    #   pyusb is optional
    # - require users to decide which backend they want to install...
    extensions = []
else:

    # Platform specific libraries and source files
    if platform.system() == "Windows":
        ignore_subdirs = {"linux", "osx", "posix"}
        compile_opts = dict(
            define_macros=[("_WINDOWS", None)],
            include_dirs=[],
            libraries=["winusb", "ws2_32", "setupapi"],
            library_dirs=[],
        )
        if sys.version_info.major == 2:  # this should actually check if msv9
            compile_opts["define_macros"].extend([("WINDOWS", None), ("_WIN32", None)])

    elif platform.system() == "Darwin":
        ignore_subdirs = {"linux", "winusb", "windows"}
        compile_opts = dict(
            define_macros=[], include_dirs=[], libraries=[], library_dirs=[]
        )
        compile_opts["extra_link_args"] = [
            "-framework",
            "Carbon",
            "-framework",
            "CoreFoundation",
            "-framework",
            "IOKit",
        ]

    else:
        ignore_subdirs = {"osx", "winusb", "windows"}
        try:
            import pkgconfig

            compile_opts = pkgconfig.parse("libusb")
        except ImportError:
            compile_opts = dict(
                define_macros=[], include_dirs=[], libraries=["usb"], library_dirs=[]
            )

    # Collect all source files for cseabreeze backend
    sources = ["src/seabreeze/cseabreeze/c_seabreeze_wrapper.pyx"]
    for root, subdirs, fns in os.walk("src/libseabreeze/src"):
        subdirs[:] = (d for d in subdirs if d not in ignore_subdirs)
        sources.extend((os.path.join(root, fn) for fn in fns))
    # Add seabreeze include dirs
    compile_opts["include_dirs"].append(os.path.relpath("src/libseabreeze/include"))

    # define extension
    libseabreeze = Extension(
        "seabreeze.cseabreeze._wrapper",
        language="c++",
        sources=[os.path.relpath(s) for s in sources],
        **compile_opts
    )

    building_sphinx_documentation = bool(
        strtobool(os.environ.get("READTHEDOCS", "false"))
    )
    libseabreeze.cython_directives = {
        "binding": building_sphinx_documentation,  # fix class method parameters for sphinx
        "embedsignature": not building_sphinx_documentation,  # add function signature to docstring for ipython
    }
    extensions = [libseabreeze]


# noinspection PyPep8Naming
class sb_build_ext(build_ext):
    def build_extensions(self):
        # Deal with windows command line limit
        if os.name == "nt":
            # noinspection PyArgumentList
            self.compiler.spawn = win_spawn.__get__(self.compiler)
        # prevent cpp compiler warning
        # - see: https://stackoverflow.com/a/36293331
        # - see: https://github.com/python/cpython/pull/7476/files
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        except (AttributeError, ValueError):
            pass
        # call superclass
        build_ext.build_extensions(self)


# the windows shell can't handle all the object files provided to link.exe
def win_spawn(self, cmd):
    from distutils.spawn import spawn
    from subprocess import list2cmdline

    old_path = os.getenv("path")
    try:
        if hasattr(self, "_paths"):  # msv9 compat
            os.environ["path"] = self._paths
        if cmd[0].endswith("link.exe"):
            with open("ihatewindowssomuch.rsp", "w") as f:
                f.write(list2cmdline(cmd[1:]) + "\n\r")
            return spawn(cmd[:1] + ["@{}".format(os.path.abspath(f.name))])
        else:
            return spawn(cmd)
    finally:
        if hasattr(self, "_paths"):
            os.environ["path"] = old_path


if WARN_NO_CYTHON and extensions:
    logging.basicConfig(stream=sys.stderr, level=logging.INFO)
    log = logging.getLogger("setup")
    log.info(
        "if error `unknown file type '.pyx'` occurs try `pip install cython` and rerun"
    )

setup(
    name="seabreeze",
    author="Andreas Poehlmann",
    author_email="andreas@poehlmann.io",
    url="https://github.com/ap--/python-seabreeze",
    license="MIT",
    use_scm_version={
        "write_to": "src/seabreeze/_version.py",
        "write_to_template": '__version__ = "{version}"',
        "tag_regex": r"^(?P<pkg>python-seabreeze-)?(?P<prefix>v)?(?P<version>[^\+]+)(?P<suffix>.*)?$",
    },
    setup_requires=[
        "setuptools>=18.0",
        "cython>=0.18",
        "wheel>=0.31.0",
        "setuptools_scm",
        "pkgconfig",
        "pytest-runner",
    ],
    install_requires=[
        "numpy<1.17 ; python_version<'3.6'",  # numpy support for <3.6 dropped with 1.17
        "numpy ; python_version>='3.6'",
        "future",
    ],
    extras_require={
        "cseabreeze": [],
        "pyseabreeze": ["pyusb >=1.0", "enum34 ; python_version<'3.4'"],
    },
    tests_require=[
        "pytest<5",
        # mock and zipp break python setup.py pytest on python 2.7
        "mock<4 ; python_version<'3.6'",
        "mock ; python_version>='3.6'",
        "zipp<2 ; python_version<'3.6'",
        "zipp ; python_version>='3.6'",
    ],
    cmdclass={"build_ext": sb_build_ext},
    ext_modules=extensions,
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    entry_points={"console_scripts": ["seabreeze_os_setup=seabreeze.os_setup:main"],},
    description=(
        "Python interface module for Ocean Optics spectrometers. "
        "This software is not associated with Ocean Optics. "
        "Use it at your own risk."
    ),
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    classifiers=[
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
    ],
)
