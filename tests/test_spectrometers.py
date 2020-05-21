import pytest

try:
    import seabreeze.pyseabreeze as psb
except ImportError:
    psb = None

try:
    import seabreeze.cseabreeze as csb
except ImportError:
    csb = None


# skip all tests in this module if no backends can be imported
if csb is not None or psb is not None:
    import seabreeze

    seabreeze.use("cseabreeze", force=False)
    from seabreeze.spectrometers import list_devices, Spectrometer
else:
    pytestmark = pytest.mark.skip("No backend available!")


def _retr():
    """retrieves a list of all connected spectrometers for all backends"""
    params, ids = [], []
    csb_serials, psb_serials = set(), set()
    for serials, backend in [(csb_serials, csb), (psb_serials, psb)]:
        if backend is None:
            continue
        api = backend.SeaBreezeAPI()
        try:
            serials.update((d.serial_number, d.model) for d in api.list_devices())
        except:
            pass
        finally:
            api.shutdown()
    for ser_mod in csb_serials.union(psb_serials):
        params.extend(
            [
                (csb, ser_mod[0])
                if ser_mod in csb_serials
                else pytest.param((csb, None), marks=pytest.mark.skip),
                (psb, ser_mod[0])
                if ser_mod in psb_serials
                else pytest.param((psb, None), marks=pytest.mark.skip),
            ]
        )
        ids.extend(
            [
                ("cseabreeze:{}:{}".format(ser_mod[1], ser_mod[0])),
                ("pyseabreeze:{}:{}".format(ser_mod[1], ser_mod[0])),
            ]
        )
    if not params:
        params = [
            pytest.param((csb, None), marks=pytest.mark.skip),
            pytest.param((psb, None), marks=pytest.mark.skip),
        ]
        ids = [("cseabreeze:no-spectrometer"), ("pyseabreeze:no-spectrometer")]
    return dict(params=params, ids=ids)


@pytest.fixture(
    scope="function",
    params=[
        csb or pytest.param(csb, marks=pytest.mark.skip),
        psb or pytest.param(psb, marks=pytest.mark.skip),
    ],
    ids=["cseabreeze", "pyseabreeze"],
)
def backendlify(request):
    backend = request.param
    # list_devices
    _api = getattr(list_devices, "_api", None)
    if not isinstance(_api, backend.SeaBreezeAPI):
        _api = backend.SeaBreezeAPI()
        setattr(list_devices, "_api", _api)
    # Spectrometer
    Spectrometer._backend = backend
    try:
        yield
    finally:
        _api.shutdown()


@pytest.fixture(scope="function", **_retr())  # request all spectrometers on module load
def backendlified_serial(request):
    backend, serial = request.param
    # list_devices
    _api = getattr(list_devices, "_api", None)
    if not isinstance(_api, backend.SeaBreezeAPI):
        _api = backend.SeaBreezeAPI()
        setattr(list_devices, "_api", _api)
    # Spectrometer
    Spectrometer._backend = backend
    try:
        yield serial
    finally:
        _api.shutdown()


@pytest.mark.usefixtures("backendlify")
def test_cant_find_serial():
    exc = Spectrometer._backend.SeaBreezeError
    with pytest.raises(exc):
        Spectrometer.from_serial_number("i-do-not-exist")


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


def test_read_intensities(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    arr = spec.intensities()
    assert arr.size == spec.pixels


def test_read_wavelengths(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    arr = spec.wavelengths()
    assert arr.size == spec.pixels


def test_read_spectrum(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    arr = spec.spectrum()
    assert arr.shape == (2, spec.pixels)


def test_max_intensity(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    value = spec.max_intensity
    assert isinstance(value, float)
    assert 0 < value < 1e8


def test_integration_time_limits(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    low, high = spec.integration_time_micros_limits
    assert isinstance(low, int)
    assert isinstance(high, int)
    assert 0 < low < high < 2 ** 64


def test_integration_time(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    exc = Spectrometer._backend.SeaBreezeError
    spec = Spectrometer.from_serial_number(backendlified_serial)

    with pytest.raises(exc):
        # fail because too low
        spec.integration_time_micros(0)

    with pytest.raises(exc):
        # fail because too large
        spec.integration_time_micros(2 ** 62)

    with pytest.raises(exc):
        # fail because too large long overflow
        spec.integration_time_micros(2 ** 64)

    spec.integration_time_micros(10000)


def test_trigger_mode(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    exc = Spectrometer._backend.SeaBreezeError
    spec = Spectrometer.from_serial_number(backendlified_serial)

    spec.trigger_mode(0x00)  # <- normal mode

    with pytest.raises(exc):
        spec.trigger_mode(0xF0)  # <- should be unsupported for all specs
    # test again to see if the bus is locked
    spec.trigger_mode(0x00)  # <- normal mode
