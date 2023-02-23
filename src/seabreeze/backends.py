import logging
import sys
import warnings
from typing import Any
from typing import Dict
from typing import Optional
from typing import cast

if sys.version_info >= (3, 8):
    from typing import Literal
else:
    from typing_extensions import Literal

from seabreeze.types import SeaBreezeBackend

__all__ = ["use", "get_backend"]


class BackendConfig:
    """*internal* config dict"""

    requested = "cseabreeze"  # default is cseabreeze
    available = ("cseabreeze", "pyseabreeze")
    allow_fallback = False
    api_kwargs: Dict[str, Any] = {}  # for pytests


def use(
    backend: Literal["cseabreeze", "pyseabreeze"], force: bool = True, **kwargs: Any
) -> None:
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

    pyseabreeze only
    ----------------
    pyusb_backend: str
        either libusb1, libusb0 or openusb

    """
    if backend not in BackendConfig.available:
        raise ValueError(
            "backend not in: {}".format(", ".join(BackendConfig.available))
        )
    if "seabreeze.spectrometers" in sys.modules:
        warnings.warn(
            "seabreeze.use has to be called before importing seabreeze.spectrometers",
            stacklevel=2,
        )

    BackendConfig.requested = backend
    BackendConfig.allow_fallback = not force
    BackendConfig.api_kwargs = {}

    if backend == "pyseabreeze":
        if "pyusb_backend" in kwargs:
            pyusb_backend = kwargs.pop("pyusb_backend")
            BackendConfig.api_kwargs["pyusb_backend"] = pyusb_backend
    if kwargs:
        raise TypeError(
            f"unknown keyword arguments {set(kwargs)!r} for backend {backend!r}"
        )


def get_backend() -> SeaBreezeBackend:
    """
    return the requested backend or a fallback. configuration is done
    via ``seabreeze.use()``

    Returns
    -------
    backend:
        a backend interface for communicating with the spectrometers
    """

    def _use_backend(name: str) -> Optional[SeaBreezeBackend]:
        # internal: import the libseabreeze cython wrapper -> cseabreeze
        try:
            if name == "cseabreeze":
                import seabreeze.cseabreeze as sbb
            elif name == "pyseabreeze":
                import seabreeze.pyseabreeze as sbb  # type: ignore
            else:
                raise ValueError(f"unknown backend {name!r}")
        except ImportError as err:
            logging.getLogger(__name__).warning(
                f"seabreeze can't load {name!r} backend - error: '{err!r}'",
                exc_info=True,
            )
            return None
        else:
            return cast(SeaBreezeBackend, sbb)

    requested = BackendConfig.requested
    fallback = BackendConfig.allow_fallback
    backends = BackendConfig.available

    backend = _use_backend(requested)  # trying to import requested backend
    if backend is None and fallback:
        warnings.warn(
            f"seabreeze failed to load user requested {requested!r} backend but will try fallback backends",
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
        raise ImportError(f"Could not import backend. Requested: {requested}")

    # provide for testing purposes
    backend._api_kwargs = BackendConfig.api_kwargs

    return backend
