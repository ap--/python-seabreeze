"""device classes for pyseabreeze

Author: Andreas Poehlmann
"""
from __future__ import annotations

import enum
import itertools
from collections import defaultdict
from typing import Any
from typing import Iterable
from typing import Tuple
from typing import TypeVar

from seabreeze.pyseabreeze import features as sbf
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.features import SeaBreezeFeature
from seabreeze.pyseabreeze.protocol import ADCProtocol
from seabreeze.pyseabreeze.protocol import OBP2Protocol
from seabreeze.pyseabreeze.protocol import OBPProtocol
from seabreeze.pyseabreeze.protocol import OOIProtocol
from seabreeze.pyseabreeze.transport import USBTransport
from seabreeze.pyseabreeze.types import PySeaBreezeTransport
from seabreeze.types import SeaBreezeFeatureAccessor

# class registry for all spectrometer models

_model_class_registry: dict[str, type[SeaBreezeDevice]] = {}


class _SeaBreezeDeviceMeta(type):
    """metaclass for pyseabreeze devices"""

    def __new__(
        mcs: type[_SeaBreezeDeviceMeta],
        name: str,
        bases: tuple[Any],
        attr_dict: dict[str, Any],
    ) -> _SeaBreezeDeviceMeta:
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
                raise AttributeError(f"'model_name' not provided for class '{name}'")
            model_name = attr_dict.pop("model_name")
            if not isinstance(model_name, str):
                raise TypeError(f"{name}.model_name not a str")

            new_attr_dict: dict[str, Any] = {
                "_model_name": model_name,
            }

            if any(base is SeaBreezeDevice for base in bases):
                # gather the transport classes defined on the class
                transport_classes = mcs._extract_transform_classes(
                    model_name, class_name=name, attr_dict=attr_dict
                )
                new_attr_dict["_transport_classes"] = transport_classes
            # gather the feature classes defined on the class
            feature_classes = mcs._extract_feature_classes(
                model_name, class_name=name, attr_dict=attr_dict
            )
            new_attr_dict["_feature_classes"] = feature_classes

            try:
                # move classmethod to new class for dynamic model class subsitution
                k = "_substitute_compatible_subclass"
                new_attr_dict[k] = attr_dict[k]
            except KeyError:
                pass

            if any(not attr.startswith("_") for attr in attr_dict):
                raise ValueError(
                    "can't define extra attrs on spectrometer classes: {}".format(
                        ", ".join(
                            attr for attr in attr_dict if not attr.startswith("_")
                        )
                    )
                )
            else:
                attr_dict = new_attr_dict

        return super().__new__(mcs, name, bases, attr_dict)

    def __init__(cls, name: str, bases: tuple[Any], attr_dict: dict[str, Any]) -> None:
        if name != "SeaBreezeDevice":
            # > model name
            model_name = getattr(cls, "_model_name")
            assert isinstance(model_name, str), "model name not a str"
            _model_class_registry[model_name] = cls  # type: ignore

        super().__init__(name, bases, attr_dict)

    @staticmethod
    def _extract_transform_classes(
        model_name: str, class_name: str, attr_dict: dict[str, Any]
    ) -> tuple[type[PySeaBreezeTransport[Any]], ...]:
        visited_attrs = set()
        transport_classes = []
        try:
            supported_transport_classes = attr_dict.pop("transport")
        except KeyError:
            raise AttributeError(f"{class_name}.transport not provided")
        if (
            not isinstance(supported_transport_classes, tuple)
            or not supported_transport_classes
        ):
            raise TypeError(f"{class_name}.transport not a tuple of len > 0")

        for idx, transport_cls in enumerate(supported_transport_classes):
            # for each supported transport of the spectrometer, gather the configuration from
            # the spectrometer class and specialize the transport_cls with the provided settings.
            #
            if not issubclass(transport_cls, PySeaBreezeTransport):
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
    def _extract_feature_classes(
        model_name: str, class_name: str, attr_dict: dict[str, Any]
    ) -> dict[str, list[type[SeaBreezeFeature]]]:
        visited_attrs = set()
        feature_classes: defaultdict[str, list[type[SeaBreezeFeature]]] = defaultdict(
            list
        )
        try:
            supported_feature_classes = attr_dict.pop("feature_classes")
        except KeyError:
            raise AttributeError(f"{class_name}.feature_classes not provided")
        if (
            not isinstance(supported_feature_classes, tuple)
            or not supported_feature_classes
        ):
            raise TypeError(f"{class_name}.feature_classes not a tuple of len > 0")
        for idx, feature_cls in enumerate(supported_feature_classes):
            # for each supported feature of the spectrometer, gather the configuration
            # from the spectrometer class and subclass the feature_cls with the provided
            # settings. Also check if requirements are fulfilled, i.e. if the feature depends
            # on other features or on specific protocols
            #
            if not issubclass(feature_cls, SeaBreezeFeature):
                raise TypeError(
                    "{}.feature_classes[{:d}] '{}' does not derive from SeaBreezeFeature".format(
                        class_name, idx, feature_cls.__name__
                    )
                )
            # noinspection PyProtectedMember
            required = set(feature_cls._required_features)
            if not required.issubset(feature_classes):
                raise KeyError(
                    "{}.feature_classes[{:d}] '{}' requires '{}'. To fix, re-order or add.".format(
                        class_name,
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


class EndPointMap:
    """internal endpoint map for spectrometer classes"""

    def __init__(
        self,
        ep_out: int | None = None,
        lowspeed_in: int | None = None,
        highspeed_in: int | None = None,
        highspeed_in2: int | None = None,
    ) -> None:
        self.primary_out = self.ep_out = ep_out
        self.primary_in = self.lowspeed_in = lowspeed_in
        self.secondary_out = ep_out
        self.secondary_in = self.highspeed_in = highspeed_in
        self.secondary_in2 = self.highspeed_in2 = highspeed_in2


class DarkPixelIndices(Tuple[int, ...]):
    """internal dark pixel range class"""

    def __new__(
        cls: type[DarkPixelIndices], indices: Iterable[int]
    ) -> DarkPixelIndices:
        """dark pixel indices

        Parameters
        ----------
        indices : iterable
            index of electric dark pixel
        """
        return super().__new__(DarkPixelIndices, sorted(set(indices)))  # type: ignore

    @classmethod
    def from_ranges(cls, *ranges: tuple[int, int]) -> DarkPixelIndices:
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
    OBP_EDGE = 0x01
    OBP_LEVEL = 0x03

    @classmethod
    def supported(cls, *mode_strings: str) -> set[TriggerMode]:
        return {getattr(cls, mode_string) for mode_string in mode_strings}


DT = TypeVar("DT", bound="SeaBreezeDevice")


class SeaBreezeDevice(metaclass=_SeaBreezeDeviceMeta):
    # internal attribute
    _model_name = None
    _serial_number = "?"
    _cached_features: dict[str, list[SeaBreezeFeature]] | None = None
    _transport_classes: tuple[type[PySeaBreezeTransport[Any]], ...]
    _feature_classes: dict[str, list[type[SeaBreezeFeature]]]

    def __new__(cls: type[DT], raw_device: Any = None) -> SeaBreezeDevice:
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
        return super().__new__(specialized_cls)

    def __init__(self, raw_device: Any = None) -> None:
        if raw_device is None:
            raise SeaBreezeError(
                "Don't instantiate SeaBreezeDevice directly. Use `SeabreezeAPI.list_devices()`."
            )
        self._raw_device = raw_device
        for transport in self._transport_classes:
            if transport.supported_model(self._raw_device) is not None:
                self._transport: PySeaBreezeTransport[Any] = transport()
                break
        else:
            raise TypeError("No transport supports device.")
        try:
            # sneakily switch in the correct subclass in case it's needed
            self.__class__ = self.__class__._substitute_compatible_subclass(
                self._transport
            )
        except AttributeError:
            pass
        try:
            self._serial_number = self.get_serial_number()
        except SeaBreezeError:
            pass

    @property
    def model(self) -> str:
        if self._model_name is None:
            raise RuntimeError("model can't be None")
        return self._model_name

    @property
    def serial_number(self) -> str:
        return self._serial_number

    @classmethod
    def _substitute_compatible_subclass(
        cls: type[DT], transport: PySeaBreezeTransport[Any]
    ) -> type[DT]:
        return cls

    def __repr__(self) -> str:
        return f"<SeaBreezeDevice {self.model}:{self.serial_number}>"

    def open(self) -> None:
        """open the spectrometer usb connection

        Returns
        -------
        None
        """
        self._transport.open_device(self._raw_device)
        # substitute subclass if needed
        self.__class__ = self.__class__._substitute_compatible_subclass(self._transport)
        # cache features
        self._cached_features = None
        _ = self.features
        # get serial
        self._serial_number = self.get_serial_number()

    def close(self) -> None:
        """close the spectrometer usb connection

        Returns
        -------
        None
        """
        if self.is_open:
            self._transport.close_device()

    @property
    def is_open(self) -> bool:
        """returns if the spectrometer device usb connection is opened

        Returns
        -------
        bool
        """
        return self._transport.is_open

    def get_serial_number(self) -> str:
        """return the serial number string of the spectrometer

        Returns
        -------
        serial_number: str
        """
        try:
            protocol = self._transport.protocol
        except RuntimeError:
            raise SeaBreezeError("device not open")

        if isinstance(protocol, OOIProtocol) or isinstance(protocol, ADCProtocol):
            # The serial is stored in slot 0
            return self.f.eeprom.eeprom_read_slot(0)

        elif isinstance(protocol, OBP2Protocol):
            serial_str = protocol.query(0x00000100).strip()
            return serial_str.decode("utf8")

        elif isinstance(protocol, OBPProtocol):
            serial_len = ord(protocol.query(0x00000101))
            serial_str = protocol.query(0x00000100)[:serial_len]
            return serial_str.decode("utf8")

        else:
            raise NotImplementedError(
                f"No serial number for protocol class {type(protocol).__name__}"
            )

    @property
    def features(self) -> dict[str, list[SeaBreezeFeature]]:
        """return a dictionary of all supported features

        this returns a dictionary with all supported Features of the spectrometer

        Returns
        -------
        features : `dict` [`str`, `seabreeze.cseabreeze.SeaBreezeFeature`]
        """
        if not self._cached_features:
            protocol = self._transport.protocol
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
                    f_list.append(feature_cls(self._transport.protocol, len(f_list)))
        return self._cached_features

    @property
    def f(self) -> SeaBreezeFeatureAccessor:
        """convenience access to features via attributes

        this allows you to access a feature like this::

            # via .features
            device.features['spectrometer'][0].get_intensities()
            # via .f
            device.f.spectrometer.get_intensities()

        """

        class FeatureAccessHandler:
            def __init__(self, feature_dict: dict[str, Any]) -> None:
                for identifier, features in feature_dict.items():
                    setattr(
                        self, identifier, features[0] if features else None
                    )  # TODO: raise FeatureNotAvailable?

        accessor: SeaBreezeFeatureAccessor = FeatureAccessHandler(self.features)  # type: ignore
        return accessor


# SPECTROMETER DEFINITIONS
# ========================
#
class USB2000PLUS(SeaBreezeDevice):
    model_name = "USB2000PLUS"

    # communication config
    transport = (USBTransport,)
    usb_vendor_id = 0x2457
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
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE", "SINGLE_SHOT"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureUSB2000PLUS,
        sbf.nonlinearity.NonlinearityCoefficientsEEPromFeatureOOI,
        sbf.continuousstrobe.SeaBreezeContinuousStrobeFeatureOOI,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )

    @classmethod
    def _substitute_compatible_subclass(
        cls: type[DT], transport: PySeaBreezeTransport[Any]
    ) -> type[DT]:
        """return the correct subclass of the usb2000plus like model"""
        try:
            protocol = transport.protocol
        except RuntimeError:
            raise AttributeError("transport not opened")

        # noinspection PyUnresolvedReferences,PyProtectedMember
        from seabreeze.pyseabreeze.features.fpga import _FPGARegisterFeatureOOI

        fpga = _FPGARegisterFeatureOOI(protocol)
        if fpga.get_firmware_version()[0] >= 3:
            return FLAMES  # type: ignore
        else:
            return USB2000PLUS  # type: ignore


class FLAMES(USB2000PLUS):
    model_name = "FLAMES"

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((6, 21))  # as in seabreeze-3.0.9
    integration_time_min = 1000
    integration_time_max = 655350000
    integration_time_base = 1
    spectrum_num_pixel = 2048
    spectrum_raw_length = (2048 * 2) + 1
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE", "SINGLE_SHOT"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureUSB2000PLUS,
        sbf.nonlinearity.NonlinearityCoefficientsEEPromFeatureOOI,
        sbf.continuousstrobe.SeaBreezeContinuousStrobeFeatureOOI,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class USB2000(SeaBreezeDevice):
    model_name = "USB2000"

    # communication config
    transport = (USBTransport,)
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    integration_time_max = 65535000
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    trigger_modes = TriggerMode.supported(
        "NORMAL",
    )

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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
    usb_product_id = 0x4004
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81
    )  # XXX: we'll ignore the alternative EPs
    usb_protocol = OBPProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((0, 4), (1040, 1044))
    integration_time_min = 8000
    integration_time_max = 1600000000
    integration_time_base = 1
    spectrum_num_pixel = 1044
    spectrum_raw_length = (1044 * 4) + 32  # XXX: Metadata
    spectrum_max_value = (2**18) - 1
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
    usb_vendor_id = 0x2457
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
    usb_vendor_id = 0x2457
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


