import struct

import numpy

from seabreeze.pyseabreeze.exceptions import SeaBreezeError, SeaBreezeNotSupported
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
from seabreeze.pyseabreeze.features.eeprom import SeaBreezeEEPromFeatureOOI
from seabreeze.pyseabreeze.protocol import OBPProtocol, OOIProtocol


# Definition
# ==========
#
class SeaBreezeSpectrometerFeature(SeaBreezeFeature):
    identifier = "spectrometer"

    def set_trigger_mode(self, mode):
        raise NotImplementedError("implement in derived class")

    def set_integration_time_micros(self, integration_time_micros):
        raise NotImplementedError("implement in derived class")

    def get_integration_time_micros_limits(self):
        raise NotImplementedError("implement in derived class")

    def get_maximum_intensity(self):
        raise NotImplementedError("implement in derived class")

    def get_electric_dark_pixel_indices(self):
        raise NotImplementedError("implement in derived class")

    def _spectrum_length(self):
        raise NotImplementedError("implement in derived class")

    def get_wavelengths(self):
        raise NotImplementedError("implement in derived class")

    def get_intensities(self):
        raise NotImplementedError("implement in derived class")

    def _get_spectrum_raw(self):
        raise NotImplementedError("implement in derived class")

    def get_fast_buffer_spectrum(self):
        raise SeaBreezeNotSupported(
            "needs to be provided in the specific implementation if supported"
        )


# Spectrometer Features based on USBCommOOI
# =========================================
#
class SeaBreezeSpectrometerFeatureOOI(SeaBreezeSpectrometerFeature):
    _required_protocol_cls = OOIProtocol
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

    # config
    _dark_pixel_indices = None
    _integration_time_min = None
    _integration_time_max = None
    _integration_time_base = None
    _spectrum_num_pixel = None
    _spectrum_raw_length = None
    _spectrum_max_value = None
    _trigger_modes = None

    def __init__(self, device, feature_id, **kwargs):
        super(SeaBreezeSpectrometerFeatureOOI, self).__init__(
            device, feature_id, **kwargs
        )
        self._dark_pixel_indices = kwargs["dark_pixel_indices"]
        self._integration_time_min = kwargs["integration_time_min"]
        self._integration_time_max = kwargs["integration_time_max"]
        self._integration_time_base = kwargs["integration_time_base"]
        self._spectrum_num_pixel = kwargs["spectrum_num_pixel"]
        self._spectrum_raw_length = kwargs["spectrum_raw_length"]
        self._spectrum_max_value = kwargs["spectrum_max_value"]
        self._trigger_modes = kwargs["trigger_modes"]

    def set_trigger_mode(self, mode):
        if mode in self._trigger_modes:
            self.protocol.send(0x0A, mode)
        else:
            raise SeaBreezeError("Only supports: %s" % str(self._trigger_modes))

    def set_integration_time_micros(self, integration_time_micros):
        t_min = self._integration_time_min
        t_max = self._integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self._integration_time_base)
            self.protocol.send(0x02, i_time)
        else:
            raise SeaBreezeError("Integration not in [{:d}, {:d}]".format(t_min, t_max))

    def get_integration_time_micros_limits(self):
        return self._integration_time_min, self._integration_time_max

    def get_maximum_intensity(self):
        return float(self._spectrum_max_value)

    def get_electric_dark_pixel_indices(self):
        return numpy.array(self._dark_pixel_indices)

    @property
    def _spectrum_length(self):
        return self._spectrum_num_pixel

    def get_wavelengths(self):
        indices = numpy.arange(self._spectrum_length, dtype=numpy.float64)
        # OOI spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            # noinspection PyProtectedMember
            coeffs.append(
                float(
                    SeaBreezeEEPromFeatureOOI._func_eeprom_read_slot(self.protocol, i)
                )
            )
        return sum(wl * (indices ** i) for i, wl in enumerate(coeffs))

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        ret = numpy.array(
            struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]),
            dtype=numpy.double,
        )
        return ret * self._normalization_value

    def _get_spectrum_raw(self):
        tmp = numpy.empty((self._spectrum_raw_length,), dtype=numpy.uint8)
        self.protocol.send(0x09)

        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        tmp[:] = self.protocol.receive(
            size=self._spectrum_raw_length, timeout_ms=timeout, mode="high_speed"
        )
        return tmp

    def get_fast_buffer_spectrum(self):
        raise SeaBreezeNotSupported(
            "needs to be provided in the specific implementation if supported"
        )


