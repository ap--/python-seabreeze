"""


"""
import itertools
import struct
import time

from future.utils import with_metaclass
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.communication import USBCommBase, USBCommOOI, USBCommOBP
from seabreeze.pyseabreeze import features as sbfeatures

# spectrometer models for pyseabreeze
#
VENDOR_ID = 0x2457
# NOTE: PRODUCT_IDS and _model_registry will be filled
#       via the SeaBreezeDevice metaclass below
PRODUCT_IDS = set()
_model_class_registry = {}


def is_ocean_optics_usb_device(dev):
    """return if the provided device is a supported ocean optics device

    Parameters
    ----------
    dev : usb.core.Device

    Returns
    -------
    bool
    """
    # noinspection PyUnresolvedReferences
    return dev.idVendor == VENDOR_ID and dev.idProduct in PRODUCT_IDS


class _SeaBreezeDeviceMeta(type):
    """metaclass for pyseabreeze devices"""

    def __new__(mcs, name, bases, attr_dict):
        if name != 'SeaBreezeDevice':
            # > the command interface
            _interface_cls = attr_dict['interface_cls']
            assert issubclass(_interface_cls, USBCommBase), "instance class does not derive from USBCommBase"

            # make the seabreeze device derive from the interface_cls
            bases = tuple(b for b in itertools.chain(bases, [_interface_cls]))

        return super(_SeaBreezeDeviceMeta, mcs).__new__(mcs, name, bases, attr_dict)

    def __init__(cls, name, bases, attr_dict):
        if name != 'SeaBreezeDevice':
            # check if required class attributes are present and correctly typed
            # > product_id
            _product_id = attr_dict['product_id']
            assert isinstance(_product_id, int), "product_od not an int"
            assert 0 <= _product_id <= 0xFFFF, "product_id not a 16bit int"
            assert _product_id not in PRODUCT_IDS, "product_id already registered"
            # > usb endpoint map
            _endpoint_map = attr_dict['endpoint_map']
            assert isinstance(_endpoint_map, _EndPointMap), "no endpoint map provided"
            # > model name
            _model_name = attr_dict['model_name']
            assert isinstance(_model_name, str), "model name not a str"

            # add to the class registry
            PRODUCT_IDS.add(_product_id)
            _model_class_registry[_product_id] = cls

        super(_SeaBreezeDeviceMeta, cls).__init__(name, bases, attr_dict)


class _EndPointMap(object):
    """internal endpoint map for spectrometer classes"""
    def __init__(self, ep_out=None, lowspeed_in=None, highspeed_in=None, highspeed_in2=None):
        self.primary_out = self.ep_out = ep_out
        self.primary_in = self.lowspeed_in = lowspeed_in
        self.secondary_out = ep_out
        self.secondary_in = self.highspeed_in = highspeed_in
        self.secondary_in2 = self.highspeed_in2 = highspeed_in2


class _DarkPixelRanges(tuple):
    """internal dark pixel range class"""
    def __new__(cls, *ranges):
        dp = itertools.chain(*(range(low, high) for (low, high) in ranges))
        return super(_DarkPixelRanges, cls).__new__(_DarkPixelRanges, dp)


class _TriggerMode(object):
    """internal trigger modes class"""
    NORMAL = 0x00
    SOFTWARE = 0x01
    LEVEL = 0x01
    SYNCHRONIZATION = 0x02
    HARDWARE = 0x03
    EDGE = 0x03
    SINGLE_SHOT = 0x04
    SELF_NORMAL = 0x80
    SELF_SOFTWARE = 0x81
    SELF_SYNCHRONIZATION = 0x82
    SELF_HARDWARE = 0x83
    DISABLED = 0xFF
    OBP_NORMAL = 0x00
    OBP_EXTERNAL = 0x01
    OBP_INTERNAL = 0x02


