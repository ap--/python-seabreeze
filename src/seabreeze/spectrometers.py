"""
seabreeze.spectrometers core module
===================================

Author: Andreas Poehlmann
Email: andreas@poehlmann.io

"""
from __future__ import annotations

from typing import TYPE_CHECKING

import numpy

import seabreeze.backends
from seabreeze.types import SeaBreezeAPI
from seabreeze.types import SeaBreezeBackend
from seabreeze.types import SeaBreezeFeatureAccessor
from seabreeze.types import SeaBreezeFeatureDict

# get the backend and add some functions/classes to this module
_lib: SeaBreezeBackend = seabreeze.backends.get_backend()

if TYPE_CHECKING:
    from numpy.typing import NDArray

    from seabreeze.types import SeaBreezeDevice

else:
    SeaBreezeDevice = _lib.SeaBreezeDevice
    SeaBreezeError = _lib.SeaBreezeError

__all__ = [
    "list_devices",
    "SeaBreezeError",
    "Spectrometer",
]


def list_devices() -> list[SeaBreezeDevice]:
    """returns available SeaBreezeDevices

    list all connected Ocean Optics devices supported

    Returns
    -------
    devices: `list[SeaBreezeDevice]`
        connected Spectrometer instances
    """
    api: SeaBreezeAPI
    try:
        api = list_devices._api  # type: ignore
    except AttributeError:
        _kw = _lib._api_kwargs
        api = list_devices._api = _lib.SeaBreezeAPI(**_kw)  # type: ignore
    return api.list_devices()


