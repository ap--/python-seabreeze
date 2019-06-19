
from .common import SeaBreezeError, get_pyseabreeze_decorator
from .communication import USBCommOOI, USBCommOBP
from .defines import ModelNames, DarkPixels, TriggerModes
from .wavelength import WavelengthCoefficientsEEPromFeature
from .eeprom import EEPromFeature

import struct
import numpy
import time

convert_exceptions = get_pyseabreeze_decorator('interfaces.spectrometer')

#==========================#
# OOI SpectrometerFeatures #
#==========================#

class SpectrometerFeatureOOI(WavelengthCoefficientsEEPromFeature,
                             EEPromFeature,
                             USBCommOOI):

    _INTEGRATION_TIME_MIN = None
    _INTEGRATION_TIME_MAX = None
    _INTEGRATION_TIME_BASE = None
    _PIXELS = None
    _RAW_SPECTRUM_LEN = None
    _MAX_PIXEL_VALUE = None
    _SATURATION_VALUE = None

    def has_spectrometer_feature(self):
        return [True]

    def _initialize_common(self):
        if self._SATURATION_VALUE is None:
            self._NORMALIZATION_VALUE = 1.
        else:
            self._NORMALIZATION_VALUE = float(self._MAX_PIXEL_VALUE) / self._SATURATION_VALUE

    def _initialize_model_specific(self):
        """This function should be used for model specific initializations"""
        pass

    @convert_exceptions("An error occured during opening.")
    def open(self, device):
        # open the usb device with USBCommBase
        self.open_device(device.handle)
        # initialize the spectrometer
        self.usb_send(struct.pack('<B', 0x01))
        time.sleep(0.1)  # wait shortly after init command
        self._initialize_common()
        self._initialize_model_specific()

    @convert_exceptions("An error occured during closing.")
    def close(self):
        # close the usb device with USBCommBase
        self.close_device()

    @convert_exceptions("Is the spectrometer initialized?")
    def get_model(self):
        # return model name with lookuptable
        return ModelNames[self._device.idProduct]

    @convert_exceptions("Trigger mode error.")
    def set_trigger_mode(self, mode):
        if mode in list(TriggerModes[self.get_model()].values()):
            self.usb_send(struct.pack("<BH", 0x0A, mode))
        else:
            raise SeaBreezeError("Only supports: %s" % list(TriggerModes[self.get_model()].keys()))

    @convert_exceptions("Integration time error.")
    def set_integration_time_microsec(self, integration_time_micros):
        if self._INTEGRATION_TIME_MIN <= integration_time_micros < self._INTEGRATION_TIME_MAX:
            itime = int(integration_time_micros / self._INTEGRATION_TIME_BASE)
            self.usb_send(struct.pack('<BI', 0x02, itime))
        else:
            raise SeaBreezeError("Integration time must be in range(%d, %d)." %
                                 (self._INTEGRATION_TIME_MIN, self._INTEGRATION_TIME_MAX))

    @convert_exceptions("Is the spectrometer initialized?")
    def get_min_integration_time_microsec(self):
        return self._INTEGRATION_TIME_MIN

    @convert_exceptions("Error while reading raw spectrum.")
    def get_unformatted_spectrum(self, out):
        self.usb_send(struct.pack('<B', 0x09))
        out[:] = self.usb_read_highspeed(size=self._RAW_SPECTRUM_LEN,
                        timeout=int(self._INTEGRATION_TIME_MAX * 1e-3 + self.usbtimeout_ms))
        return self._RAW_SPECTRUM_LEN  # compatibility

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp[:-1]), dtype=numpy.double)
        out[:] = ret * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility

    @convert_exceptions("")
    def get_unformatted_spectrum_length(self):
        return self._RAW_SPECTRUM_LEN

    @convert_exceptions("")
    def get_formatted_spectrum_length(self):
        return self._PIXELS

    @convert_exceptions("")
    def get_wavelengths(self, out):
        indices = numpy.arange(self._PIXELS, dtype=numpy.float64)
        wlcoeff = self.get_wavelength_coefficients()
        out[:] = sum( wl * (indices**i) for i, wl in enumerate(wlcoeff) )
        return self._PIXELS

    @convert_exceptions("Is the spectrometer initialized?")
    def get_electric_dark_pixel_indices(self):
        # return model name with lookuptable
        return numpy.array(DarkPixels[self.get_model()])

    @convert_exceptions("Error when reading serial number.")
    def get_serial_number(self):
        # The serial is stored in slot 0
        return str(self.read_eeprom_slot(0))


