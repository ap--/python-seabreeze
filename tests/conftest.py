import mock
import pytest


def _running_on_ci():
    """returns if we're currently running on a CI"""
    import os

    CI = os.environ.get("CI", "").lower()
    CONDA_BUILD = os.environ.get("CONDA_BUILD", "").lower()
    if CONDA_BUILD in {"1", "true"}:
        return True
    if CI in {"1", "true", "azure", "travis", "appveyor", "circleci"}:
        return True
    return False


@pytest.fixture(scope="session", autouse=_running_on_ci())
def mock_pyusb_backend_libusb0():
    """mock usb.core.find

    when running on a ci, the libusb0 backend of pyusb can crash if access
    to the usbfs is denied: https://github.com/pyusb/pyusb/issues/151

    this fixture is automatically loaded when the CI env var is set.
    """
    with mock.patch("usb.core.find", return_value=[]):
        yield
