import pytest

from seabreeze.spectrometers import list_devices, Spectrometer
import seabreeze.pyseabreeze as psb
import seabreeze.cseabreeze as csb


def _retr():
    params, ids = [], []
    csb_serials, psb_serials = set(), set()
    for serials, backend in [(csb_serials, csb), (psb_serials, psb)]:
        api = backend.SeaBreezeAPI()
        try:
            serials.update((d.serial_number, d.model) for d in api.list_devices())
        finally:
            api.shutdown()
    for ser_mod in csb_serials.union(psb_serials):
        params.extend([
            (csb, ser_mod[0]) if ser_mod in csb_serials else pytest.param((csb, None), marks=pytest.mark.skip),
            (psb, ser_mod[0]) if ser_mod in psb_serials else pytest.param((psb, None), marks=pytest.mark.skip),
        ])
        ids.extend([
            ("cseabreeze:{}:{}".format(ser_mod[1], ser_mod[0])),
            ("pyseabreeze:{}:{}".format(ser_mod[1], ser_mod[0])),
        ])
    if not params:
        params = [
            pytest.param((csb, None), marks=pytest.mark.skip),
            pytest.param((psb, None), marks=pytest.mark.skip),
        ]
        ids = [("cseabreeze:no-spectrometer"), ("pyseabreeze:no-spectrometer")]
    return dict(params=params, ids=ids)


@pytest.fixture(scope='function', **_retr())
def backendlified_serial(request):
    backend, serial = request.param
    # list_devices
    _api = getattr(list_devices, '_api', None)
    if not isinstance(_api, backend.SeaBreezeAPI):
        _api = backend.SeaBreezeAPI()
        setattr(list_devices, '_api', _api)
    # Spectrometer
    Spectrometer._backend = backend
    try:
        yield serial
    finally:
        _api.shutdown()


def test_read_model(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    model = spec.model
    assert len(model) > 0


def test_read_serial_number(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    serial = spec.serial_number
    assert len(serial) > 0


def test_read_spectrum(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    arr = spec.intensities()
    assert arr.size == spec.pixels