class Spectrometer:
    """Spectrometer class for all supported spectrometers"""

    # store reference to backend to allow backend switching in tests
    _backend: SeaBreezeBackend = _lib

    def __init__(self, device: SeaBreezeDevice) -> None:
        """create a Spectrometer instance for the provided device

        The Spectrometer class provides a thin abstraction layer for the
        basic spectrometer feature of the provided SeaBreezeDevice.

        Parameters
        ----------
        device : `seabreeze.spectrometers.SeaBreezeDevice`
            a SeaBreezeDevice as returned from `list_devices`
        """
        if not isinstance(device, self._backend.SeaBreezeDevice):
            raise TypeError("device has to be a `SeaBreezeDevice`")
        self._dev = device
        self.open()  # always open the device here to allow caching values

        # check for nonlinearity correction support
        nc_feature = self._dev.f.nonlinearity_coefficients
        self._nc = None
        if nc_feature is not None:
            try:
                # NOTE: the spark spectrometer raises a transport error when trying
                # to receive the nc coefficients. In this case continue with disabled
                # nonlinearity correction support
                self._nc = numpy.poly1d(
                    nc_feature.get_nonlinearity_coefficients()[::-1]
                )
            except self._backend.SeaBreezeError:
                pass
        # check for dark pixel correction support
        self._dp = self._dev.f.spectrometer.get_electric_dark_pixel_indices()
        # cache wavelengths on open
        self._wavelengths = self._dev.f.spectrometer.get_wavelengths()

    @classmethod
    def from_first_available(cls) -> Spectrometer:
        """open first available spectrometer

        Returns
        -------
        spectrometer : `Spectrometer`
            the first available supported spectrometer
        """
        for dev in list_devices():
            if not dev.is_open:
                return cls(dev)
        else:
            raise cls._backend.SeaBreezeError("No unopened device found.")

    @classmethod
    def from_serial_number(cls, serial: str | None = None) -> Spectrometer:
        """open the spectrometer matching the provided serial number

        Allows to open a specific spectrometer if multiple are connected.
        Spectrometer serial numbers are visible in the repr string of
        each `SeaBreezeDevice` or their `serial_number` attribute.

        Parameters
        ----------
        serial : `str`, optional
            the spectrometer's serial number. If `None` (default) it
            returns the first available unopened spectrometer.

        Returns
        -------
        spectrometer : `Spectrometer`
            the spectrometer with the requested serial number
        """
        if serial is None:  # pick first spectrometer
            return cls.from_first_available()

        for dev in list_devices():
            if dev.serial_number == str(serial):
                if dev.is_open:
                    raise cls._backend.SeaBreezeError("Device already opened.")
                else:
                    return cls(dev)
        else:
            raise cls._backend.SeaBreezeError(
                "No device attached with serial number '%s'." % serial
            )

    def wavelengths(self) -> NDArray[numpy.float_]:
        """wavelength array of the spectrometer

        wavelengths in (nm) corresponding to each pixel of the spectrometer

        Returns
        -------
        wavelengths : `numpy.ndarray`
            wavelengths in (nm)
        """
        return self._wavelengths

    def intensities(
        self, correct_dark_counts: bool = False, correct_nonlinearity: bool = False
    ) -> NDArray[numpy.float_]:
        """measured intensity array in (a.u.)

        Measured intensities as numpy array returned by the spectrometer.
        The measuring behavior can be adjusted by setting the trigger mode.
        Pixels at the start and end of the array might not be optically
        active so interpret their returned measurements with care. Refer
        to the spectrometer's datasheet for further information.

        Notes
        -----
        Intensities are in arbitrary units and the range depends on the
        ADC bit resolution of the hardware used in the specific spectrometer.
        Some spectrometers store a `saturation` value in their eeprom,
        which is used to rescale the raw ADC output to the full bit range.
        (This is done in `libseabreeze` and therefore also in `cseabreeze`
        --- for compatibility reasons the same is done in `pyseabreeze`)
        I.e. this means that a 16bit (max value 65535) spectrometer with a
        saturation value of ~30000 is effectively only returning ~15bit
        resolution raw readings. While most of the lower bits are dominated
        by noise anyways, it's just something to keep in mind. Refer to
        `pyseabreeze.features.spectrometer._SeaBreezeSpectrometerSaturationMixin`
        for the implementation.

        Parameters
        ----------
        correct_dark_counts : `bool`
            If requested and supported the average value of electric dark
            pixels on the ccd of the spectrometer is subtracted from the
            measurements to remove the noise floor in the measurements
            caused by non optical noise sources.
        correct_nonlinearity : `bool`
            Some spectrometers store non linearity correction coefficients
            in their eeprom. If requested and supported by the spectrometer
            the readings returned by the spectrometer will be linearized
            using the stored coefficients.

        Returns
        -------
        intensities : `numpy.ndarray`
            measured intensities in (a.u.)
        """
        if correct_dark_counts and not self._dp:
            raise self._backend.SeaBreezeError(
                "This device does not support dark count correction."
            )
        if correct_nonlinearity and not self._nc:
            raise self._backend.SeaBreezeError(
                "This device does not support nonlinearity correction."
            )
        # Get the intensities
        out = self._dev.f.spectrometer.get_intensities()
        # Do corrections if requested
        if correct_nonlinearity or correct_dark_counts:
            dark_offset = numpy.mean(out[self._dp]) if self._dp else 0.0
            out -= dark_offset
        if correct_nonlinearity and self._nc:
            out = out / numpy.polyval(self._nc, out)
        if correct_nonlinearity and (not correct_dark_counts):
            # noinspection PyUnboundLocalVariable
            out += dark_offset
        return out

    @property
    def max_intensity(self) -> float:
        """return the maximum intensity of the spectrometer

        Returns
        -------
        max_intensity : `float`
            the maximum intensity that can be returned by the spectrometer in (a.u.)
            It's possible that the spectrometer saturates already at lower values.
        """
        return self._dev.f.spectrometer.get_maximum_intensity()

    def spectrum(
        self, correct_dark_counts: bool = False, correct_nonlinearity: bool = False
    ) -> NDArray[numpy.float_]:
        """returns wavelengths and intensities as single array

        Convenience method to allow:

        >>> spec = Spectrometer.from_first_available()
        >>> wavelengths, intensities = spec.spectrum()

        Parameters
        ----------
        correct_dark_counts : `bool`
            see `Spectrometer.intensities`
        correct_nonlinearity : `bool`
            see `Spectrometer.intensities`

        Returns
        -------
        spectrum : `numpy.ndarray`
            combined array of wavelengths and measured intensities
        """
        return numpy.vstack(
            (
                self._wavelengths,
                self.intensities(correct_dark_counts, correct_nonlinearity),
            )
        )

    def integration_time_micros(self, integration_time_micros: int) -> None:
        """set the integration time in microseconds

        Parameters
        ----------
        integration_time_micros : `int`
            integration time in microseconds
        """
        # NOTE: Protect against a bug in libseabreeze:
        # If integration time is out of bounds, libseabreeze returns Undefined Error
        # (Probably only for devices with a non micro second time base...)
        try:
            self._dev.f.spectrometer.set_integration_time_micros(
                integration_time_micros
            )
        except OverflowError:
            raise self._backend.SeaBreezeError(
                "[OverFlow] Specified integration time is out of range."
            )
        except self._backend.SeaBreezeError as e:
            if getattr(e, "error_code", None) == 1:
                # Only replace if 'Undefined Error'
                raise self._backend.SeaBreezeError(
                    "[Fix] Specified integration time is out of range."
                )
            else:
                raise e

    @property
    def integration_time_micros_limits(self) -> tuple[int, int]:
        """return the hardcoded minimum and maximum integration time

        Returns
        -------
        integration_time_micros_min_max : `tuple[int, int]`
            min and max integration time in micro seconds
        """
        return self._dev.f.spectrometer.get_integration_time_micros_limits()

    def trigger_mode(self, mode: int) -> None:
        """set the trigger mode of the device

        Parameters
        ----------
        mode : `int`
            refer to your spectrometer's datasheet to determine the correct
            value for the trigger mode you want to use.
        """
        self._dev.f.spectrometer.set_trigger_mode(mode)

    @property
    def serial_number(self) -> str:
        """the spectrometer's serial number"""
        return self._dev.serial_number

    @property
    def model(self) -> str:
        """the spectrometer's model type"""
        return self._dev.model

    @property
    def pixels(self) -> int:
        """the spectrometer's number of pixels"""
        # noinspection PyProtectedMember
        return self._dev.f.spectrometer._spectrum_length

    @property
    def features(self) -> SeaBreezeFeatureDict:
        """return a dictionary of all supported features

        this returns a dictionary with all supported Features of the spectrometer
        and gives direct access to the features provided by the backend interface.

        Returns
        -------
        features :
            a dictionary mapping feature names to lists of feature instances.
            An empty list means the feature is not available.
        """
        return self._dev.features

    @property
    def f(self) -> SeaBreezeFeatureAccessor:
        """convenience assess to features via attributes

        this allows you to access a feature like this::

        >>> spec = Spectrometer.from_first_available()
        >>> # via .features
        >>> spec.features['eeprom'][0].eeprom_read_slot(4)
        >>> # via .f
        >>> spec.f.eeprom.eeprom_read_slot(4)

        """
        return self._dev.f

    def open(self) -> None:
        """open the connection to the SeaBreezeDevice

        Notes
        -----
        Normally you do not have to call this function manually.
        If you're trying to use the same spectrometer from multiple
        processes (honestly, why would you?) then this might come
        in handy. But I'll leave the lock/semaphore handling to you.
        """
        self._dev.open()

    def close(self) -> None:
        """close the connection to the SeaBreezeDevice

        Notes
        -----
        Normally you do not have to call this function manually.
        If you're trying to use the same spectrometer from multiple
        processes (honestly, why would you?) then this might come
        in handy. But I'll leave the lock/semaphore handling to you.
        """
        self._dev.close()

    def __repr__(self) -> str:
        return f"<Spectrometer {self.model}:{self.serial_number}>"
