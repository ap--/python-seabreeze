import logging
import warnings

__all__ = ['use', 'get_backend']


_log = logging.getLogger(__name__)


def _use_cseabreeze():
    # internal: import the libseabreeze cython wrapper -> cseabreeze
    try:
        import seabreeze.cseabreeze as sbb
    except ImportError as err:
        _log.warn("seabreeze can't load 'cseabreeze' backend - error: '%s'" % str(err), exc_info=True)
        return None
    else:
        sbb.initialize()
        return sbb


def _use_pyseabreeze():
    # internal: import the pyusb wrapper -> pyseabreeze
    try:
        import seabreeze.pyseabreeze as sbb
    except ImportError as err:
        _log.warn("seabreeze can't load 'pyseabreeze' backend - error: '%s'" % str(err), exc_info=True)
        return None
    else:
        sbb.initialize()
        return sbb


# internal config dict used exclusively in this submodule
_SeaBreezeConfig = {
    'requested_backend': 'cseabreeze',  # default is cseabreeze
    'available_backends': {
        'cseabreeze': _use_cseabreeze,
        'pyseabreeze': _use_pyseabreeze
    },
    'allow_fallback': True  # WARNING: WILL CHANGE TO FALSE IN FUTURE VERSIONS
}


def use(backend, force=None):
    """
    select the backend used for communicating with the spectrometer

    Parameters
    ----------
    backend : str

    force : bool, optional, default: False
        raises an ImportError when ``seabreeze.get_backend()`` is called
        and the requested backend can not be imported. force=True should
        be used in user code to ensure that the correct backend is being
        loaded.
    """
    if force is None:
        warnings.warn("use() will default to force=True in future versions", FutureWarning)
        force = False
    if backend not in _SeaBreezeConfig['available_backends']:
        raise ValueError('backend not in: {}'.format(', '.join(_SeaBreezeConfig['available_backends'])))
    _SeaBreezeConfig['requested_backend'] = backend
    _SeaBreezeConfig['allow_fallback'] = not force


def get_backend():
    """
    return the requested backend or a fallback. configuration is done
    via ``seabreeze.use()``

    Returns
    -------
    backend:
        a backend interface for communicating with the spectrometers
    """
    requested = _SeaBreezeConfig['requested_backend']
    fallback = _SeaBreezeConfig['allow_fallback']
    backends = _SeaBreezeConfig['available_backends']

    import_func = backends.get(requested, lambda: None)
    backend = import_func()  # trying to import requested backend
    if backend is None and fallback:

        warnings.warn("seabreeze failed to load user requested '%s' backend but will try fallback backends" % requested,
                      category=ImportWarning)

        fallback_backends = [b for b in backends.keys() if b != requested]
        for fallback_backend in fallback_backends:
            import_func = backends.get(fallback_backend)
            backend = import_func()
            if backend is not None:
                break
        else:
            raise ImportError('Failed to import backend. '
                              'Tried: {}'.format(requested, ', '.join(fallback_backends)))

    if backend is None:
        raise ImportError('Could not import any backends. Wanted "%s"' % requested)

    return backend
