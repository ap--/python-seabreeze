#!/bin/bash

echo "downloading libseabreeze"
wget http://sourceforge.net/projects/seabreeze/files/SeaBreeze/seabreeze-3.0.0.tar.bz2
echo "extracting libseabreeze"
tar xjf seabreeze-3.0.0.tar.bz2

echo "build shared library"
cd seabreeze-3.0.0
make lib/libseabreeze.so
cd ..

echo "install to /usr/local/lib"
sudo install seabreeze-3.0.0/lib/libseabreeze.so /usr/local/lib/

