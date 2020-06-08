import subprocess

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
    if not _running_on_ci():
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
        (psb, "openusb") if psb else pytest.param((), marks=pytest.mark.skip),
        (psb, "libusb0") if psb else pytest.param((), marks=pytest.mark.skip),
        (psb, "libusb1") if psb else pytest.param((), marks=pytest.mark.skip),
        (csb, None) if csb else pytest.param((), marks=pytest.mark.skip),
    ],
    ids=["pyseabreeze(openusb)", "pyseabreeze(libusb0)", "pyseabreeze(libusb1)", "cseabreeze"],
)
def backendlify(request):
    backend, usb_backend = request.param
    # list_devices
    kwargs = {} if usb_backend is None else {"_pyusb_backend": usb_backend}
    try:
        _api = backend.SeaBreezeAPI(**kwargs)
    except RuntimeError:
        pytest.skip("can't load pyusb backend '{}'".format(usb_backend))
        _api = None
    setattr(list_devices, "_api", _api)
    # Spectrometer
    Spectrometer._backend = backend
    try:
        yield backend
    finally:
        _api.shutdown()
        delattr(list_devices, "_api")


@pytest.fixture(scope="function", **_retr())  # request all spectrometers on module load
def backendlified_serial(request):
    backend, serial = request.param
    # list_devices
    _api = backend.SeaBreezeAPI()
    setattr(list_devices, "_api", _api)
    # Spectrometer
    Spectrometer._backend = backend
    try:
        yield serial
    finally:
        _api.shutdown()
        delattr(list_devices, "_api")


@pytest.mark.usefixtures("backendlify")
def test_cant_find_serial():
    exc = Spectrometer._backend.SeaBreezeError
    with pytest.raises(exc):
        Spectrometer.from_serial_number("i-do-not-exist")


def test_device_cleanup_on_exit(backendlify):
    """test if opened devices cleanup correctly"""
    usb_backend = ""
    try:
        devices = list_devices()
        if len(devices) == 0:
            pytest.skip("no supported device connected")
        else:
            if isinstance(devices[0], psb.SeaBreezeDevice):
                # noinspection PyProtectedMember
                usb_backend = "_pyusb_backend='{}'".format(
                    devices[0]._raw_device.pyusb_backend
                )
        del devices
    finally:
        # noinspection PyProtectedMember
        list_devices._api.shutdown()

    # noinspection PyProtectedMember
    backend_name = backendlify._backend_

    cmd = [
        "python",
        "-c",
        "import seabreeze.%s as sb; d = sb.SeaBreezeAPI(%s).list_devices()[0]; d.open()"
        % (backend_name, usb_backend),
    ]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    assert p.returncode == 0, stderr


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


@pytest.mark.xfail(reason='check if following tests work after crash')
def test_crash_may_not_influence_following_tests(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    _ = Spectrometer.from_serial_number(backendlified_serial)
    raise Exception('crash on purpose')


def test_read_intensities(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    spec = Spectrometer.from_serial_number(backendlified_serial)
    arr = spec.intensities()
    assert arr.size == spec.pixels


def test_correct_dark_pixels(backendlified_serial):
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")
    # noinspection PyProtectedMember
    SeaBreezeError = Spectrometer._backend.SeaBreezeError

    spec = Spectrometer.from_serial_number(backendlified_serial)
    try:
        arr = spec.intensities(correct_dark_counts=True)
    except SeaBreezeError:
        pytest.skip("does not support dark counts")
    else:
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


@pytest.mark.usefixtures("backendlify")
def test_list_devices_dont_close_opened_devices():
    """test list_devices() interaction with already opened devices"""
    devices = list(list_devices())
    if len(devices) == 0:
        pytest.skip("no supported device connected")

    num_devices = len(devices)

    dev = devices[0]
    dev.open()
    assert dev.is_open is True
    devices_2 = list_devices()
    assert len(devices_2) == num_devices
    # dev.is_open is still True here...
    assert dev.is_open is True
    # but if we delete the new reference created by the second call to list_devices:
    del devices_2
    assert dev.is_open is True
