#!/bin/bash
echo "Building wheel"
python --version
pip --version

"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64 10.0.17134.0
set DISTUTILS_USE_SDK=1

pip wheel . -w wheelhouse/
