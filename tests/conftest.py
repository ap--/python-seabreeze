import os
from distutils.util import strtobool

import mock
import pytest

# Fix for running pyseabreeze tests on CIs
RUNNING_ON_CI = strtobool(os.environ.get("CI", "0")) == 1


@pytest.fixture(scope="session", autouse=RUNNING_ON_CI)
def mock_pyusb_backend_libusb0():
    """mock usb.backend.libusb0

    when running on a ci, the libusb0 backend of pyusb can crash if access
    to the usbfs is denied: https://github.com/pyusb/pyusb/issues/151

    this fixture is automatically loaded when the CI env var is set.
    """
    with mock.patch("usb.backend.libusb0.get_backend"):
        yield
