"""


"""
import enum
import itertools
from collections import defaultdict

from future.utils import with_metaclass

from seabreeze.pyseabreeze import features as sbf
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.features import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import OBPProtocol, OOIProtocol

# spectrometer models for pyseabreeze
#
from seabreeze.pyseabreeze.transport import TransportInterface, USBTransport

_model_class_registry = {}


class _SeaBreezeDeviceMeta(type):
    """metaclass for pyseabreeze devices"""

    def __new__(mcs, name, bases, attr_dict):
        # This part of the metaclass magic is very opaque. It could be avoided by moving all
        # of this logic to custom __init__ methods of the Spectrometer classes, or to factory
        # functions. But I am using python-seabreeze as a playground to experiment with
        # different ways of implementing extendable interfaces, so please forgive these design
        # decisions ^^"
        #
        # I also like the concise way you can define the spectrometer functionality this way.
        # It allows you to get a quick overview of the functionality implemented in pyseabreeze,
        # without having to read through a lot of code. And I think that it's easy to understand
        # what a spectrometer can do, by just looking at the spectrometer class definition, even
        # if you don't understand the details of the implementation.
        #
        # Because it's quite opaque the code below tries hard to enforce strict typing in all
        # defined spectrometer classes to minimize the amount of errors you can make when adding
        # a new spectrometer to pyseabreeze.
        #
        if name != "SeaBreezeDevice":
            # This runs for all subclasses of SeaBreezeDevice, so for all defined spectrometers
            # What might be unintuitive to the user is, that all defined attributes in the
            # spectrometer classes are only used to configure the subclass and are NOT directly
            # available in the instances later. (look at how attr_dict is modified below).
            #
            if "model_name" not in attr_dict:
                raise AttributeError(
                    "'model_name' not provided for class '{}'".format(name)
                )
            model_name = attr_dict.pop("model_name")
            if not isinstance(model_name, str):
                raise TypeError("{}.model_name not a str".format(name))

            # gather the transport classes defined on the class
            transport_classes = mcs._extract_transform_classes(
                model_name, class_name=name, attr_dict=attr_dict
            )
            # gather the feature classes defined on the class
            feature_classes = mcs._extract_feature_classes(
                model_name, class_name=name, attr_dict=attr_dict
            )

            if any(not attr.startswith("_") for attr in attr_dict):
                raise ValueError(
                    "can't define extra attrs on spectrometer classes: {}".format(
                        ", ".join(
                            attr for attr in attr_dict if not attr.startswith("_")
                        )
                    )
                )

            attr_dict = {
                "_model_name": model_name,
                "_transport_classes": transport_classes,
                "_feature_classes": feature_classes,
            }

        return super(_SeaBreezeDeviceMeta, mcs).__new__(mcs, name, bases, attr_dict)

    def __init__(cls, name, bases, attr_dict):
        if name != "SeaBreezeDevice":
            # > model name
            model_name = getattr(cls, "_model_name")
            assert isinstance(model_name, str), "model name not a str"
            _model_class_registry[model_name] = cls

        super(_SeaBreezeDeviceMeta, cls).__init__(name, bases, attr_dict)

    @staticmethod
    def _extract_transform_classes(model_name, class_name, attr_dict):

        visited_attrs = set()
        transport_classes = []
        try:
            supported_transport_classes = attr_dict.pop("transport")
        except KeyError:
            raise AttributeError("{}.transport not provided")
        if (
            not isinstance(supported_transport_classes, tuple)
            or not supported_transport_classes
        ):
            raise TypeError("{}.transport not a tuple of len > 0")

        for idx, transport_cls in enumerate(supported_transport_classes):
            # for each supported transport of the spectrometer, gather the configuration from
            # the spectrometer class and specialize the transport_cls with the provided settings.
            #
            if not issubclass(transport_cls, TransportInterface):
                raise TypeError(
                    "{}.transport[{:d}] '{}' does not derive from TransportInterface".format(
                        class_name, idx, transport_cls.__name__
                    )
                )
            # noinspection PyProtectedMember
            kwargs = transport_cls._required_init_kwargs
            transport_init_kwargs = {}
            for kw in kwargs:
                if kw not in attr_dict:
                    raise AttributeError(
                        "{}.{} not provided for class but '{}' requires it.".format(
                            class_name, kw, transport_cls.__name__
                        )
                    )
                transport_init_kwargs[kw] = attr_dict[kw]
            visited_attrs.update(kwargs)
            # specialize the transport class with the spectrometer's custom config
            specialized_transport_cls = transport_cls.specialize(
                model_name, **transport_init_kwargs
            )
            transport_classes.append(specialized_transport_cls)

        for attr in visited_attrs:
            del attr_dict[attr]

        return tuple(transport_classes)

    @staticmethod
    def _extract_feature_classes(model_name, class_name, attr_dict):

        visited_attrs = set()
        feature_classes = defaultdict(list)
        try:
            supported_feature_classes = attr_dict.pop("feature_classes")
        except KeyError:
            raise AttributeError("{}.feature_classes not provided")
        if (
            not isinstance(supported_feature_classes, tuple)
            or not supported_feature_classes
        ):
            raise TypeError("{}.feature_classes not a tuple of len > 0")
        for idx, feature_cls in enumerate(supported_feature_classes):
            # for each supported feature of the spectrometer, gather the configuration
            # from the spectrometer class and subclass the feature_cls with the provided
            # settings. Also check if requirements are fulfilled, i.e. if the feature depends
            # on other features or on specific protocols
            #
            if not issubclass(feature_cls, SeaBreezeFeature):
                raise TypeError(
                    "{}.feature_classes[{:d}] '{}' does not derive from SeaBreezeFeature".format(
                        model_name, idx, feature_cls.__name__
                    )
                )
            # noinspection PyProtectedMember
            required = set(feature_cls._required_features)
            if not required.issubset(feature_classes):
                raise KeyError(
                    "{}.feature_classes[{:d}] '{}' requires '{}'. To fix, re-order or add.".format(
                        model_name,
                        idx,
                        feature_cls.__name__,
                        ", ".join(required - set(feature_classes)),
                    )
                )
            # noinspection PyProtectedMember
            kwargs = feature_cls._required_kwargs
            feature_attrs = {}
            for kw in kwargs:
                if kw not in attr_dict:
                    raise AttributeError(
                        "{}.{} not provided for class but '{}' requires it.".format(
                            class_name, kw, feature_cls.__name__
                        )
                    )
                feature_attrs[kw] = attr_dict[kw]
            visited_attrs.update(kwargs)
            # specialize the feature class with the spectrometer's custom config
            specialized_feature_cls = feature_cls.specialize(
                model_name, **feature_attrs
            )
            feature_classes[feature_cls.identifier].append(specialized_feature_cls)

        for attr in visited_attrs:
            del attr_dict[attr]

        return feature_classes


