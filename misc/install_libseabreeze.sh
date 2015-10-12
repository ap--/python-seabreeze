#!/bin/bash
SEABREEZE_FILE="seabreeze-3.0.10.zip"

SEABREEZE_FOLDER=$(basename $SEABREEZE_FILE .zip)
SEABREEZE_URL="http://sourceforge.net/projects/seabreeze/files/SeaBreeze/source"

case "$(uname -s)" in
  Linux)
    SEABREEZE_EXTENSION="so"
    ;;
  Darwin)
    SEABREEZE_EXTENSION="dylib"
    ;;
  CYGWIN*)
    SEABREEZE_EXTENSION="dll"
    ;;
  *)
    echo 'Your operating system is not supported.'
    ;;
esac

if [ ! -f $SEABREEZE_FILE ]; then
    echo -e "# SEABREEZE: downloading $SEABREEZE_FILE"
    wget -nv -nc "$SEABREEZE_URL/$SEABREEZE_FILE"
else
    echo -e "# SEABREEZE: using $SEABREEZE_FILE"
fi

echo -e "# SEABREEZE: extracting $SEABREEZE_FILE"
if ! unzip -n $SEABREEZE_FILE; then
    exit 1
fi

echo -e "# SEABREEZE: building shared library"
cd $SEABREEZE_FOLDER
make "lib/libseabreeze.$SEABREEZE_EXTENSION"
cd ..

echo -e "# SEABREEZE: installing to /usr/local/lib"
sudo install "$SEABREEZE_FOLDER/lib/libseabreeze.$SEABREEZE_EXTENSION" /usr/local/lib/
