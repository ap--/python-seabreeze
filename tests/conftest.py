import mock
import pytest


def pytest_addoption(parser):
    pyusb_choices = [
        "any",
        "cseabreeze",
        "pyseabreeze",
        "pyseabreeze:libusb0",
        "pyseabreeze:libusb1",
        "pyseabreeze:openusb",
    ]
    parser.addoption(
        "--seabreeze-backend",
        default="any",
        choices=pyusb_choices,
        help="options: %s" % ", ".join(pyusb_choices),
    )


@pytest.fixture(scope="function")
def mock_pyusb_core_find():
    """mock usb.core.find

    when running on a ci, the pyusb can crash if access
    to the usbfs is denied: https://github.com/pyusb/pyusb/issues/151

    """
    try:
        with mock.patch("usb.core.find", return_value=[]):
            yield
    except ImportError:
        pytest.mark.skip(reason="no pyusb installed")


@pytest.fixture(scope="module")
def cseabreeze():
    """try importing the cseabreeze backend"""
    yield pytest.importorskip("seabreeze.cseabreeze")


@pytest.fixture(scope="module")
def pyseabreeze():
    """try importing the pyseabreeze backend"""
    yield pytest.importorskip("seabreeze.pyseabreeze")


@pytest.fixture(
    scope="function",
    params=[None, "openusb", "libusb0", "libusb1"],
    ids=["any", "openusb", "libusb0", "libusb1"],
)
def pyseabreeze_pyusb_backend(request):
    yield request.param
