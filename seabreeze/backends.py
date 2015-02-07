import warnings

__SEABREEZE_BACKEND = 'cseabreeze'

def use(backend):
    if not backend in ['cseabreeze', 'pyseabreeze']:
        raise Exception('Choose backend from "cseabreeze", "pyseabreeze"')
    else:
        global __SEABREEZE_BACKEND
        __SEABREEZE_BACKEND = backend

def get_backend():
    global __SEABREEZE_BACKEND
    if __SEABREEZE_BACKEND == 'cseabreeze':
        b = _use_cseabreeze()
        if b: return b
        b = _use_pyseabreeze()
        if b: return b
    if __SEABREEZE_BACKEND == 'pyseabreeze':
        b = _use_pyseabreeze()
        if b: return b
        b = _use_cseabreeze()
        if b: return b
    raise ImportError('Could not import any backends. Wanted "%s"' % __SEABREEZE_BACKEND)

def _use_cseabreeze():
    try:
        import seabreeze.cseabreeze as sbb
    except ImportError:
        warnings.warn("Can't load seabreeze c library wrapper. Falling back to pyseabreeze")
        return None
    else:
        sbb.initialize()
        return sbb

def _use_pyseabreeze():
    try:
        import seabreeze.pyseabreeze as sbb
    except ImportError as e:
        print e.message
        warnings.warn("Can't load pyusb wrapper. Falling back to cseabreeze")
        return None
    else:
        sbb.initialize()
        return sbb