class EndPointMap(object):
    """internal endpoint map for spectrometer classes"""

    def __init__(
        self, ep_out=None, lowspeed_in=None, highspeed_in=None, highspeed_in2=None
    ):
        self.primary_out = self.ep_out = ep_out
        self.primary_in = self.lowspeed_in = lowspeed_in
        self.secondary_out = ep_out
        self.secondary_in = self.highspeed_in = highspeed_in
        self.secondary_in2 = self.highspeed_in2 = highspeed_in2


class DarkPixelIndices(tuple):
    """internal dark pixel range class"""

    def __new__(cls, indices):
        """dark pixel indices

        Parameters
        ----------
        indices : iterable
            index of electric dark pixel
        """
        return super(DarkPixelIndices, cls).__new__(
            DarkPixelIndices, sorted(set(indices))
        )

    @classmethod
    def from_ranges(cls, *ranges):
        """return dark pixes indices from ranges

        Parameters
        ----------
        *ranges : (`int`, `int`)
            ranges of electric dark pixels
        """
        dp = itertools.chain(*(range(low, high) for (low, high) in ranges))
        # noinspection PyArgumentList
        return cls(dp)


class TriggerMode(enum.IntEnum):
    """internal trigger modes enum"""

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

    @classmethod
    def supported(cls, *mode_strings):
        return set(getattr(cls, mode_string) for mode_string in mode_strings)


