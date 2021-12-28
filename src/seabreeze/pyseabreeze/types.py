from __future__ import annotations

import weakref
from abc import ABC
from abc import abstractmethod
from typing import Generic
from typing import List
from typing import Optional
from typing import Tuple
from typing import Type
from typing import TypeVar
from typing import Union

from seabreeze.types import SeaBreezeDevice


class PySeaBreezeProtocol(ABC):
    def __init__(
        self,
        transport: PySeaBreezeTransport,
    ) -> None:
        if not isinstance(transport, PySeaBreezeTransport):
            raise TypeError("transport does not implement read and write methods")
        self.transport: PySeaBreezeTransport = weakref.proxy(transport)

    @abstractmethod
    def send(
        self,
        msg_type: int,
        payload: Union[tuple[int | str], str, int],
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> None:
        ...

    @abstractmethod
    def receive(
        self,
        size: Optional[int] = None,
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> bytes:
        ...

    @abstractmethod
    def query(
        self,
        msg_type: int,
        payload: Union[tuple[int | str], str, int],
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> bytes:
        ...


DT = TypeVar("DT")


class PySeaBreezeTransport(ABC, Generic[DT]):

    _required_init_kwargs: Tuple[str, ...] = ()

    @abstractmethod
    def open_device(self, device: SeaBreezeDevice) -> None:
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
    def write(
        self, data: bytes, timeout_ms: Optional[int] = None, **kwargs: str
    ) -> None:
        """write data to the device"""
        ...

    @abstractmethod
    def read(
        self,
        size: Optional[int] = None,
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> bytes:
        """read data from the

        Returns
        -------
        str
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
    def list_devices(cls) -> List[DT]:
        ...

    @classmethod
    @abstractmethod
    def register_model(cls, model_name: str, **kwargs: str) -> None:
        ...

    @classmethod
    @abstractmethod
    def supported_model(cls, device: SeaBreezeDevice) -> Optional[str]:
        """return if the device supports the transport or vice versa

        Returns
        -------
        model_name : ``
        """
        return None

    @classmethod
    @abstractmethod
    def specialize(cls, model_name: str, **kwargs: str) -> Type[PySeaBreezeTransport]:
        ...