class SeaBreezeDevice(with_metaclass(_SeaBreezeDeviceMeta)):

    # attributes have to be defined in derived classes
    product_id = None
    model_name = None
    endpoint_map = None
    interface_cls = None

    # internal attribute
    _serial_number = '?'
    _cached_features = None

    def __new__(cls, handle=None):
        if handle is None:
            raise SeaBreezeError("Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`.")
        specialized_cls = _model_class_registry[handle.idProduct]
        return super(SeaBreezeDevice, cls).__new__(specialized_cls, handle)

    def __init__(self, handle=None):
        if handle is None:
            raise SeaBreezeError("Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`.")
        self.handle = handle
        try:
            self._serial_number = self.get_serial_number()
        except SeaBreezeError:
            pass

    @property
    def model(self):
        return self.model_name

    @property
    def serial_number(self):
        return self._serial_number

    def __repr__(self):
        return "<SeaBreezeDevice %s:%s>" % (self.model, self.serial_number)

    def open(self):
        """open the spectrometer usb connection

        Returns
        -------
        None
        """
        self.open_device(self.handle)
        if issubclass(self.interface_cls, USBCommOOI):
            # initialize the spectrometer
            self.usb_send(struct.pack('<B', 0x01))
            time.sleep(0.1)  # wait shortly after init command
        # cache features
        self._cached_features = {}
        _ = self.features
        # get serial
        self._serial_number = self.get_serial_number()

    def close(self):
        """close the spectrometer usb connection

        Returns
        -------
        None
        """
        self.close_device()

    @property
    def is_open(self):
        """returns if the spectrometer device usb connection is opened

        Returns
        -------
        bool
        """
        return self._is_open()

    def get_serial_number(self):
        """return the serial number string of the spectrometer

        Returns
        -------
        serial_number: str
        """
        try:
            if issubclass(self.interface_cls, USBCommOOI):
                # The serial is stored in slot 0
                # noinspection PyUnresolvedReferences
                return str(self.f.eeprom.eeprom_read_slot(0))

            elif issubclass(self.interface_cls, USBCommOBP):
                return self.query(0x00000100, "")

            else:
                raise NotImplementedError("No serial number for interface class %s" % str(self.interface_cls))
        except AttributeError:
            raise SeaBreezeError("device not open")

    def get_model(self):
        """return the model string of the spectrometer

        Returns
        -------
        model: str
        """
        return self.model_name

    @property
    def features(self):
        """return a dictionary of all supported features

        this returns a dictionary with all supported Features of the spectrometer

        Returns
        -------
        features : `dict` [`str`, `seabreeze.cseabreeze.SeaBreezeFeature`]
        """
        # TODO: make this a cached property
        if not self._cached_features:
            self._cached_features = {k: [] for k in sbfeatures.SeaBreezeFeature.get_feature_class_registry()}
            for feature_cls in self.feature_classes:
                f_list = self._cached_features.setdefault(feature_cls.identifier, [])
                f_list.append(feature_cls(self, len(f_list)))
        return self._cached_features

    @property
    def f(self):
        """convenience access to features via attributes

        this allows you to access a feature like this::

            # via .features
            device.features['spectrometer'][0].get_intensities()
            # via .f
            device.f.spectrometer.get_intensities()

        """
        class FeatureAccessHandler(object):
            def __init__(self, feature_dict):
                for identifier, features in feature_dict.items():
                    setattr(self, identifier, features[0] if features else None)  # TODO: raise FeatureNotAvailable?
        return FeatureAccessHandler(self.features)


