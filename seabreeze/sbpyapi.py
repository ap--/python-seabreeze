
__all__ = ['list_spectrometers', 'Spectrometer']

import seabreeze.sbcapi as csb
import numpy as np


def list_spectrometers():
    """list_spectrometers()

    list all available oceanoptics spectrometers
    """
    # Get number of connected spectrometers
    idx = 0
    while True:
        try:
            csb.open_spectrometer(idx)
        except csb.SeaBreezeError:
            break
        idx += 1
    N = idx

    # Get the required info and close the spectrometers
    out = []
    for i in range(N):
        out.append({'index': i,
                    'model': csb.get_model(i),
                    'serial': csb.get_serial_number(i)})
        csb.close_spectrometer(i)

    return out


class Spectrometer(object):
    def __init__(self, index):
        self._idx = index
        csb.open_spectrometer(index)
        self._pixels = csb.get_formatted_spectrum_length(self._idx)
        self._min_itime_us = csb.get_min_integration_time_microsec(self._idx)

        # load wavelengths
        self._wavelengths = np.zeros((self._pixels,), dtype=np.double)
        csb.get_wavelengths(self._idx, self._wavelengths)

        # get dark pixel indices
        self._dark = csb.get_electric_dark_pixel_indices(self._idx)

    def wavelengths(self):
        return self._wavelengths

    def intensities(self):
        out = np.empty((self._pixels,), dtype=np.double)
        csb.get_formatted_spectrum(self._idx, out)
        return out

    def spectrum(self):
        return np.vstack((self._wavelengths, self.intensities()))

    def integration_time(self, time_sec):
        time_us = int(time_sec * 1000000)
        csb.set_integration_time_microsec(self._idx, time_us)

    def close(self):
        csb.close_spectrometer(self._idx)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()



