#!/bin/bash
curl https://raw.githubusercontent.com/matthew-brett/multibuild/88a0b6f0eb770cf9f95792d66410e7696ce3d384/osx_utils.sh -s -o osx_utils.sh
curl https://raw.githubusercontent.com/matthew-brett/multibuild/88a0b6f0eb770cf9f95792d66410e7696ce3d384/common_utils.sh -s -o common_utils.sh
source osx_utils.sh

MACPYTHON_OSX_VERSION="10.6"
if [[ ${PYTHON_VERSION} == "3.8" ]]; then
    MACPYTHON_OSX_VERSION="10.9"
fi

get_macpython_environment ${PYTHON_VERSION} venv ${MACPYTHON_OSX_VERSION}

echo "Building wheel"
python --version
pip --version
pip wheel . -w wheelhouse/

repair_wheelhouse ./wheelhouse
