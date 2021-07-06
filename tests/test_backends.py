# test both backends
import pytest


def test_seabreeze_installed():
    """if this test fails, you forgot to install seabreeze in your environment"""
    # noinspection PyUnresolvedReferences
    import seabreeze


def test_seabreeze_wrong_backend_requested():
    """requests a nonexistent backend"""
    import seabreeze

    with pytest.raises(ValueError):
        seabreeze.use("i-do-not-exist")


@pytest.mark.parametrize("backend_name", ["cseabreeze", "pyseabreeze"])
def test_seabreeze_any_backend_available(backend_name):
    """requests one backend and allows fallback"""
    import seabreeze

    seabreeze.use(backend_name, force=False)
    from seabreeze.backends import get_backend

    get_backend()


def test_seabreeze_cseabreeze_backend_available():
    """tests if cseabreeze was compiled on install"""
    try:
        import seabreeze.cseabreeze
    except ImportError:
        pytest.skip("can't import cseabreeze")


def test_seabreeze_pyseabreeze_backend_available():
    """basically just tests if pyusb is installed"""
    try:
        import seabreeze.pyseabreeze
    except ImportError:
        pytest.skip("can't import pyseabreeze")


def test_seabreeze_cseabreeze_api_init(cseabreeze):
    """check if SeaBreezeAPI can be instantiated"""
    cseabreeze.SeaBreezeAPI()


@pytest.mark.usefixtures("mock_pyusb_core_find")
def test_seabreeze_pyseabreeze_api_init(pyseabreeze, pyseabreeze_pyusb_backend):
    """check if SeaBreezeAPI can be instantiated"""
    try:
        pyseabreeze.SeaBreezeAPI(_pyusb_backend=pyseabreeze_pyusb_backend)
    except RuntimeError:
        if pyseabreeze_pyusb_backend is None:
            raise  # fail if no backend library is available
        else:
            pytest.skip(f"pyusb_backend {pyseabreeze_pyusb_backend} not available")


def _get_class_public_interface_dict(backend):
    """return a dictionary with a set of all public functions for each feature"""
    base_class = backend.SeaBreezeFeature
    feature_classes = base_class.__subclasses__()
    interface_dict = {}
    for fcls in feature_classes:
        name = fcls.__name__
        attrs = {attr for attr in dir(fcls) if not attr.startswith("_")}
        interface_dict[name] = attrs
    return interface_dict


def test_seabreeze_compare_backend_feature_interfaces(cseabreeze, pyseabreeze):
    """test if the public backend feature class interfaces are identical"""
    c_feature_interface = _get_class_public_interface_dict(cseabreeze)
    py_feature_interface = _get_class_public_interface_dict(pyseabreeze)

    # test if the interfaces contain something
    assert c_feature_interface and py_feature_interface

    # test if all feature interfaces are non empty
    assert all(c_feature_interface.values()) and all(py_feature_interface.values())

    # test if there's a difference between cseabreeze and pyseabreeze features
    assert set(c_feature_interface) == set(py_feature_interface)

    CSEABREEZE_CUSTOM = {"feature_id"}
    PYSEABREEZE_CUSTOM = {"supports_protocol", "specialize"}

    for feature in c_feature_interface:
        c_attrs = c_feature_interface[feature] - CSEABREEZE_CUSTOM
        py_attrs = py_feature_interface[feature] - PYSEABREEZE_CUSTOM
        # for each feature test if there's a difference between cseabreeze and pyseabreeze
        assert c_attrs == py_attrs, f"feature {feature} differs in attrs"
