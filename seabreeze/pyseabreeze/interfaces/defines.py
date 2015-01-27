"""
File:           defines.py
Author:         Andreas Poehlmann

Some definitions
"""

VendorId = 0x2457
ModelNames = {
    0x1002: 'USB2000',
    0x100a: 'HR2000',
    0x1012: 'HR4000',
    0x1016: 'HR2000PLUS',
    0x1018: 'QE65000',
    0x101E: 'USB2000PLUS',
    0x1022: 'USB4000',
    0x1026: 'NIRQUEST512',
    0x1028: 'NIRQUEST256',
    0x102a: 'MAYA2000PRO',
    0x102c: 'MAYA2000',
    0x1040: 'TORUS',
    0x1044: 'APEX',
    0x2000: 'JAZ',
    0x4000: 'STS',
    0x4004: 'QEPRO',
    0x5000: 'VENTANA',
    0x1014: 'USB650',
}
ProductIds = ModelNames.keys()
SpectrometerClasses = ModelNames.values()

DarkPixels = {
    'USB2000'	  : range(2, 24),
    'HR2000'	  : range(2, 24),
    'HR4000'	  : range(2, 13),
    'HR2000PLUS'  : range(2, 24),
    'QE65000'	  : range(0, 4) + range(1040, 1044),  # as in seabreeze-3.0.5
    'USB2000PLUS' : range(2, 24),
    'USB4000'	  : range(2, 13),
    'NIRQUEST512' : [],
    'NIRQUEST256' : [],
    'MAYA2000PRO' : range(0, 4) + range(2064, 2068),
    'MAYA2000'	  : range(0, 8) + range(2072, 2080),
    'TORUS'	  : [],
    'APEX'	  : range(0, 4) + range(2064, 2068),
    'JAZ'	  : range(2, 24),
    'STS'	  : [],
    'QEPRO'	  : range(0, 4) + range(1040, 1044),
    'VENTANA'	  : [],
    'USB650'	  : [],  # ??? unkown
}


class _EndPointMap(object):
    def __init__(self, ep_out=None, lowspeed_in=None,
                 highspeed_in=None, highspeed_in2=None):
        self.ep_out = ep_out
        self.lowspeed_in = lowspeed_in
        self.highspeed_in = highspeed_in
        self.highspeed_in2 = highspeed_in2

_EML2K = _EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)
_EMFPG = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)
_EMDUA = _EndPointMap(ep_out=0x01, lowspeed_in=0x81)  # XXX: we'll ignore the alternative EPs
_EMVEN = _EndPointMap(ep_out=0x01, lowspeed_in=0x82)
_EMJAZ = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82)

EndPoints= {
     'USB2000'	   : _EML2K,
     'HR2000'	   : _EML2K,
     'HR4000'	   : _EMFPG,
     'HR2000PLUS'  : _EMFPG,
     'QE65000'	   : _EMFPG,
     'USB2000PLUS' : _EMFPG,
     'USB4000'	   : _EMFPG,
     'NIRQUEST512' : _EML2K,
     'NIRQUEST256' : _EML2K,
     'MAYA2000PRO' : _EMFPG,
     'MAYA2000'	   : _EMFPG,
     'TORUS'	   : _EMFPG,
     'APEX'	   : _EMFPG,
     'JAZ'	   : _EMJAZ,
     'STS'	   : _EMDUA,
     'QEPRO'	   : _EMDUA,
     'VENTANA'	   : _EMVEN,
     'USB650'	   : _EML2K,
}
