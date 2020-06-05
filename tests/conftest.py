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
def mock_pyusb_core_find():
    """mock usb.core.find

    when running on a ci, the libusb0 backend of pyusb can crash if access
    to the usbfs is denied: https://github.com/pyusb/pyusb/issues/151

    this fixture is automatically loaded when the CI env var is set.
    """
    with mock.patch("usb.core.find", return_value=[]):
        yield


@pytest.fixture(scope="module")
def cseabreeze():
    """try importing the cseabreeze backend"""
    yield pytest.importorskip("seabreeze.cseabreeze")


@pytest.fixture(scope="module")
def pyseabreeze():
    """try importing the pyseabreeze backend"""
    yield pytest.importorskip("seabreeze.pyseabreeze")


@pytest.fixture(scope='function')
def cseabreeze_api(cseabreeze):
    api = cseabreeze.SeaBreezeAPI()
    try:
        yield api
    finally:
        api.shutdown()


@pytest.fixture(scope='function')
def pyseabreeze_api(pyseabreeze):
    api = pyseabreeze.SeaBreezeAPI()
    try:
        yield api
    finally:
        api.shutdown()