class SpectrometerFeatureOOI2K(SpectrometerFeatureOOI):

    @convert_exceptions("Error while reading raw spectrum")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # The byte order is different for some models
        tsorted = numpy.empty((self._RAW_SPECTRUM_LEN - 1), dtype=numpy.uint8)
        idx = [(i//2)%64 + (i%2)*64 + (i//(128))*(128) for i in range(self._RAW_SPECTRUM_LEN-1)]
        tsorted = tmp[idx] & numpy.array((0xFF, 0x0F)*(self._PIXELS), dtype=numpy.uint8)  # high nibble not guaranteed to be pulled low
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tsorted), dtype=numpy.double)
        # sorted and parsed
        out[:] = ret * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility


class SpectrometerFeatureOOIFPGA(SpectrometerFeatureOOI):

    def _initialize_model_specific(self):
        self.usb_send(struct.pack("<B", 0xFE))
        ret = self.usb_read(size=16)
        data = struct.unpack("<HLBBBBBBBBBB", ret[:])
        self.usbspeed = data[10]


class SpectrometerFeatureOOIFPGA4K(SpectrometerFeatureOOIFPGA):

    @convert_exceptions("Error while reading raw spectrum")
    def get_unformatted_spectrum(self, out):
        self.usb_send(struct.pack('<B', 0x09))
        if self.usbspeed == 0x00:  # lowspeed
            out[:] = self.usb_read_highspeed(size=self._RAW_SPECTRUM_LEN,
                        timeout=int(self._INTEGRATION_TIME_MAX * 1e-3 + self.usbtimeout_ms))
        else: # self.usbspeed == 0x80  highspeed
            out[:2048] = self.usb_read_highspeed_alt(size=2048,
                        timeout=int(self._INTEGRATION_TIME_MAX * 1e-3 + self.usbtimeout_ms))
            out[2048:] = self.usb_read_highspeed(size=self._RAW_SPECTRUM_LEN - 2048,
                        timeout=int(self._INTEGRATION_TIME_MAX * 1e-3 + self.usbtimeout_ms))
        return self._RAW_SPECTRUM_LEN  # compatibility


class SpectrometerFeatureOOIGain(SpectrometerFeatureOOI):

    def _initialize_model_specific(self):
        # set the usbspeed
        super(SpectrometerFeatureOOIGain, self)._initialize_model_specific()
        # get the saturation value
        ret = self.read_eeprom_slot_raw(17)
        # ret contains the first two response bytes, then the eeprom data
        saturation = struct.unpack("<H", ret[6:8])[0]
        if saturation == 0:
            pass  # not initialized?
        else:
            self._NORMALIZATION_VALUE = float(self._MAX_PIXEL_VALUE) / saturation


class SpectrometerFeatureOOIFPGAGain(SpectrometerFeatureOOIFPGA):

    def _initialize_model_specific(self):
        # set the usbspeed
        super(SpectrometerFeatureOOIFPGAGain, self)._initialize_model_specific()
        # get the saturation value
        ret = self.read_eeprom_slot_raw(17)
        # ret contains the first two response bytes, then the eeprom data
        saturation = struct.unpack("<H", ret[6:8])[0]
        if saturation == 0:
            pass  # not initialized?
        else:
            self._NORMALIZATION_VALUE = float(self._MAX_PIXEL_VALUE) / saturation


class SpectrometerFeatureOOIFPGA4KGain(SpectrometerFeatureOOIFPGA4K):

    def _initialize_model_specific(self):
        # set the usbspeed
        super(SpectrometerFeatureOOIFPGA4KGain, self)._initialize_model_specific()
        # get the saturation value
        ret = self.read_eeprom_slot_raw(17)
        # ret contains the first two response bytes, then the eeprom data
        saturation = struct.unpack("<H", ret[6:8])[0]
        if saturation == 0:
            pass  # not initialized?
        else:
            self._NORMALIZATION_VALUE = float(self._MAX_PIXEL_VALUE) / saturation


class SpectrometerFeatureOOIGainAlt(SpectrometerFeatureOOIGain):

    def _initialize_model_specific(self):
        # set the usbspeed
        super(SpectrometerFeatureOOIGainAlt, self)._initialize_model_specific()
        # get the saturation value
        ret = self.read_eeprom_slot_raw(17)
        # XXX: The NIRQUEST stores this value somewhere else
        #      and might also not have been programmed yet..
        # TODO: And this is planned for the QE65000 apparently.
        # ret contains the first two response bytes, then the eeprom data
        saturation = struct.unpack("<L", ret[6:10])[0]
        if saturation == 0:
            pass  # not initialized?
        else:
            self._NORMALIZATION_VALUE = float(self._MAX_PIXEL_VALUE) / saturation


