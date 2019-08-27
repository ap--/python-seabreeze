#!/bin/bash
echo "Building wheel"
python --version
pip --version

echo "# INCLUDE:"
echo $INCLUDE

echo "# "
pip wheel . -w wheelhouse/
