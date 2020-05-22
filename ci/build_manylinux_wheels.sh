#!/bin/bash
set -e -x

yum install -y libusb-devel

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    if [[ ! $PYBIN =~ "34" ]]; then  # exclude 3.4
        "${PYBIN}/pip" wheel /io/ -w wheelhouse/
    fi
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/seabreeze-*.whl; do
    auditwheel repair "$whl" --plat $PLAT -w /io/wheelhouse/
done

# Install packages and test
for PYBIN in /opt/python/*/bin/; do
    echo $(ls wheelhouse/*.whl)
done