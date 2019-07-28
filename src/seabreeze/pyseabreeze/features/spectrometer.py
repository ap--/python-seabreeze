import struct

import numpy

from seabreeze.pyseabreeze.communication import USBCommOOI
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
class SeaBreezeSpectrometerFeature(SeaBreezeFeature):
    identifier = 'spectrometer'

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

    def _get_fast_buffer_spectrum(self):
        raise NotImplementedError("implement in derived class")


# Spectrometer Features based on USBCommOOI
# =========================================
#
class SeaBreezeSpectrometerFeatureOOI(SeaBreezeSpectrometerFeature):
    required_interface_cls = USBCommOOI
    required_features = ('eeprom',)

    _normalization_value = 1.0

    def __init__(self, device, feature_id):
        super(SeaBreezeSpectrometerFeatureOOI, self).__init__(device, feature_id)

    def set_trigger_mode(self, mode):
        if mode in self.device.trigger_modes:
            self.device.usb_send(struct.pack("<BH", 0x0A, mode))
        else:
            raise SeaBreezeError("Only supports: %s" % str(self.device.trigger_modes))

    def set_integration_time_micros(self, integration_time_micros):
        t_min = self.device.integration_time_min
        t_max = self.device.integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self.device.integration_time_base)
            self.device.usb_send(struct.pack('<BI', 0x02, i_time))
        else:
            raise SeaBreezeError("Integration not in [{:d}, {:d}]".format(t_min, t_max))

    def get_integration_time_micros_limits(self):
        return self.device.integration_time_min, self.device.integration_time_max

    def get_maximum_intensity(self):
        return self.device.spectrum_max_value

    def get_electric_dark_pixel_indices(self):
        return numpy.array(self.device.dark_pixel_indices)

    @property
    def _spectrum_length(self):
        return self.device.spectrum_num_pixel

    @property
    def _spectrum_raw_length(self):
        return self.device.spectrum_raw_length

    def get_wavelengths(self):
        indices = numpy.arange(self._spectrum_length, dtype=numpy.float64)
        # OOI spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            coeffs.append(float(self.device.f.eeprom.eeprom_read_slot(i)))
        return sum(wl * (indices ** i) for i, wl in enumerate(coeffs))

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1]), dtype=numpy.double)
        return ret * self._normalization_value

    def _get_spectrum_raw(self):
        tmp = numpy.empty((self._spectrum_raw_length,), dtype=numpy.uint8)
        self.device.usb_send(struct.pack('<B', 0x09))

        timeout = int(self.device.integration_time_max * 1e-3 + self.device.usbtimeout_ms)
        tmp[:] = self.device.usb_read_highspeed(size=self._spectrum_raw_length,
                                                timeout=timeout)
        return tmp

    def _get_fast_buffer_spectrum(self):
        raise NotImplementedError("implement in derived class")


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

    def __init__(self, device, feature_id):
        super(SeaBreezeSpectrometerFeatureOOIFPGA, self).__init__(device, feature_id)
        self.device.usb_send(struct.pack("<B", 0xFE))
        ret = self.device.usb_read(size=16)
        data = struct.unpack("<HLBBBBBBBBBB", ret[:])
        self.device.usbspeed = data[10]


class SeaBreezeSpectrometerFeatureOOIFPGA4K(SeaBreezeSpectrometerFeatureOOIFPGA):

    def _get_spectrum_raw(self):
        tmp = numpy.empty((self._spectrum_raw_length,), dtype=numpy.uint8)
        timeout = int(self.device.integration_time_max * 1e-3 + self.device.usbtimeout_ms)
        self.device.usb_send(struct.pack('<B', 0x09))
        if self.device.usbspeed == 0x00:  # lowspeed
            tmp[:] = self.device.usb_read_highspeed(size=self._spectrum_raw_length,
                                                    timeout=timeout)
        else:  # self.usbspeed == 0x80  highspeed
            tmp[:2048] = self.device.usb_read_highspeed_alt(size=2048,
                                                            timeout=timeout)
            tmp[2048:] = self.device.usb_read_highspeed(size=self._spectrum_raw_length - 2048,
                                                        timeout=timeout)
        return tmp