# SPECTROMETER DEFINITIONS
# ========================
#
class USB2000PLUS(SeaBreezeDevice):

    # communication config
    product_id = 0x101E
    model_name = 'USB2000PLUS'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((6, 21))  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureUSB2000PLUS,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class USB2000(SeaBreezeDevice):

    # communication config
    product_id = 0x1002
    model_name = 'USB2000'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((2, 24))
    integration_time_min = 3000
    integration_time_max = 655350000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 4095
    trigger_modes = ('NORMAL', 'SOFTWARE', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureUSB2000,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class HR2000(SeaBreezeDevice):

    # communication config
    product_id = 0x100a
    model_name = 'HR2000'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((2, 24))
    integration_time_min = 3000
    integration_time_max = 655350000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 4095
    trigger_modes = ('NORMAL', 'SOFTWARE', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureHR2000,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class HR4000(SeaBreezeDevice):

    # communication config
    product_id = 0x1012
    model_name = 'HR4000'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((2, 13))
    integration_time_min = 10
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 3840
    spectrum_raw_length = (3840 * 2) + 1
    spectrum_max_value = 16383
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureHR4000,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class HR2000PLUS(SeaBreezeDevice):

    # communication config
    product_id = 0x1016
    model_name = 'HR2000PLUS'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((2, 24))
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 16383
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureHR2000PLUS,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class USB650(SeaBreezeDevice):

    # communication config
    product_id = 0x1014
    model_name = 'USB650'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges()
    integration_time_min = 3000
    integration_time_max = 655350000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 4095
    trigger_modes = ('NORMAL', 'SOFTWARE', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureUSB650,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class QE65000(SeaBreezeDevice):

    # communication config
    product_id = 0x1018
    model_name = 'QE65000'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((0, 4), (1040, 1044))  # as in seabreeze-3.0.5
    integration_time_min = 8000
    integration_time_max = 1600000000
    integration_time_base = 1000
    spectrum_num_pixel = 1280
    spectrum_raw_length = (1024 + 256)*2 + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureQE65000,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class USB4000(SeaBreezeDevice):

    # communication config
    product_id = 0x1022
    model_name = 'USB4000'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((5, 16)),  # as in seabreeze-3.0.9
    integration_time_min = 10
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 3840
    spectrum_raw_length = (3840 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureUSB4000,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class NIRQUEST512(SeaBreezeDevice):

    # communication config
    product_id = 0x1026
    model_name = 'NIRQUEST512'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges(),  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 1600000000
    integration_time_base = 1000
    spectrum_num_pixel = 512
    spectrum_raw_length = (512 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureNIRQUEST512,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class NIRQUEST256(SeaBreezeDevice):

    # communication config
    product_id = 0x1028
    model_name = 'NIRQUEST256'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges(),  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 1600000000
    integration_time_base = 1000
    spectrum_num_pixel = 256
    spectrum_raw_length = (256 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureNIRQUEST256,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class MAYA2000PRO(SeaBreezeDevice):

    # communication config
    product_id = 0x102a
    model_name = 'MAYA2000PRO'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((0, 4), (2064, 2068))
    integration_time_min = 7200
    integration_time_max = 65000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 64000
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureMAYA2000PRO,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class MAYA2000(SeaBreezeDevice):

    # communication config
    product_id = 0x102c
    model_name = 'MAYA2000'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((0, 8), (2072, 2080))
    integration_time_min = 15000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureMAYA2000,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class TORUS(SeaBreezeDevice):

    # communication config
    product_id = 0x1040
    model_name = 'TORUS'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges()
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureTORUS,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class APEX(SeaBreezeDevice):

    # communication config
    product_id = 0x1044
    model_name = 'APEX'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((0, 4), (2064, 2068))
    integration_time_min = 15000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 64000
    trigger_modes = ('NORMAL', )

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureAPEX,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class MAYALSL(SeaBreezeDevice):

    # communication config
    product_id = 0x1046
    model_name = 'MAYALSL'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((0, 4), (2064, 2068))
    integration_time_min = 7200
    integration_time_max = 65000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 64000
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureMAYALSL,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class JAZ(SeaBreezeDevice):

    # communication config
    product_id = 0x2000
    model_name = 'JAZ'
    interface_cls = USBCommOOI
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((2, 24))
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2)  # XXX: No Sync byte!
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'SOFTWARE', 'SYNCHRONIZATION', 'HARDWARE')

    # features
    feature_classes = (
        sbfeatures.eeprom.SeaBreezeEEPromFeatureOOI,
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureJAZ,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class STS(SeaBreezeDevice):

    # communication config
    product_id = 0x4000
    model_name = 'STS'
    interface_cls = USBCommOBP
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81)  # XXX: we'll ignore the alternative EPs

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges()
    integration_time_min = 10
    integration_time_max = 85000000
    integration_time_base = 1
    spectrum_num_pixel = 1024
    spectrum_raw_length = (1024 * 2)
    spectrum_max_value = 16383
    trigger_modes = ('OBP_NORMAL', 'OBP_EXTERNAL', 'OBP_INTERNAL')

    # features
    feature_classes = (
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureSTS,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class QEPRO(SeaBreezeDevice):

    # communication config
    product_id = 0x4004
    model_name = 'QEPRO'
    interface_cls = USBCommOBP
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x81)  # XXX: we'll ignore the alternative EPs

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges((0, 4), (1040, 1044))
    integration_time_min = 10000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 1044
    spectrum_raw_length = (1044 * 4) + 32  # XXX: Metadata
    spectrum_max_value = (2**18)-1
    trigger_modes = ('NORMAL', 'LEVEL', 'SYNCHRONIZATION', 'EDGE')

    # features
    feature_classes = (
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureQEPRO,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )


class VENTANA(SeaBreezeDevice):

    # communication config
    product_id = 0x5000
    model_name = 'VENTANA'
    interface_cls = USBCommOBP
    endpoint_map = _EndPointMap(ep_out=0x01, lowspeed_in=0x82)

    # spectrometer config
    dark_pixel_indices = _DarkPixelRanges()
    integration_time_min = 22000
    integration_time_max = 60000000
    integration_time_base = 1
    spectrum_num_pixel = 1024
    spectrum_raw_length = (1024 * 2)  # XXX: No Sync byte!
    spectrum_max_value = 65535
    trigger_modes = ('NORMAL', 'LEVEL', 'SYNCHRONIZATION', 'EDGE')

    # features
    feature_classes = (
        sbfeatures.spectrometer.SeaBreezeSpectrometerFeatureVENTANA,
        sbfeatures.rawusb.SeaBreezeRawUSBAccessFeature,
    )