class SeaBreezeSpectrometerFeatureOOI2K(SeaBreezeSpectrometerFeatureOOI):
    def get_intensities(self):
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
    def __init__(self, device, feature_id, **kwargs):
        super(SeaBreezeSpectrometerFeatureOOIFPGA, self).__init__(
            device, feature_id, **kwargs
        )
        self.protocol.send(0xFE)
        ret = self.protocol.receive(size=16)
        data = struct.unpack("<HLBBBBBBBBBB", ret[:])
        # FIXME: check data[10] return values
        self.protocol.transport._default_read_endpoint = (
            "low_speed" if data[10] else "high_speed"
        )


class SeaBreezeSpectrometerFeatureOOIFPGA4K(SeaBreezeSpectrometerFeatureOOIFPGA):
    def _get_spectrum_raw(self):
        tmp = numpy.empty((self._spectrum_raw_length,), dtype=numpy.uint8)
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        self.protocol.send(0x09)
        # noinspection PyProtectedMember
        if self.protocol.transport._default_read_endpoint == "low_speed":
            tmp[:] = self.protocol.receive(
                size=self._spectrum_raw_length, timeout_ms=timeout
            )
        else:  # high_speed
            tmp[:2048] = self.protocol.receive(
                size=2048, timeout_ms=timeout, mode="high_speed_alt"
            )
            tmp[2048:] = self.protocol.receive(
                size=self._spectrum_raw_length - 2048,
                timeout_ms=timeout,
                mode="high_speed",
            )
        return tmp


class _SeaBreezeSpectrometerSaturationMixin(object):
    def _saturation_unpack(self, ret):
        return struct.unpack("<H", ret[6:8])[0]

    def _saturation_not_initialized(self, x):
        return x == 0

    # noinspection PyUnresolvedReferences
    def _saturation_get_normalization_value(self):
        """internal only"""
        # noinspection PyProtectedMember
        ret = SeaBreezeEEPromFeatureOOI._func_eeprom_read_slot(
            self.protocol, 17, raw=True
        )
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
    def __init__(self, device, feature_id, **kwargs):
        # set the usbspeed
        super(SeaBreezeSpectrometerFeatureOOIGain, self).__init__(
            device, feature_id, **kwargs
        )
        # load the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIFPGAGain(
    SeaBreezeSpectrometerFeatureOOIFPGA, _SeaBreezeSpectrometerSaturationMixin
):
    def __init__(self, device, feature_id, **kwargs):
        # set the usbspeed
        super(SeaBreezeSpectrometerFeatureOOIFPGAGain, self).__init__(
            device, feature_id, **kwargs
        )
        # load the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIFPGA4KGain(
    SeaBreezeSpectrometerFeatureOOIFPGA4K, _SeaBreezeSpectrometerSaturationMixin
):
    def __init__(self, device, feature_id, **kwargs):
        # set the usbspeed
        super(SeaBreezeSpectrometerFeatureOOIFPGA4KGain, self).__init__(
            device, feature_id, **kwargs
        )
        # get the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIGainAlt(SeaBreezeSpectrometerFeatureOOIGain):
    # XXX: The NIRQUEST stores this value somewhere else
    #      and might also not have been programmed yet..
    # TODO: And this is planned for the QE65000 apparently.
    def _saturation_unpack(self, ret):
        return struct.unpack("<L", ret[6:10])[0]