class _SeaBreezeSpectrometerSaturationMixin(object):

    def _saturation_unpack(self, ret):
        return struct.unpack("<H", ret[6:8])[0]

    def _saturation_not_initialized(self, x):
        return x == 0

    # noinspection PyUnresolvedReferences
    def _saturation_get_normalization_value(self):
        """internal only"""
        ret = self.device.eeprom.eeprom_read_slot(17, raw=True)
        # ret contains the first two response bytes, then the eeprom data
        saturation = self._saturation_unpack(ret)
        if self._saturation_not_initialized(saturation):
            # pass  # not initialized?
            return self._normalization_value
        else:
            return float(self.device.spectrum_max_value) / saturation


class SeaBreezeSpectrometerFeatureOOIGain(SeaBreezeSpectrometerFeatureOOI,
                                          _SeaBreezeSpectrometerSaturationMixin):

    def __init__(self, device, feature_id):
        # set the usbspeed
        super(SeaBreezeSpectrometerFeatureOOIGain, self).__init__(device, feature_id)
        # load the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIFPGAGain(SeaBreezeSpectrometerFeatureOOIFPGA,
                                              _SeaBreezeSpectrometerSaturationMixin):

    def __init__(self, device, feature_id):
        # set the usbspeed
        super(SeaBreezeSpectrometerFeatureOOIFPGAGain, self).__init__(device, feature_id)
        # load the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIFPGA4KGain(SeaBreezeSpectrometerFeatureOOIFPGA4K,
                                                _SeaBreezeSpectrometerSaturationMixin):

    def __init__(self, device, feature_id):
        # set the usbspeed
        super(SeaBreezeSpectrometerFeatureOOIFPGA4KGain, self).__init__(device, feature_id)
        # get the saturation value
        self._normalization_value = self._saturation_get_normalization_value()


class SeaBreezeSpectrometerFeatureOOIGainAlt(SeaBreezeSpectrometerFeatureOOIGain):
    # XXX: The NIRQUEST stores this value somewhere else
    #      and might also not have been programmed yet..
    # TODO: And this is planned for the QE65000 apparently.
    def _saturation_unpack(self, ret):
        return struct.unpack("<L", ret[6:10])[0]


class SeaBreezeSpectrometerFeatureOOIFPGAGainAlt(SeaBreezeSpectrometerFeatureOOIFPGAGain):
    # XXX: The Apex, Maya2000pro, MayaLSL store this value somewhere else
    #      and might also not have been programmed yet...
    # ret contains the first two response bytes, then the eeprom data
    def _saturation_unpack(self, ret):
        return struct.unpack("<H", ret[2:4])[0]

    def _saturation_not_initialized(self, x):
        return x <= 32768 or x > self.device.spectrum_max_value