class SeaBreezeDevice(with_metaclass(_SeaBreezeDeviceMeta)):

    # internal attribute
    _model_name = None
    _serial_number = "?"
    _cached_features = None

    def __new__(cls, raw_device=None):
        if raw_device is None:
            raise SeaBreezeError(
                "Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`."
            )
        for transport in {USBTransport}:
            supported_model = transport.supported_model(raw_device)
            if supported_model is not None:
                break
        else:
            raise TypeError("No transport supports device.")
        specialized_cls = _model_class_registry[supported_model]
        return super(SeaBreezeDevice, cls).__new__(specialized_cls)

    def __init__(self, raw_device=None):
        if raw_device is None:
            raise SeaBreezeError(
                "Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`."
            )
        self._raw_device = raw_device
        for transport in self._transport_classes:
            if transport.supported_model(self._raw_device) is not None:
                self._transport = transport()
                break
        else:
            raise TypeError("No transport supports device.")
        try:
            self._serial_number = self.get_serial_number()
        except SeaBreezeError:
            pass

    def __del__(self):
        self.close()

    @property
    def model(self):
        return self._model_name

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
        self._transport.open_device(self._raw_device)
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
        self._transport.close_device()

    @property
    def is_open(self):
        """returns if the spectrometer device usb connection is opened

        Returns
        -------
        bool
        """
        return self._transport.is_open

    def get_serial_number(self):
        """return the serial number string of the spectrometer

        Returns
        -------
        serial_number: str
        """
        try:
            # noinspection PyProtectedMember
            protocol = self._transport._protocol
            if protocol is None:
                raise AttributeError("transport not opened")

            elif isinstance(protocol, OOIProtocol):
                # The serial is stored in slot 0
                # noinspection PyUnresolvedReferences
                return self.f.eeprom.eeprom_read_slot(0)

            elif isinstance(protocol, OBPProtocol):
                return protocol.query(0x00000100).decode("utf8")

            else:
                raise NotImplementedError(
                    "No serial number for protocol class {}".format(
                        protocol.__class__.__name__
                    )
                )
        except AttributeError:
            raise SeaBreezeError("device not open")

    @property
    def features(self):
        """return a dictionary of all supported features

        this returns a dictionary with all supported Features of the spectrometer

        Returns
        -------
        features : `dict` [`str`, `seabreeze.cseabreeze.SeaBreezeFeature`]
        """
        if not self._cached_features:
            # noinspection PyProtectedMember
            protocol = self._transport._protocol
            self._cached_features = {}
            for identifier in sbf.SeaBreezeFeature.get_feature_class_registry():
                f_list = self._cached_features.setdefault(identifier, [])
                for feature_cls in self._feature_classes[identifier]:
                    assert (
                        issubclass(feature_cls, sbf.SeaBreezeFeature)
                        and identifier == feature_cls.identifier
                    )
                    if not feature_cls.supports_protocol(protocol):
                        continue
                    # noinspection PyProtectedMember
                    f_list.append(feature_cls(self._transport._protocol, len(f_list)))
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
                    setattr(
                        self, identifier, features[0] if features else None
                    )  # TODO: raise FeatureNotAvailable?

        return FeatureAccessHandler(self.features)


# SPECTROMETER DEFINITIONS
# ========================
#
class USB2000PLUS(SeaBreezeDevice):

    model_name = "USB2000PLUS"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x101E
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((6, 21))  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureUSB2000PLUS,
        sbf.continuousstrobe.SeaBreezeContinuousStrobeFeatureOOI,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class USB2000(SeaBreezeDevice):

    model_name = "USB2000"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1002
    usb_endpoint_map = EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((2, 24))
    integration_time_min = 3000
    integration_time_max = 655350000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 4095
    trigger_modes = TriggerMode.supported("NORMAL", "SOFTWARE", "HARDWARE")

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureUSB2000,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class HR2000(SeaBreezeDevice):

    model_name = "HR2000"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x100A
    usb_endpoint_map = EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((2, 24))
    integration_time_min = 3000
    integration_time_max = 655350000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 4095
    trigger_modes = TriggerMode.supported("NORMAL", "SOFTWARE", "HARDWARE")

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureHR2000,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class HR4000(SeaBreezeDevice):

    model_name = "HR4000"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1012
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((2, 13))
    integration_time_min = 10
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 3840
    spectrum_raw_length = (3840 * 2) + 1
    spectrum_max_value = 16383
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureHR4000,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class HR2000PLUS(SeaBreezeDevice):

    model_name = "HR2000PLUS"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1016
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((2, 24))
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 16383
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureHR2000PLUS,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class USB650(SeaBreezeDevice):

    model_name = "USB650"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1014
    usb_endpoint_map = EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 3000
    integration_time_max = 655350000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 4095
    trigger_modes = TriggerMode.supported("NORMAL", "SOFTWARE", "HARDWARE")

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureUSB650,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
        sbf.continuousstrobe.SeaBreezeContinuousStrobeFeatureOOI,
    )