class SeaBreezeSpectrometerFeatureOOIFPGAGainAlt(
    SeaBreezeSpectrometerFeatureOOIFPGAGain
):
    # XXX: The Apex, Maya2000pro, MayaLSL store this value somewhere else
    #      and might also not have been programmed yet...
    # ret contains the first two response bytes, then the eeprom data
    def _saturation_unpack(self, ret):
        return struct.unpack("<H", ret[2:4])[0]

    def _saturation_not_initialized(self, x):
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
    _dark_pixel_indices = None
    _integration_time_min = None
    _integration_time_max = None
    _integration_time_base = None
    _spectrum_num_pixel = None
    _spectrum_raw_length = None
    _spectrum_max_value = None
    _trigger_modes = None

    def __init__(self, device, feature_id, **kwargs):
        super(SeaBreezeSpectrometerFeatureOBP, self).__init__(
            device, feature_id, **kwargs
        )
        self._dark_pixel_indices = kwargs["dark_pixel_indices"]
        self._integration_time_min = kwargs["integration_time_min"]
        self._integration_time_max = kwargs["integration_time_max"]
        self._integration_time_base = kwargs["integration_time_base"]
        self._spectrum_num_pixel = kwargs["spectrum_num_pixel"]
        self._spectrum_raw_length = kwargs["spectrum_raw_length"]
        self._spectrum_max_value = kwargs["spectrum_max_value"]
        self._trigger_modes = kwargs["trigger_modes"]

    def set_trigger_mode(self, mode):
        if mode in self._trigger_modes:
            self.protocol.send(0x00110110, mode, request_ack=False)
        else:
            raise SeaBreezeError("Only supports: %s" % str(self._trigger_modes))

    def set_integration_time_micros(self, integration_time_micros):
        t_min = self._integration_time_min
        t_max = self._integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self._integration_time_base)
            self.protocol.send(0x00110010, i_time)
        else:
            raise SeaBreezeError("Integration not in [{:d}, {:d}]".format(t_min, t_max))

    def get_integration_time_micros_limits(self):
        return self._integration_time_min, self._integration_time_max

    def get_maximum_intensity(self):
        return float(self._spectrum_max_value)

    def get_electric_dark_pixel_indices(self):
        return numpy.array(self._dark_pixel_indices)

    @property
    def _spectrum_length(self):
        return self._spectrum_num_pixel

    def get_wavelengths(self):
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
        return sum(wl * (indices ** i) for i, wl in enumerate(coeffs))

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # noinspection PyTypeChecker
        return numpy.array(
            struct.unpack("<" + "H" * self._spectrum_length, tmp), dtype=numpy.double
        )

    def _get_spectrum_raw(self):
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        datastring = self.protocol.query(0x00101100, timeout_ms=timeout)
        return numpy.fromstring(datastring, dtype=numpy.uint8)

    def get_fast_buffer_spectrum(self):
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
    def get_intensities(self):
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
    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # The HR2000PLUS needs to xor with 0x2000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x2000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureQE65000(SeaBreezeSpectrometerFeatureOOIFPGA):
    def get_wavelengths(self):
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
        return sum(wl * (indices ** i) for i, wl in enumerate(coeffs))

    def get_intensities(self):
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
    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # The NIRQUEST512 needs to xor with 0x8000
        ret = (
            numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]))
            ^ 0x8000
        )
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureNIRQUEST256(SeaBreezeSpectrometerFeatureOOIGainAlt):
    def get_intensities(self):
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
    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # XXX: No sync byte for the Jaz
        ret = numpy.array(
            struct.unpack("<" + "H" * self._spectrum_length, tmp[:]), dtype=numpy.double
        )
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureSTS(SeaBreezeSpectrometerFeatureOBP):
    pass


class SeaBreezeSpectrometerFeatureQEPRO(SeaBreezeSpectrometerFeatureOBP):
    def _get_spectrum_raw(self):
        timeout = int(
            self._integration_time_max * 1e-3
            + self.protocol.transport.default_timeout_ms
        )
        datastring = self.protocol.query(0x00100928, timeout_ms=timeout)
        return numpy.fromstring(datastring, dtype=numpy.uint8)

    def get_intensities(self):
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
