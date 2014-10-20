
__all__ = ['list_spectrometers', 'Spectrometer']

import seabreeze.sbcapi as csb


def open_spectrometer(index=None, model=None, serialnumber=None):
    pass


def list_spectrometers():
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
    pass
