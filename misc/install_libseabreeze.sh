#!/bin/bash

SB_VER=3.0.11.11
SB_URL=https://github.com/ap--/libseabreeze/archive/libseabreeze-${SB_VER}.zip

cat << EOF
running install script:

Compiling the python-seabreeze module from source...
====================================================

EOF

curl -L -o libseabreeze.zip ${SB_URL}
unzip libseabreeze.zip
mv libseabreeze-libseabreeze-${SB_VER} libseabreeze
cd libseabreeze/SeaBreeze
sed -i "s/\t/    /g" src/common/Log.cpp

case "$(uname -s)" in
  Linux)
    SEABREEZE="libseabreeze.so"
    make "lib/${SEABREEZE}"
    echo "# SEABREEZE: installing to /usr/local/lib"
    sudo install "lib/${SEABREEZE}" /usr/local/lib/
    sudo ldconfig
    if ldconfig -p | grep "seabreeze"; then
        echo "ok."
    else
        sudo echo "/usr/local/lib" >> /etc/ld.so.conf.d/usrlocal.conf
        sudo ldconfig
        echo "should be ok now."
    fi
    ;;
  Darwin)
    SEABREEZE="libseabreeze.dylib"
    echo "Requires xcode."
    echo ""
    echo "# SEABREEZE: building shared library"
    make install_name=/usr/local/lib/${SEABREEZE} "lib/${SEABREEZE}"
    echo "# SEABREEZE: installing to /usr/local/lib"
    sudo install "lib/${SEABREEZE}" /usr/local/lib/
    ;;
  *)
    echo 'Your operating system is not supported.'
    ;;
esac