# Spectrometer Features based on USBCommOBP
# =========================================
#
class SeaBreezeSpectrometerFeatureOBP(SeaBreezeSpectrometerFeature):
    required_interface_cls = USBCommOOI
    required_features = ()

    _normalization_value = 1.0

    def __init__(self, device, feature_id):
        super(SeaBreezeSpectrometerFeatureOBP, self).__init__(device, feature_id)

    def set_trigger_mode(self, mode):
        if mode in self.device.trigger_modes:
            self.device.send_command(0x00110110, struct.pack("<B", mode))
        else:
            raise SeaBreezeError("Only supports: %s" % str(self.device.trigger_modes))

    def set_integration_time_micros(self, integration_time_micros):
        t_min = self.device.integration_time_min
        t_max = self.device.integration_time_max
        if t_min <= integration_time_micros < t_max:
            i_time = int(integration_time_micros / self.device.integration_time_base)
            self.device.send_command(0x00110010, struct.pack("<L", i_time))
        else:
            raise SeaBreezeError("Integration not in [{:d}, {:d}]".format(t_min, t_max))

    def get_integration_time_micros_limits(self):
        return self.device.integration_time_min, self.device.integration_time_max

    def get_maximum_intensity(self):
        return self.device.spectrum_max_value

    def get_electric_dark_pixel_indices(self):
        return numpy.array(self.device.dark_pixel_indices)

    @property
    def _spectrum_length(self):
        return self.device.spectrum_num_pixel

    @property
    def _spectrum_raw_length(self):
        return self.device.spectrum_raw_length

    def get_wavelengths(self):
        # get number of wavelength coefficients
        data = self.device.query(0x00180100, "")
        N = struct.unpack("<B", data)[0]
        # now query the coefficients
        coeffs = []
        for i in range(N):
            data = self.device.query(0x00180101, struct.pack("<B", i))
            coeffs.append(struct.unpack("<f", data)[0])
        # and generate the wavelength array
        indices = numpy.arange(self._spectrum_length, dtype=numpy.float64)
        return sum(wl * (indices ** i) for i, wl in enumerate(coeffs))

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # noinspection PyTypeChecker
        return numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp), dtype=numpy.double)

    def _get_spectrum_raw(self):
        timeout = int(self.device.integration_time_max * 1e-3 + self.device.usbtimeout_ms)
        datastring = self.device.query(0x00101100, "", timeout=timeout)
        return numpy.fromstring(datastring, dtype=numpy.uint8)

    def _get_fast_buffer_spectrum(self):
        raise NotImplementedError("implement in derived class")


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
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1])) ^ 0x2000
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureUSB650(SeaBreezeSpectrometerFeatureOOI2K):
    pass


class SeaBreezeSpectrometerFeatureHR2000PLUS(SeaBreezeSpectrometerFeatureOOIFPGA):

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # The HR2000PLUS needs to xor with 0x2000
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1])) ^ 0x2000
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureQE65000(SeaBreezeSpectrometerFeatureOOIFPGA):

    def get_wavelengths(self):
        # QE65000 specific override
        indices = numpy.arange(-10, self._spectrum_length - 10, dtype=numpy.float64)
        # OOI spectrometers store the wavelength calibration in slots 1,2,3,4
        coeffs = []
        for i in range(1, 5):
            coeffs.append(float(self.device.f.eeprom.eeprom_read_slot(i)))
        return sum(wl * (indices ** i) for i, wl in enumerate(coeffs))

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # The QE65000 needs to xor with 0x8000
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1])) ^ 0x8000
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureUSB2000PLUS(SeaBreezeSpectrometerFeatureOOIFPGAGain):
    pass


class SeaBreezeSpectrometerFeatureUSB4000(SeaBreezeSpectrometerFeatureOOIFPGA4KGain):
    pass


class SeaBreezeSpectrometerFeatureNIRQUEST512(SeaBreezeSpectrometerFeatureOOIGainAlt):

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # The NIRQUEST512 needs to xor with 0x8000
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1])) ^ 0x8000
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureNIRQUEST256(SeaBreezeSpectrometerFeatureOOIGainAlt):

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # The NIRQUEST256 needs to xor with 0x8000
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:-1])) ^ 0x8000
        return ret.astype(numpy.double) * self._normalization_value


class SeaBreezeSpectrometerFeatureMAYA2000PRO(SeaBreezeSpectrometerFeatureOOIFPGAGainAlt):
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
        ret = numpy.array(struct.unpack("<" + "H" * self._spectrum_length, tmp[:]), dtype=numpy.double)
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureSTS(SeaBreezeSpectrometerFeatureOBP):
    pass


class SeaBreezeSpectrometerFeatureQEPRO(SeaBreezeSpectrometerFeatureOBP):

    def _get_spectrum_raw(self):
        timeout = int(self.device.integration_time_max * 1e-3 + self.device.usbtimeout_ms)
        datastring = self.device.query(0x00100928, "", timeout=timeout)
        return numpy.fromstring(datastring, dtype=numpy.uint8)

    def get_intensities(self):
        tmp = self._get_spectrum_raw()
        # 32byte metadata block at beginning
        ret = numpy.array(struct.unpack("<" + "I" * self._spectrum_length, tmp[32:]), dtype=numpy.double)
        return ret * self._normalization_value


class SeaBreezeSpectrometerFeatureVENTANA(SeaBreezeSpectrometerFeatureOBP):
    pass
