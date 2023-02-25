"""modify some behavior of pyseabreeze"""
from __future__ import annotations

import os
from contextlib import contextmanager
from typing import Callable
from typing import Iterator
from typing import TypeVar

__all__ = [
    "config",
]


def _to_bool(val: str) -> bool:
    if val in {"1", "true", "True", "TRUE", "T"}:
        return True
    elif val in {"0", "false", "False", "FALSE", "F"}:
        return False
    else:
        raise ValueError(f"can't convert to bool: {val!r}")


T = TypeVar("T")


class _Config:
    """holds configuration for pyseabreeze

    Usage
    -----
    use config.set to set globally
    >>> from seabreeze.pyseabreeze.config import config
    >>>
    >>> config.set(obp_protocol_checks=False)
    use in a context
    >>> with config.context(obp_protocol_checks=False):
    >>>     ...  # do something
    Or set via export SEABREEZE_OPB_PROTOCOL_CHECKS=False

    """

    obp_protocol_checks: bool

    def __init__(
        self,
        obp_protocol_checks: bool = True,
    ) -> None:
        self.set(
            obp_protocol_checks=self._getenv(
                "obp_protocol_checks", _to_bool, obp_protocol_checks
            ),
        )

    def set(
        self,
        obp_protocol_checks: bool | None = None,
    ) -> None:
        """set a configuration variable"""
        if obp_protocol_checks is not None:
            if not isinstance(obp_protocol_checks, bool):
                raise TypeError
            self.obp_protocol_checks = obp_protocol_checks

    @contextmanager
    def context(
        self,
        obp_protocol_checks: bool | None = None,
    ) -> Iterator[None]:
        state = vars(self).copy()
        try:
            self.set(obp_protocol_checks=obp_protocol_checks)
            yield
        finally:
            self.__dict__.clear()
            self.__dict__.update(state)

    def _getenv(self, varname: str, type_: Callable[[str], T], default: T) -> T:
        val = os.environ.get(f"SEABREEZE_{varname.upper()}")
        if val is None:
            return default
        else:
            return type_(val)


config = _Config()
"""global config instance"""