class SpectrometerFeatureOOIFPGAGainAlt(SpectrometerFeatureOOIFPGAGain):

    def _initialize_model_specific(self):
        # set the usbspeed
        super(SpectrometerFeatureOOIFPGAGainAlt, self)._initialize_model_specific()
        # get the saturation value
        ret = self.read_eeprom_slot_raw(17)
        # XXX: The Apex, Maya2000pro, MayaLSL store this value somewhere else
        #      and might also not have been programmed yet...
        # ret contains the first two response bytes, then the eeprom data
        saturation = struct.unpack("<H", ret[2:4])[0]
        if saturation <= 32768 or saturation > self._MAX_PIXEL_VALUE:
            pass  # not initialized?
        else:
            self._NORMALIZATION_VALUE = float(self._MAX_PIXEL_VALUE) / saturation


#==========================#
# OBP SpectrometerFeatures #
#==========================#

class SpectrometerFeatureOBP(USBCommOBP):

    _INTEGRATION_TIME_MIN = None
    _INTEGRATION_TIME_MAX = None
    _INTEGRATION_TIME_BASE = None
    _PIXELS = None
    _RAW_SPECTRUM_LEN = None
    _MAX_PIXEL_VALUE = None
    _SATURATION_VALUE = None

    def has_spectrometer_feature(self):
        return [True]

    def _initialize_common(self):
        pass

    def _initialize_model_specific(self):
        """This function should be used for model specific initializations"""
        pass

    @convert_exceptions("An error occured during opening.")
    def open(self, device):
        # open the usb device with USBCommBase
        self.open_device(device.handle)
        self._initialize_common()
        self._initialize_model_specific()

    @convert_exceptions("An error occured during closing.")
    def close(self):
        # close the usb device with USBCommBase
        self.close_device()

    @convert_exceptions("Is the spectrometer initialized?")
    def get_model(self):
        # return model name with lookuptable
        return ModelNames[self._device.idProduct]

    @convert_exceptions("Trigger mode error.")
    def set_trigger_mode(self, mode):
        if mode in list(TriggerModes[self.get_model()].values()):
            self.send_command(0x00110110, struct.pack("<B", mode))
        else:
            raise SeaBreezeError("Only supports: %s" % list(TriggerModes[self.get_model()].keys()))

    @convert_exceptions("Integration time error.")
    def set_integration_time_microsec(self, integration_time_micros):
        if self._INTEGRATION_TIME_MIN <= integration_time_micros < self._INTEGRATION_TIME_MAX:
            itime = int(integration_time_micros / self._INTEGRATION_TIME_BASE)
            self.send_command(0x00110010, struct.pack("<L", itime))
        else:
            raise SeaBreezeError("Integration time must be in range(%d, %d)." %
                                 (self._INTEGRATION_TIME_MIN, self._INTEGRATION_TIME_MAX))

    @convert_exceptions("Is the spectrometer initialized?")
    def get_min_integration_time_microsec(self):
        return self._INTEGRATION_TIME_MIN

    @convert_exceptions("Error while reading raw spectrum.")
    def get_unformatted_spectrum(self, out):
        datastring = self.query(0x00101100, "",
                    timeout=int(self._INTEGRATION_TIME_MAX * 1e-3 + self.usbtimeout_ms))
        out[:] = numpy.fromstring(datastring, dtype=numpy.uint8)
        return self._RAW_SPECTRUM_LEN  # compatibility

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        out[:] = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp), dtype=numpy.double)
        return self._PIXELS  # compatibility

    @convert_exceptions("")
    def get_unformatted_spectrum_length(self):
        return self._RAW_SPECTRUM_LEN

    @convert_exceptions("")
    def get_formatted_spectrum_length(self):
        return self._PIXELS

    @convert_exceptions("")
    def get_wavelengths(self, out):
        # get number of wavelength coefficients
        data = self.query(0x00180100, "")
        N = struct.unpack("<B", data)[0]
        # now query the coefficients
        coeffs = []
        for i in range(N):
            data = self.query(0x00180101, struct.pack("<B", i))
            coeffs.append(struct.unpack("<f", data)[0])
        # and generate the wavelength array
        indices = numpy.arange(self._PIXELS, dtype=numpy.float64)
        out[:] = sum( wl * (indices**i) for i, wl in enumerate(coeffs) )
        return self._PIXELS

    @convert_exceptions("Is the spectrometer initialized?")
    def get_electric_dark_pixel_indices(self):
        # return model name with lookuptable
        return numpy.array(DarkPixels[self.get_model()])

    @convert_exceptions("Error when reading serial number.")
    def get_serial_number(self):
        return self.query(0x00000100, "")


