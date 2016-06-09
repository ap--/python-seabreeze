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
        warnings.warn("Falling back to 'pyseabreeze'.")
        b = _use_pyseabreeze()
        if b: return b
    if __SEABREEZE_BACKEND == 'pyseabreeze':
        b = _use_pyseabreeze()
        if b: return b
        warnings.warn("Falling back to 'cseabreeze'.")
        b = _use_cseabreeze()
        if b: return b
    raise ImportError('Could not import any backends. Wanted "%s"' % __SEABREEZE_BACKEND)

def _use_cseabreeze():
    try:
        import seabreeze.cseabreeze as sbb
    except ImportError as err:
        warnings.warn("Can't load seabreeze c library wrapper, because:\n'%s'\n" % str(err) )
        return None
    else:
        sbb.initialize()
        return sbb

def _use_pyseabreeze():
    try:
        import seabreeze.pyseabreeze as sbb
    except ImportError as err:
        warnings.warn("Can't load pyusb wrapper, because:\n'%s'\n" % str(err) )
        return None
    else:
        sbb.initialize()
        return sbb


