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
    0x1014: 'USB650',
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
    0x1046: 'MAYALSL',
    0x2000: 'JAZ',
    0x4000: 'STS',
    0x4004: 'QEPRO',
    0x5000: 'VENTANA',
}
ProductIds = list(ModelNames.keys())
SpectrometerClasses = list(ModelNames.values())

DarkPixels = {
    'USB2000'	  : list(range(2, 24)),
    'HR2000'	  : list(range(2, 24)),
    'HR4000'	  : list(range(2, 13)),
    'USB650'	  : [],  # ??? unkown
    'HR2000PLUS'  : list(range(2, 24)),
    'QE65000'	  : list(range(0, 4)) + list(range(1040, 1044)),  # as in seabreeze-3.0.5
    'USB2000PLUS' : list(range(6, 21)),  # as in seabreeze-3.0.9
    'USB4000'	  : list(range(5, 16)),  # as in seabreeze-3.0.9
    'NIRQUEST512' : [],
    'NIRQUEST256' : [],
    'MAYA2000PRO' : list(range(0, 4)) + list(range(2064, 2068)),
    'MAYA2000'	  : list(range(0, 8)) + list(range(2072, 2080)),
    'TORUS'	  : [],
    'APEX'	  : list(range(0, 4)) + list(range(2064, 2068)),
    'MAYALSL'     : list(range(0, 4)) + list(range(2064, 2068)),
    'JAZ'	  : list(range(2, 24)),
    'STS'	  : [],
    'QEPRO'	  : list(range(0, 4)) + list(range(1040, 1044)),
    'VENTANA'	  : [],
}


class _EndPointMap(object):
    def __init__(self, ep_out=None, lowspeed_in=None,
                 highspeed_in=None, highspeed_in2=None):
        self.ep_out = ep_out
        self.lowspeed_in = lowspeed_in
        self.highspeed_in = highspeed_in
        self.highspeed_in2 = highspeed_in2

_EML2K = _EndPointMap(ep_out=0x02, lowspeed_in=0x87, highspeed_in=0x82)
_EMFPGA = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82, highspeed_in2=0x86)
_EMDUAL = _EndPointMap(ep_out=0x01, lowspeed_in=0x81)  # XXX: we'll ignore the alternative EPs
_EMVENT = _EndPointMap(ep_out=0x01, lowspeed_in=0x82)
_EMJAZ = _EndPointMap(ep_out=0x01, lowspeed_in=0x81, highspeed_in=0x82)

EndPoints= {
     'USB2000'	   : _EML2K,
     'HR2000'	   : _EML2K,
     'HR4000'	   : _EMFPGA,  # usbspeed  ^0x2000
     'USB650'	   : _EML2K,  # ??? assumed
     'HR2000PLUS'  : _EMFPGA,  # usbspeed  ^0x2000
     'QE65000'	   : _EMFPGA,  # usbspeed  ^0x8000
     'USB2000PLUS' : _EMFPGA,  # usbspeed
     'USB4000'	   : _EMFPGA,  # usbspeed
     'NIRQUEST512' : _EMFPGA,
     'NIRQUEST256' : _EMFPGA,
     'MAYA2000PRO' : _EMFPGA,  # usbspeed
     'MAYA2000'	   : _EMFPGA,  # usbspeed
     'TORUS'	   : _EMFPGA,  # usbspeed
     'APEX'	   : _EMFPGA,  # usbspeed
     'MAYALSL'     : _EMFPGA,  # usbspeed
     'JAZ'	   : _EMJAZ,
     'STS'	   : _EMDUAL,
     'QEPRO'	   : _EMDUAL,
     'VENTANA'	   : _EMVENT,
}


TriggerModes = {
    'USB2000' : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 3,
    },
    'HR2000' : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 3,
    },
    'HR4000'	   : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 3,
       'EXT_HW_SYNC'  : 2,
    },
    'HR2000PLUS'  : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 2,
       'EXT_HW_SYNC'  : 3,
       'EXT_HW_EDGE'  : 4,
        },
    'QE65000'	   : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 3,
       'EXT_HW_SYNC'  : 2,
        },
    'USB2000PLUS' : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 2,
       'EXT_HW_SYNC'  : 3,
       'EXT_HW_EDGE'  : 4,
        },
    'USB4000'	   : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 3,
       'EXT_HW_SYNC'  : 2,
        },
    'NIRQUEST512' : {
       'FREE_RUNNING' : 0,
       'EXT_HW'       : 2,
        },
    'NIRQUEST256' : {
       'FREE_RUNNING' : 0,
       'EXT_HW'       : 2,
        },
    'MAYA2000PRO' : {
       'FREE_RUNNING' : 0,
       'EXT_HW'       : 1,
       'EXT_HW_SYNC'  : 2,
       'EXT_HW_EDGE'  : 3,
        },
    'MAYA2000'	   : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'REALTIME'     : 3,
        },
    'MAYALSL'	   : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'REALTIME'     : 3,
        },
    'TORUS'	   : {
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 2,
       'EXT_HW_SYNC'  : 3,
       'EXT_HW_EDGE'  : 4,
        },
    'APEX'	   : dict(),
    'JAZ'	   : dict(),  # FIXME
    'STS'	   : {
       'FREE_RUNNING' : 2,
       'SOFTWARE'     : 0,
       'EXT_HW'       : 1,
        },
    'QEPRO'	   : dict(),  # FIXME
    'VENTANA'	   : dict(),  # FIXME
    'USB650'	   : {  # ??? assumed.
       'FREE_RUNNING' : 0,
       'SOFTWARE'     : 1,
       'EXT_HW'       : 3,
        },
}




