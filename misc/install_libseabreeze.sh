#!/bin/bash
SEABREEZE_FILE="seabreeze-3.0.3.tar.bz2"

SEABREEZE_FOLDER=$(basename $SEABREEZE_FILE .tar.bz2)
SEABREEZE_URL="http://sourceforge.net/projects/seabreeze/files/SeaBreeze"

if [ ! -f $SEABREEZE_FILE ]; then
    echo -e "# SEABREEZE: downloading $SEABREEZE_FILE"
    wget -nv -nc "$SEABREEZE_URL/$SEABREEZE_FILE"
else
    echo -e "# SEABREEZE: using $SEABREEZE_FILE"
fi

echo -e "# SEABREEZE: extracting $SEABREEZE_FILE"
if ! tar xjf $SEABREEZE_FILE; then
    exit 1
fi

echo -e "# SEABREEZE: building shared library"
cd $SEABREEZE_FOLDER
make lib/libseabreeze.so
cd ..

echo -e "# SEABREEZE: installing to /usr/local/lib"
sudo install "$SEABREEZE_FOLDER/lib/libseabreeze.so" /usr/local/lib/
