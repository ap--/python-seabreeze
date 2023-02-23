from __future__ import annotations

import weakref
from abc import ABC
from abc import abstractmethod
from typing import Any
from typing import Generic
from typing import Iterable
from typing import TypeVar


class PySeaBreezeProtocol(ABC):
    def __init__(
        self,
        transport: PySeaBreezeTransport[Any],
    ) -> None:
        if not isinstance(transport, PySeaBreezeTransport):
            raise TypeError("transport does not implement read and write methods")
        self.transport: PySeaBreezeTransport[Any] = weakref.proxy(transport)

    @abstractmethod
    def send(
        self,
        msg_type: int,
        payload: tuple[int | str | float, ...] | str | int | float = (),
        timeout_ms: int | None = None,
        **kwargs: Any,
    ) -> int:
        ...

    @abstractmethod
    def receive(
        self,
        size: int | None = None,
        timeout_ms: int | None = None,
        **kwargs: Any,
    ) -> bytes:
        ...

    @abstractmethod
    def query(
        self,
        msg_type: int,
        payload: tuple[int | str | float, ...] | str | int | float = (),
        timeout_ms: int | None = None,
        **kwargs: Any,
    ) -> bytes:
        ...


DT = TypeVar("DT")


class PySeaBreezeTransport(ABC, Generic[DT]):
    _required_init_kwargs: tuple[str, ...] = ()

    @abstractmethod
    def open_device(self, device: DT) -> None:
        """open a seabreeze device

        Parameters
        ----------
        device : seabreeze.pyseabreeze.devices.SeaBreezeDevice
        """
        ...

    @property
    @abstractmethod
    def is_open(self) -> bool:
        """return if device is opened

        Returns
        -------
        bool
        """
        return False

    @abstractmethod
    def close_device(self) -> None:
        """close the seabreeze device"""
        ...

    @abstractmethod
    def write(self, data: bytes, timeout_ms: int | None = None, **kwargs: Any) -> int:
        """write data to the device"""
        ...

    @abstractmethod
    def read(
        self,
        size: int | None = None,
        timeout_ms: int | None = None,
        **kwargs: Any,
    ) -> bytes:
        """read data from the

        Returns
        -------
        bytes
        """
        ...

    @property
    @abstractmethod
    def default_timeout_ms(self) -> int:
        ...

    @property
    @abstractmethod
    def protocol(self) -> PySeaBreezeProtocol:
        ...

    @classmethod
    @abstractmethod
    def list_devices(cls) -> Iterable[DT]:
        ...

    @classmethod
    @abstractmethod
    def register_model(cls, model_name: str, **kwargs: str) -> None:
        ...

    @classmethod
    @abstractmethod
    def supported_model(cls, device: DT) -> str | None:
        """return if the device supports the transport or vice versa

        Returns
        -------
        model_name : ``
        """
        return None

    @classmethod
    @abstractmethod
    def specialize(
        cls, model_name: str, **kwargs: str
    ) -> type[PySeaBreezeTransport[Any]]:
        ...