#========================#
# Model specific changes #
#========================#
class SpectrometerFeatureUSB2000(SpectrometerFeatureOOI2K):
    pass


class SpectrometerFeatureHR2000(SpectrometerFeatureOOI2K):
    pass


class SpectrometerFeatureHR4000(SpectrometerFeatureOOIFPGA4K):

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # The HR4000 needs to xor with 0x2000
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp[:-1])) ^ 0x2000
        out[:] = ret.astype(numpy.double) * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility


class SpectrometerFeatureUSB650(SpectrometerFeatureOOI2K):
    pass


class SpectrometerFeatureHR2000PLUS(SpectrometerFeatureOOIFPGA):

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # The HR2000Plus needs to xor with 0x2000
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp[:-1])) ^ 0x2000
        out[:] = ret.astype(numpy.double) * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility


class SpectrometerFeatureQE65000(SpectrometerFeatureOOIFPGA):

    @convert_exceptions("")
    def get_wavelengths(self, out):
        indices = numpy.arange(-10, self._PIXELS - 10, dtype=numpy.float64)
        wlcoeff = self.get_wavelength_coefficients()
        out[:] = sum( wl * (indices**i) for i, wl in enumerate(wlcoeff) )
        return self._PIXELS

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # The QE65000 needs to xor with 0x8000
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp[:-1])) ^ 0x8000
        out[:] = ret.astype(numpy.double) * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility


class SpectrometerFeatureUSB2000PLUS(SpectrometerFeatureOOIFPGAGain):
    pass


class SpectrometerFeatureUSB4000(SpectrometerFeatureOOIFPGA4KGain):
    pass


class SpectrometerFeatureNIRQUEST512(SpectrometerFeatureOOIGainAlt):

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # The NIRQUEST512 needs to xor with 0x8000
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp[:-1])) ^ 0x8000
        out[:] = ret.astype(numpy.double) * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility

class SpectrometerFeatureNIRQUEST256(SpectrometerFeatureOOIGainAlt):

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # The NIRQUEST256 needs to xor with 0x8000
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp[:-1])) ^ 0x8000
        out[:] = ret.astype(numpy.double) * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility


class SpectrometerFeatureMAYA2000PRO(SpectrometerFeatureOOIFPGAGainAlt):
    pass


class SpectrometerFeatureMAYA2000(SpectrometerFeatureOOIFPGA):
    pass


class SpectrometerFeatureTORUS(SpectrometerFeatureOOIFPGAGain):
    # The Torus uses the USB2000Plus spec feature
    pass


class SpectrometerFeatureAPEX(SpectrometerFeatureOOIFPGAGainAlt):
    pass


class SpectrometerFeatureMAYALSL(SpectrometerFeatureOOIFPGAGainAlt):
    pass


class SpectrometerFeatureJAZ(SpectrometerFeatureOOIGain):

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # XXX: No sync byte for the Jaz
        ret = numpy.array(struct.unpack("<" + "H"*self._PIXELS, tmp), dtype=numpy.double)
        out[:] = ret * self._NORMALIZATION_VALUE
        return self._PIXELS  # compatibility


class SpectrometerFeatureSTS(SpectrometerFeatureOBP):
    pass

class SpectrometerFeatureQEPRO(SpectrometerFeatureOBP):

    @convert_exceptions("Error while reading raw spectrum.")
    def get_unformatted_spectrum(self, out):
        datastring = self.query(0x00100928, "",
                    timeout=int(self._INTEGRATION_TIME_MAX * 1e-3 + self.usbtimeout_ms))
        out[:] = numpy.fromstring(datastring, dtype=numpy.uint8)
        return self._RAW_SPECTRUM_LEN  # compatibility

    @convert_exceptions("Error while reading formatted spectrum.")
    def get_formatted_spectrum(self, out):
        tmp = numpy.empty((self._RAW_SPECTRUM_LEN), dtype=numpy.uint8)
        self.get_unformatted_spectrum(tmp)
        # 32byte metadata block at beginning
        out[:] = numpy.array(struct.unpack("<" + "I"*self._PIXELS, tmp[32:]), dtype=numpy.double)
        return self._PIXELS  # compatibility

class SpectrometerFeatureVENTANA(SpectrometerFeatureOBP):
    pass