class HDX(SeaBreezeDevice):
    model_name = "HDX"

    # communication config
    transport = (USBTransport,)
    usb_vendor_id = 0x2457
    usb_product_id = 0x2003
    usb_endpoint_map = EndPointMap(
        ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86
    )
    usb_protocol = OBPProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 6000
    integration_time_max = 10000000
    integration_time_base = 1
    spectrum_num_pixel = 2068
    spectrum_raw_length = (2068 * 2) + 64  # XXX: Metadata
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "OBP_NORMAL", "OBP_LEVEL", "OBP_EDGE", "DISABLED"
    )

    # features
    feature_classes = (
        sbf.spectrometer.SeaBreezeSpectrometerFeatureHDX,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
        sbf.nonlinearity.NonlinearityCoefficientsFeatureOBP,
    )


class ADC1000USB(SeaBreezeDevice):
    model_name = "ADC1000-USB"

    # communication config
    transport = (USBTransport,)
    usb_vendor_id = 0x2457
    usb_product_id = 0x1004
    usb_endpoint_map = EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)
    usb_protocol = ADCProtocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges((2, 24))
    integration_time_min = 5000
    integration_time_max = 65535000
    integration_time_base = 1000
    spectrum_num_pixel = 2048
    spectrum_raw_length = 2048 * 2 + 1  # Sync byte!
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported(
        "NORMAL", "SOFTWARE", "SYNCHRONIZATION", "HARDWARE"
    )

    # features
    feature_classes = (
        sbf.eeprom.SeaBreezeEEPromFeatureADC,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureADC,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
    )


class SR4(SeaBreezeDevice):
    model_name = "SR4"

    # communication config
    transport = (USBTransport,)
    usb_vendor_id = 0x0999
    usb_product_id = 0x1002
    usb_endpoint_map = EndPointMap(ep_out=0x01, highspeed_in=0x81)
    usb_protocol = OBP2Protocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 6000  # ???
    integration_time_max = 10000000  # ???
    integration_time_base = 1
    spectrum_num_pixel = 3648
    spectrum_raw_length = (3648 * 2) + 32  # XXX: Metadata
    spectrum_max_value = 65535
    trigger_modes = TriggerMode.supported("OBP_NORMAL")

    # features
    feature_classes = (sbf.spectrometer.SeaBreezeSpectrometerFeatureSR4,)


class ST(SeaBreezeDevice):
    model_name = "ST"

    # communication config
    transport = (USBTransport,)
    usb_vendor_id = 0x0999
    usb_product_id = 0x1000
    usb_endpoint_map = EndPointMap(ep_out=0x01, highspeed_in=0x81, highspeed_in2=0x82)
    usb_protocol = OBP2Protocol

    # spectrometer config
    dark_pixel_indices = DarkPixelIndices.from_ranges()
    integration_time_min = 1560
    integration_time_max = 6000000
    integration_time_base = 10
    spectrum_num_pixel = 1516
    spectrum_raw_length = 1516 * 2  # ??
    spectrum_max_value = 16383
    # Triggering (from Ocean ST Manual v10-5): Software, External Rising Edge
    trigger_modes = TriggerMode.supported("NORMAL", "SOFTWARE", "EDGE")

    # features
    feature_classes = (
        # sbf.eeprom.SeaBreezeEEPromFeatureOOI,
        sbf.spectrometer.SeaBreezeSpectrometerFeatureST,
        sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
        sbf.nonlinearity.NonlinearityCoefficientsFeatureOBP2,
    )
