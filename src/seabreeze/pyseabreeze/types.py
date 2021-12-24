from __future__ import annotations
import sys
from typing import Optional
from typing import Union

if sys.version_info >= (3, 8):
    from typing import Protocol
else:
    from typing_extensions import Protocol


class PySeaBreezeProtocol(Protocol):
    def send(
        self,
        msg_type: int,
        payload: Union[tuple, str, int],
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> None:
        ...

    def receive(
        self,
        size: Optional[int] = None,
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> bytes:
        ...

    def query(
        self,
        msg_type: int,
        payload: Union[tuple, str, int],
        timeout_ms: Optional[int] = None,
        **kwargs: str,
    ) -> bytes:
        ...
