import pytest

from seabreeze.spectrometers import list_devices, Spectrometer
import seabreeze.pyseabreeze as psb
import seabreeze.cseabreeze as csb


@pytest.fixture(scope='function', params=[psb, csb], ids=['pyseabreeze', 'cseabreeze'])
def backendify_classes(request):
    backend = request.param
    # list_devices
    _api = getattr(list_devices, '_api', None)
    if not isinstance(_api, backend.SeaBreezeAPI):
        _api = backend.SeaBreezeAPI()
        setattr(list_devices, '_api', _api)
    # Spectrometer
    Spectrometer._backend = backend
    try:
        yield
    finally:
        _api.shutdown()


@pytest.mark.usefixtures("backendify_classes")
def test_read_model():
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    for device in devices:
        spec = Spectrometer(device)
        model = spec.model
        assert len(model) > 0


@pytest.mark.usefixtures("backendify_classes")
def test_read_serial_number():
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    for device in devices:
        spec = Spectrometer(device)
        serial = spec.serial_number
        assert len(serial) > 0


@pytest.mark.usefixtures("backendify_classes")
def test_read_spectrum():
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    for device in devices:
        spec = Spectrometer(device)
        arr = spec.intensities()
        assert arr.size == spec.pixels


