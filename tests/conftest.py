import mock
import pytest


@pytest.fixture(scope="session")
def mock_pyusb_core_find():
    """mock usb.core.find

    when running on a ci, the pyusb can crash if access
    to the usbfs is denied: https://github.com/pyusb/pyusb/issues/151

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


@pytest.fixture(scope="module")
def cseabreeze_api(cseabreeze):
    api = cseabreeze.SeaBreezeAPI()
    try:
        yield api
    finally:
        api.shutdown()


@pytest.fixture(
    scope="function",
    params=[None, "openusb", "libusb0", "libusb1"],
    ids=["any", "openusb", "libusb0", "libusb1"],
)
def pyseabreeze_pyusb_backend(request):
    yield request.param


@pytest.fixture(scope="module")
def pyseabreeze_api(pyseabreeze, pyseabreeze_pyusb_backend):
    try:
        api = pyseabreeze.SeaBreezeAPI(_pyusb_backend=pyseabreeze_pyusb_backend)
    except RuntimeError:
        pytest.skip("can't load pyusb backend {}".format(pyseabreeze_pyusb_backend))
    else:
        try:
            yield api
        finally:
            api.shutdown()
