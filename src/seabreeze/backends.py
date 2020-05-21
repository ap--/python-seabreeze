import logging
import warnings

__all__ = ["use", "get_backend"]

_log = logging.getLogger(__name__)


def _use_backend(name):
    # internal: import the libseabreeze cython wrapper -> cseabreeze
    try:
        if name == "cseabreeze":
            import seabreeze.cseabreeze as sbb
        elif name == "pyseabreeze":
            import seabreeze.pyseabreeze as sbb
        else:
            raise ValueError("unknown backend '{}'".format(name))
    except ImportError as err:
        _log.warn(
            "seabreeze can't load '{}' backend - error: '{}'".format(name, repr(err)),
            exc_info=True,
        )
        return None
    else:
        return sbb


# internal config dict used exclusively in this submodule
_SeaBreezeConfig = {
    "requested_backend": "cseabreeze",  # default is cseabreeze
    "available_backends": ("cseabreeze", "pyseabreeze"),
    "allow_fallback": False,
}


def use(backend, force=True):
    """
    select the backend used for communicating with the spectrometer

    Parameters
    ----------
    backend : str

    force : bool, optional, default: True
        raises an ImportError when ``seabreeze.get_backend()`` is called
        and the requested backend can not be imported. force=True should
        be used in user code to ensure that the correct backend is being
        loaded.
    """
    if backend not in _SeaBreezeConfig["available_backends"]:
        raise ValueError(
            "backend not in: {}".format(
                ", ".join(_SeaBreezeConfig["available_backends"])
            )
        )
    _SeaBreezeConfig["requested_backend"] = backend
    _SeaBreezeConfig["allow_fallback"] = not force


def get_backend():
    """
    return the requested backend or a fallback. configuration is done
    via ``seabreeze.use()``

    Returns
    -------
    backend:
        a backend interface for communicating with the spectrometers
    """
    requested = _SeaBreezeConfig["requested_backend"]
    fallback = _SeaBreezeConfig["allow_fallback"]
    backends = _SeaBreezeConfig["available_backends"]

    backend = _use_backend(requested)  # trying to import requested backend
    if backend is None and fallback:

        warnings.warn(
            "seabreeze failed to load user requested '{}' backend but will try fallback backends".format(
                requested
            ),
            category=ImportWarning,
        )

        fallback_backends = [b for b in backends if b != requested]
        for fallback_backend in fallback_backends:
            backend = _use_backend(fallback_backend)
            if backend is not None:
                break
        else:
            raise ImportError(
                "Could not import backend. Tried: {}".format(", ".join(backends))
            )

    if backend is None:
        raise ImportError("Could not import backend. Requested: {}".format(requested))

    return backend
