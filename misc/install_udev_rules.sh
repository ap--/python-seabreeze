#!/bin/bash
SEABREEZE_UDEV="10-oceanoptics.rules"

echo -e "# SEABREEZE: copying ${SEABREEZE_UDEV} to /etc/udev/rules.d/"
sudo cp ./${SEABREEZE_UDEV} /etc/udev/rules.d/

echo -e "# SEABREEZE: reloading udev rules"
sudo udevadm control --reload-rules
