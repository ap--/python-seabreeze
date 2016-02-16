#!/bin/bash
# Install seabreeze lib
wget https://github.com/ap--/libseabreeze/releases/download/libseabreeze-${SEABREEZE_VERSION}/libseabreeze-${SEABREEZE_VERSION}.dmg
hdiutil attach libseabreeze-${SEABREEZE_VERSION}.dmg
cp /Volumes/lib/libseabreeze.dylib /usr/local/lib

# Setup python environment
git clone https://github.com/MacPython/terryfy
source terryfy/travis_tools.sh
get_python_environment macpython $PYTHON_VERSION venv
pip install --upgrade pip
pip install --upgrade wheel
pip install cython
pip install delocate
python --version

# build the wheel
echo "Building seabreeze-wheel for Python: $PYTHON_VERSION"
python setup.py bdist_wheel
wfn=$(echo dist/*.whl)
delocate-wheel --require-archs=intel -v $wfn
