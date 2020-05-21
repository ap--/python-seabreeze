import pytest


@pytest.fixture(scope="module")
def cseabreeze():
    yield pytest.importorskip("seabreeze.cseabreeze")


@pytest.fixture(scope="module")
def pyseabreeze():
    yield pytest.importorskip("seabreeze.pyseabreeze")


def _get_class_public_interface_dict(backend):
    base_class = backend.SeaBreezeFeature
    feature_classes = base_class.__subclasses__()
    interface_dict = {}
    for fcls in feature_classes:
        name = fcls.__name__
        attrs = set(attr for attr in dir(fcls) if not attr.startswith("_"))
        interface_dict[name] = attrs
    return interface_dict


def test_backend_features_interface(cseabreeze, pyseabreeze):
    """test if the public backend feature class interfaces are identical"""
    c_feature_interface = _get_class_public_interface_dict(cseabreeze)
    py_feature_interface = _get_class_public_interface_dict(pyseabreeze)

    assert set(c_feature_interface) == set(py_feature_interface)

    CSEABREEZE_CUSTOM = {"feature_id"}
    PYSEABREEZE_CUSTOM = {"supports_protocol", "specialize"}

    for feature in c_feature_interface:
        c_attrs = c_feature_interface[feature] - CSEABREEZE_CUSTOM
        py_attrs = py_feature_interface[feature] - PYSEABREEZE_CUSTOM
        assert c_attrs == py_attrs, "feature {} differs in attrs".format(feature)


def test_cseabreeze_seabreezeapi(cseabreeze):
    """check if list_devices doesn't crash"""
    api = cseabreeze.SeaBreezeAPI()
    api.list_devices()


def test_pyseabreeze_seabreezeapi(pyseabreeze):
    """check if list_devices doesn't crash"""
    api = pyseabreeze.SeaBreezeAPI()
    api.list_devices()