class QE65000(SeaBreezeDevice):

    model_name = "QE65000"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1018
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges(
        (0, 4), (1040, 1044)
    )  # as in seabreeze-3.0.5
    integration_time_min = 8000
    integration_time_max = 1600000000
    integration_time_base = 1000
    spectrum_num_pixel = 1280
    spectrum_raw_length = (1024 + 256) * 2 + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported("NORMAL", "SOFTWARE", "HARDWARE")

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureQE65000,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class USB4000(SeaBreezeDevice):

    model_name = "USB4000"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1022
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = (
        DarkPixelIndices.from_ranges((5, 16)),
    )  # as in seabreeze-3.0.9
    integration_time_min = 10
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 3840
    spectrum_raw_length = (3840 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureUSB4000,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class NIRQUEST512(SeaBreezeDevice):

    model_name = "NIRQUEST512"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1026
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = (DarkPixelIndices.from_ranges(),)  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 1600000000
    integration_time_base = 1000
    spectrum_num_pixel = 512
    spectrum_raw_length = (512 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureNIRQUEST512,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class NIRQUEST256(SeaBreezeDevice):

    model_name = "NIRQUEST256"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1028
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = (DarkPixelIndices.from_ranges(),)  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 1600000000
    integration_time_base = 1000
    spectrum_num_pixel = 256
    spectrum_raw_length = (256 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureNIRQUEST256,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class MAYA2000PRO(SeaBreezeDevice):

    model_name = "MAYA2000PRO"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x102A
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((0, 4), (2064, 2068))
    integration_time_min = 7200
    integration_time_max = 65000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 64000
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureMAYA2000PRO,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class MAYA2000(SeaBreezeDevice):

    model_name = "MAYA2000"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x102C
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((0, 8), (2072, 2080))
    integration_time_min = 15000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported("NORMAL", "SOFTWARE", "HARDWARE")

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureMAYA2000,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class TORUS(SeaBreezeDevice):

    model_name = "TORUS"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1040
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureTORUS,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class APEX(SeaBreezeDevice):

    model_name = "APEX"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1044
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((0, 4), (2064, 2068))
    integration_time_min = 15000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 64000
    trigger_modes = TriggerMode.supported("NORMAL",)

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureAPEX,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class MAYALSL(SeaBreezeDevice):

    model_name = "MAYALSL"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x1046
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((0, 4), (2064, 2068))
    integration_time_min = 7200
    integration_time_max = 65000000
    integration_time_base = 1
    spectrum_num_pixel = 2304
    spectrum_raw_length = (2304 * 2) + 1
    spectrum_max_value = 64000
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureMAYALSL,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class JAZ(SeaBreezeDevice):

    model_name = "JAZ"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x2000
    usb_endpoint_map = EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82)
    usb_protocol = OOIProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((2, 24))
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = 2048 * 2  # XXX: No Sync byte!
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureJAZ,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class STS(SeaBreezeDevice):

    model_name = "STS"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x4000
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81
    )  # XXX: we'll ignore the alternative EPs
    usb_protocol = OBPProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 10
    integration_time_max = 85000000
    integration_time_base = 1
    spectrum_num_pixel = 1024
    spectrum_raw_length = 1024 * 2
    spectrum_max_value = 16383
    trigger_modes = TriggerMode.supported("OBP_NORMAL", "OBP_EXTERNAL", "OBP_INTERNAL")

    # features
    feature_classes = (
        sbf.spectrometer.SeaBreezeSpectrometerFeatureSTS,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class QEPRO(SeaBreezeDevice):

    model_name = "QEPRO"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x4004
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81
    )  # XXX: we'll ignore the alternative EPs
    usb_protocol = OBPProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((0, 4), (1040, 1044))
    integration_time_min = 10000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 1044
    spectrum_raw_length = (1044 * 4) + 32  # XXX: Metadata
    spectrum_max_value = (2 ** 18) - 1
    trigger_modes = TriggerMode.supported("NORMAL", "LEVEL", "SYNCHRONIZATION", "EDGE")

    # features
    feature_classes = (
        sbf.spectrometer.SeaBreezeSpectrometerFeatureQEPRO,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class VENTANA(SeaBreezeDevice):

    model_name = "VENTANA"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x5000
    usb_endpoint_map = EndPointMap(ep_out=0x01, lowspeed_in=0x82)
    usb_protocol = OBPProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 22000
    integration_time_max = 60000000
    integration_time_base = 1
    spectrum_num_pixel = 1024
    spectrum_raw_length = 1024 * 2  # XXX: No Sync byte!
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported("NORMAL", "LEVEL", "SYNCHRONIZATION", "EDGE")

    # features
    feature_classes = (
        sbf.spectrometer.SeaBreezeSpectrometerFeatureVENTANA,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class SPARK(SeaBreezeDevice):

    model_name = "SPARK"

    # communication config
    transport = (USBTransport,)
    usb_product_id = 0x4200
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81
    )  # XXX: we'll ignore the alternative EPs
    usb_protocol = OBPProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 10
    integration_time_max = 85000000
    integration_time_base = 1
    spectrum_num_pixel = 1024
    spectrum_raw_length = (1024 * 2) + 64  # XXX: Metadata
    spectrum_max_value = 16383
    trigger_modes = TriggerMode.supported("OBP_NORMAL", "OBP_EXTERNAL", "OBP_INTERNAL")

    # features
    feature_classes = (
        sbf.spectrometer.SeaBreezeSpectrometerFeatureSPARK,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )
