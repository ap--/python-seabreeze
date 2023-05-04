from __future__ import annotations

import struct
import warnings
from typing import TYPE_CHECKING
from typing import Any

import numpy

from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.exceptions import SeaBreezeNotSupported
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.features.eeprom import SeaBreezeEEPROMFeature
from seabreeze.pyseabreeze.features.eeprom import SeaBreezeEEPromFeatureADC
from seabreeze.pyseabreeze.features.eeprom import SeaBreezeEEPromFeatureOOI
from seabreeze.pyseabreeze.protocol import ADCProtocol
from seabreeze.pyseabreeze.protocol import OBPProtocol
from seabreeze.pyseabreeze.protocol import OOIProtocol
from seabreeze.pyseabreeze.transport import USBTransport
from seabreeze.pyseabreeze.types import PySeaBreezeProtocol

if TYPE_CHECKING:
    import numpy as np
    from numpy.typing import NDArray


# Definition
# ==========
#
class SeaBreezeSpectrometerFeature(SeaBreezeFeature):
    identifier = "spectrometer"

    def set_trigger_mode(self, mode: int) -> None:
        raise NotImplementedError("implement in derived class")

    def set_integration_time_micros(self, integration_time_micros: int) -> None:
        raise NotImplementedError("implement in derived class")

    def get_integration_time_micros_limits(self) -> tuple[int, int]:
        raise NotImplementedError("implement in derived class")

    def get_maximum_intensity(self) -> float:
        raise NotImplementedError("implement in derived class")

    def get_electric_dark_pixel_indices(self) -> list[int]:
        raise NotImplementedError("implement in derived class")

    @property
    def _spectrum_length(self) -> int:
        raise NotImplementedError("implement in derived class")

    def get_wavelengths(self) -> NDArray[np.float_]:
        raise NotImplementedError("implement in derived class")

    def get_intensities(self) -> NDArray[np.float_]:
        raise NotImplementedError("implement in derived class")

    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        raise NotImplementedError("implement in derived class")

    def get_fast_buffer_spectrum(self) -> Any:
        raise SeaBreezeNotSupported(
            "needs to be provided in the specific implementation if supported"
        )


# Spectrometer Features based on USBCommOOI
# =========================================
#
class SeaBreezeSpectrometerFeatureOOI(SeaBreezeSpectrometerFeature):
    _required_protocol_cls: type[PySeaBreezeProtocol] = OOIProtocol
    _required_features = ("eeprom",)
    _required_kwargs = (
        "dark_pixel_indices",
        "integration_time_min",
        "integration_time_max",
        "integration_time_base",
        "spectrum_num_pixel",
        "spectrum_raw_length",
        "spectrum_max_value",
        "trigger_modes",
    )

    _normalization_value = 1.0
    _eeprom_cls: type[SeaBreezeEEPROMFeature] = SeaBreezeEEPromFeatureOOI

    # config
    _dark_pixel_indice: tuple[int, ...]
    _integration_time_min: int
    _integration_time_max: int
    _integration_time_base: int
    _spectrum_num_pixel: int
    _spectrum_raw_length: int
    _spectrum_max_value: int
    _trigger_modes: set[int]

    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        super().__init__(protocol, feature_id, **kwargs)
        self._dark_pixel_indices = kwargs["dark_pixel_indices"]
        self._integration_time_min = kwargs["integration_time_min"]
        self._integration_time_max = kwargs["integration_time_max"]
        self._integration_time_base = kwargs["integration_time_base"]
        self._spectrum_num_pixel = kwargs["spectrum_num_pixel"]
        self._spectrum_raw_length = kwargs["spectrum_raw_length"]
        self._spectrum_max_value = kwargs["spectrum_max_value"]
        self._trigger_modes = kwargs["trigger_modes"]

    def set_trigger_mode(self, mode: int) -> None:
        if mode in self._trigger_modes:
            self.protocol.send(0x0A, mode)
        else:
            raise SeaBreezeError("Only supports: %s" % str(self._trigger_modes))

    def set_integration_time_micros(self, integration_time_micros: int) -> None:
        t_min = self._integration_time_min
        t_max = self._integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self._integration_time_base)
            self.protocol.send(0x02, i_time)
        else:
            raise SeaBreezeError(f"Integration not in [{t_min:d}, {t_max:d}]")

    def get_integration_time_micros_limits(self) -> tuple[int, int]:
        return self._integration_time_min, self._integration_time_max

    def get_maximum_intensity(self) -> float:
        return float(self._spectrum_max_value)

    def get_electric_dark_pixel_indices(self) -> list[int]:
        return list(self._dark_pixel_indices)

    @property
    def _spectrum_length(self) -> int:
        return self._spectrum_num_pixel

    def get_wavelengths(self) -> NDArray[np.float_]:
        indices = numpy.arange(self._spectrum_length, dtype=numpy.float64)
        # OOI spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            # noinspection PyProtectedMember
            coeffs.append(
                float(self._eeprom_cls._func_eeprom_read_slot(self.protocol, i))
            )
        return sum(wl * (indices**i) for i, wl in enumerate(coeffs))  # type: ignore

    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        ret = numpy.array(
            struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]),
            dtype=numpy.double,
        )
        return ret * self._normalization_value

    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        tmp = numpy.empty((self._spectrum_raw_length,), dtype=numpy.uint8)
        self.protocol.send(0x09)

        assert isinstance(
            self.protocol.transport, USBTransport
        ), "current impl requires USBTransport"

        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        # noinspection PyProtectedMember
        tmp[:] = bytearray(
            self.protocol.receive(
                size=self._spectrum_raw_length,
                timeout_ms=timeout,
                mode=self.protocol.transport._default_read_spectrum_endpoint,
            )
        )
        return tmp

    def get_fast_buffer_spectrum(self) -> Any:
        raise SeaBreezeNotSupported(
            "needs to be provided in the specific implementation if supported"
        )


class SeaBreezeSpectrometerFeatureOOI2K(SeaBreezeSpectrometerFeatureOOI):
    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The byte order is different for some models
        N_raw = self._spectrum_raw_length - 1
        N_pix = self._spectrum_length
        idx = [(i // 2) % 64 + (i % 2) * 64 + (i // 128) * 128 for i in range(N_raw)]
        # high nibble not guaranteed to be pulled low
        tsorted = tmp[idx] & numpy.array((0xFF, 0x0F) * N_pix, dtype=numpy.uint8)
        ret = numpy.array(struct.unpack("<" + "H" * N_pix, tsorted), dtype=numpy.double)
        # sorted and parsed
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureOOIFPGA(SeaBreezeSpectrometerFeatureOOI):
    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        super().__init__(protocol, feature_id, **kwargs)
        self.protocol.send(0xFE)
        ret = self.protocol.receive(size=16)
        data = struct.unpack("<HLBBBBBBBBBB", ret[:])
        speed = data[10]

        assert isinstance(
            self.protocol.transport, USBTransport
        ), "current impl requires USBTransport"
        if speed == 0x00:
            # USB1.0 port
            self.protocol.transport._default_read_spectrum_endpoint = "low_speed"
            warnings.warn(
                "The device is connected to a USB1.0 port. "
                "This might not work. Please report this issue upstream."
            )
        elif speed == 0x80:
            # USB2.0+ port
            self.protocol.transport._default_read_spectrum_endpoint = "high_speed"
        else:
            # raise if the value is not what's expected from the datasheets
            raise ValueError(
                f"Unknown speed setting '{speed}' please file this as a bug!"
            )


class SeaBreezeSpectrometerFeatureOOIFPGA4K(SeaBreezeSpectrometerFeatureOOIFPGA):
    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        tmp = numpy.empty((self._spectrum_raw_length,), dtype=numpy.uint8)
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        self.protocol.send(0x09)
        assert isinstance(
            self.protocol.transport, USBTransport
        ), "current impl requires USBTransport"
        # noinspection PyProtectedMember
        if self.protocol.transport._default_read_spectrum_endpoint == "low_speed":
            tmp[:] = bytearray(
                self.protocol.receive(
                    size=self._spectrum_raw_length, timeout_ms=timeout
                )
            )
        else:  # high_speed
            tmp[:2048] = bytearray(
                self.protocol.receive(
                    size=2048, timeout_ms=timeout, mode="high_speed_alt"
                )
            )
            tmp[2048:] = bytearray(
                self.protocol.receive(
                    size=self._spectrum_raw_length - 2048,
                    timeout_ms=timeout,
                    mode="high_speed",
                )
            )
        return tmp


class _SeaBreezeSpectrometerSaturationMixin:
    _normalization_value: float
    _spectrum_max_value: int
    protocol: PySeaBreezeProtocol

    def _saturation_unpack(self, ret: bytes) -> int:
        return int(struct.unpack("<H", ret[6:8])[0])

    def _saturation_not_initialized(self, x: int) -> bool:
        return x == 0

    # noinspection PyUnresolvedReferences
    def _saturation_get_normalization_value(self) -> float:
        """internal only"""
        # noinspection PyProtectedMember
        ret = SeaBreezeEEPromFeatureOOI._func_eeprom_read_raw(self.protocol, 17)
        # ret contains the first two response bytes, then the eeprom data
        saturation = self._saturation_unpack(ret)
        if self._saturation_not_initialized(saturation):
            # pass  # not initialized?
            return self._normalization_value
        else:
            return float(self._spectrum_max_value) / saturation


class SeaBreezeSpectrometerFeatureOOIGain(
    SeaBreezeSpectrometerFeatureOOI, _SeaBreezeSpectrometerSaturationMixin
):
    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        # set the usbspeed
        super().__init__(protocol, feature_id, **kwargs)
        # load the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIFPGAGain(
    SeaBreezeSpectrometerFeatureOOIFPGA, _SeaBreezeSpectrometerSaturationMixin
):
    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        # set the usbspeed
        super().__init__(protocol, feature_id, **kwargs)
        # load the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIFPGA4KGain(
    SeaBreezeSpectrometerFeatureOOIFPGA4K, _SeaBreezeSpectrometerSaturationMixin
):
    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        # set the usbspeed
        super().__init__(protocol, feature_id, **kwargs)
        # get the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIGainAlt(SeaBreezeSpectrometerFeatureOOIGain):
    # XXX: The NIRQUEST stores this value somewhere else
    #      and might also not have been programmed yet..
    # TODO: And this is planned for the QE65000 apparently.
    def _saturation_unpack(self, ret: bytes) -> int:
        return int(struct.unpack("<L", ret[6:10])[0])


class SeaBreezeSpectrometerFeatureOOIFPGAGainAlt(
    SeaBreezeSpectrometerFeatureOOIFPGAGain
):
    # XXX: The Apex, Maya2000pro, MayaLSL store this value somewhere else
    #      and might also not have been programmed yet...
    # ret contains the first two response bytes, then the eeprom data
    def _saturation_unpack(self, ret: bytes) -> int:
        return int(struct.unpack("<H", ret[2:4])[0])

    def _saturation_not_initialized(self, x: int) -> bool:
        return x <= 32768 or x > self._spectrum_max_value


# Spectrometer Features based on USBCommOBP
# =========================================
#
class SeaBreezeSpectrometerFeatureOBP(SeaBreezeSpectrometerFeature):
    _required_protocol_cls = OBPProtocol
    # required_interface_cls = USBCommOBP
    required_features = ()
    _required_kwargs = (
        "dark_pixel_indices",
        "integration_time_min",
        "integration_time_max",
        "integration_time_base",
        "spectrum_num_pixel",
        "spectrum_raw_length",
        "spectrum_max_value",
        "trigger_modes",
    )

    _normalization_value = 1.0

    # config
    _dark_pixel_indice: tuple[int, ...]
    _integration_time_min: int
    _integration_time_max: int
    _integration_time_base: int
    _spectrum_num_pixel: int
    _spectrum_raw_length: int
    _spectrum_max_value: int
    _trigger_modes: set[int]

    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        super().__init__(protocol, feature_id, **kwargs)
        self._dark_pixel_indices = kwargs["dark_pixel_indices"]
        self._integration_time_min = kwargs["integration_time_min"]
        self._integration_time_max = kwargs["integration_time_max"]
        self._integration_time_base = kwargs["integration_time_base"]
        self._spectrum_num_pixel = kwargs["spectrum_num_pixel"]
        self._spectrum_raw_length = kwargs["spectrum_raw_length"]
        self._spectrum_max_value = kwargs["spectrum_max_value"]
        self._trigger_modes = kwargs["trigger_modes"]

    def set_trigger_mode(self, mode: int) -> None:
        if mode in self._trigger_modes:
            self.protocol.send(0x00110110, mode, request_ack=True)
        else:
            raise SeaBreezeError("Only supports: %s" % str(self._trigger_modes))

    def set_integration_time_micros(self, integration_time_micros: int) -> None:
        t_min = self._integration_time_min
        t_max = self._integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self._integration_time_base)
            self.protocol.send(0x00110010, i_time)
        else:
            raise SeaBreezeError(f"Integration not in [{t_min:d}, {t_max:d}]")

    def get_integration_time_micros_limits(self) -> tuple[int, int]:
        return self._integration_time_min, self._integration_time_max

    def get_maximum_intensity(self) -> float:
        return float(self._spectrum_max_value)

    def get_electric_dark_pixel_indices(self) -> list[int]:
        return list(self._dark_pixel_indices)

    @property
    def _spectrum_length(self) -> int:
        return self._spectrum_num_pixel

    def get_wavelengths(self) -> NDArray[np.float_]:
        # get number of wavelength coefficients
        data = self.protocol.query(0x00180100)
        N = struct.unpack("<B", data)[0]
        # now query the coefficients
        coeffs = []
        for i in range(N):
            data = self.protocol.query(0x00180101, i)
            coeffs.append(struct.unpack("<f", data)[0])
        # and generate the wavelength array
        indices = numpy.arange(self._spectrum_length, dtype=numpy.float64)
        return sum(wl * (indices**i) for i, wl in enumerate(coeffs))  # type: ignore

    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        arr = struct.unpack("<" + "H" * self._spectrum_length, tmp)  # type: ignore
        return numpy.array(arr, dtype=numpy.float_)

    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        datastring = self.protocol.query(0x00101100, timeout_ms=timeout)
        return numpy.frombuffer(datastring, dtype=numpy.uint8)  # type: ignore

    def get_fast_buffer_spectrum(self) -> Any:
        raise SeaBreezeNotSupported(
            "needs to be provided in the specific implementation if supported"
        )


# Model specific changes
# ======================
#
class SeaBreezeSpectrometerFeatureUSB2000(SeaBreezeSpectrometerFeatureOOI2K):
    pass


class SeaBreezeSpectrometerFeatureHR2000(SeaBreezeSpectrometerFeatureOOI2K):
    pass


class SeaBreezeSpectrometerFeatureHR4000(SeaBreezeSpectrometerFeatureOOIFPGA4K):
    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The HR4000 needs to xor with 0x2000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x2000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureUSB650(SeaBreezeSpectrometerFeatureOOI2K):
    pass


class SeaBreezeSpectrometerFeatureHR2000PLUS(SeaBreezeSpectrometerFeatureOOIFPGA):
    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The HR2000PLUS needs to xor with 0x2000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x2000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureQE65000(SeaBreezeSpectrometerFeatureOOIFPGA):
    def get_wavelengths(self) -> NDArray[np.float_]:
        # QE65000 specific override
        indices = numpy.arange(-10, self._spectrum_length - 10, dtype=numpy.float64)
        # OOI spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            # noinspection PyProtectedMember
            coeffs.append(
                float(
                    SeaBreezeEEPromFeatureOOI._func_eeprom_read_slot(self.protocol, i)
                )
            )
        return sum(wl * (indices**i) for i, wl in enumerate(coeffs))  # type: ignore

    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The QE65000 needs to xor with 0x8000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x8000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureUSB2000PLUS(SeaBreezeSpectrometerFeatureOOIFPGAGain):
    pass


class SeaBreezeSpectrometerFeatureUSB4000(SeaBreezeSpectrometerFeatureOOIFPGA4KGain):
    pass


class SeaBreezeSpectrometerFeatureNIRQUEST512(SeaBreezeSpectrometerFeatureOOIGainAlt):
    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The NIRQUEST512 needs to xor with 0x8000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x8000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureNIRQUEST256(SeaBreezeSpectrometerFeatureOOIGainAlt):
    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The NIRQUEST256 needs to xor with 0x8000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x8000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureMAYA2000PRO(
    SeaBreezeSpectrometerFeatureOOIFPGAGainAlt
):
    pass


class SeaBreezeSpectrometerFeatureMAYA2000(SeaBreezeSpectrometerFeatureOOIFPGA):
    pass


class SeaBreezeSpectrometerFeatureTORUS(SeaBreezeSpectrometerFeatureOOIFPGAGain):
    # The Torus uses the USB2000Plus spec feature
    pass


class SeaBreezeSpectrometerFeatureAPEX(SeaBreezeSpectrometerFeatureOOIFPGAGainAlt):
    pass


class SeaBreezeSpectrometerFeatureMAYALSL(SeaBreezeSpectrometerFeatureOOIFPGAGainAlt):
    pass


class SeaBreezeSpectrometerFeatureJAZ(SeaBreezeSpectrometerFeatureOOIGain):
    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # XXX: No sync byte for the Jaz
        ret = numpy.array(
            struct.unpack("<" + "H" * self._spectrum_length, tmp[:]), dtype=numpy.double
        )
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureSTS(SeaBreezeSpectrometerFeatureOBP):
    pass


class SeaBreezeSpectrometerFeatureQEPRO(SeaBreezeSpectrometerFeatureOBP):
    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        datastring = self.protocol.query(0x00100928, timeout_ms=timeout)
        return numpy.frombuffer(datastring, dtype=numpy.uint8)  # type: ignore

    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # 32byte metadata block at beginning
        ret = numpy.array(
            struct.unpack("<" + "I" * self._spectrum_length, tmp[32:]),
            dtype=numpy.double,
        )
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureVENTANA(SeaBreezeSpectrometerFeatureOBP):
    pass


class SeaBreezeSpectrometerFeatureSPARK(SeaBreezeSpectrometerFeatureOBP):
    pass


class SeaBreezeSpectrometerFeatureHDX(SeaBreezeSpectrometerFeatureOBP):
    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        # the message type is different than the default defined in the protocol,
        # requires addition of a new message type in protocol to work
        datastring = self.protocol.query(0x00101000, timeout_ms=timeout)
        return numpy.frombuffer(datastring, dtype=numpy.uint8)  # type: ignore


class SeaBreezeSpectrometerFeatureADC(SeaBreezeSpectrometerFeatureOOI):
    _required_protocol_cls = ADCProtocol
    _eeprom_cls = SeaBreezeEEPromFeatureADC

    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # The byte order is different for some models
        N_raw = self._spectrum_raw_length - 1
        N_pix = self._spectrum_length
        idx = [(i // 2) % 64 + (i % 2) * 64 + (i // 128) * 128 for i in range(N_raw)]
        tsorted = tmp[idx]
        ret = numpy.array(struct.unpack("<" + "H" * N_pix, tsorted), dtype=numpy.double)
        # sorted and parsed
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureSR4(SeaBreezeSpectrometerFeatureOBP):
    def _get_spectrum_raw(self) -> NDArray[np.uint8]:
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        datastring = self.protocol.query(0x000_01C_00, timeout_ms=timeout)
        return numpy.frombuffer(datastring, dtype=numpy.uint8)  # type: ignore

    def get_intensities(self) -> NDArray[np.float_]:
        tmp = self._get_spectrum_raw()
        # 32byte metadata block at beginning
        ret = numpy.array(
            struct.unpack("<" + "H" * self._spectrum_length, tmp[32:]),
            dtype=numpy.double,
        )
        return ret * self._normalization_value

    def set_trigger_mode(self, mode: int) -> None:
        if mode in self._trigger_modes:
            self.protocol.send(0x000_00D_01, mode, request_ack=True)
        else:
            raise SeaBreezeError("Only supports: %s" % str(self._trigger_modes))

    def set_integration_time_micros(self, integration_time_micros: int) -> None:
        t_min = self._integration_time_min
        t_max = self._integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self._integration_time_base)
            self.protocol.send(0x000_00C_01, i_time)
        else:
            raise SeaBreezeError(f"Integration not in [{t_min:d}, {t_max:d}]")

    def get_wavelengths(self) -> NDArray[np.float_]:
        data = self.protocol.query(0x000_011_00)
        num_coeffs = len(data) // 4
        assert len(data) % 4 == 0  # ???
        assert num_coeffs > 1  # ???
        coeffs = struct.unpack("<" + "f" * num_coeffs, data)[1:]
        # and generate the wavelength array
        indices = numpy.arange(self._spectrum_length, dtype=numpy.float64)
        return sum(wl * (indices**i) for i, wl in enumerate(coeffs))  # type: ignore


class SeaBreezeSpectrometerFeatureST(SeaBreezeSpectrometerFeatureSR4):
    pass
