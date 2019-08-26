#!/bin/bash
echo "Building wheel"
python --version
pip --version

"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64 $SBBUILDSDK

export DISTUTILS_USE_SDK=1
export MSSdk=1

pip wheel . -w wheelhouse/
