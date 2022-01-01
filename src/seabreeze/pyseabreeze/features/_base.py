from __future__ import annotations

import weakref
from functools import partialmethod
from typing import Any
from typing import TypeVar

from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.types import PySeaBreezeProtocol

FT = TypeVar("FT", bound="SeaBreezeFeature")


class SeaBreezeFeature:
    identifier = "base_feature"

    _required_kwargs: tuple[str, ...] = ()
    _required_features: tuple[str, ...] = ()
    _required_protocol_cls = PySeaBreezeProtocol

    def __init__(
        self, protocol: PySeaBreezeProtocol, feature_id: int, **kwargs: Any
    ) -> None:
        """SeaBreezeFeature base class

        Parameters
        ----------
        protocol : Type[seabreeze.pyseabreeze.protocol.ProtocolInterface}
        feature_id : int
        """
        if self.identifier == "base_feature":
            raise SeaBreezeError(
                "Don't instantiate SeaBreezeFeature directly. Use derived feature classes."
            )
        assert set(self._required_kwargs) == set(kwargs), "{} vs {}".format(
            str(set(self._required_kwargs)), str(set(kwargs))
        )
        # check protocol support
        if not isinstance(protocol, self._required_protocol_cls):
            raise SeaBreezeError("FeatureError: Protocol not supported by feature")
        self.protocol: PySeaBreezeProtocol = weakref.proxy(protocol)
        self.feature_id = feature_id

    def __repr__(self) -> str:
        return f"<{self.__class__.__name__} id={self.feature_id}>"

    @classmethod
    def get_feature_class_registry(cls) -> dict[str, type[SeaBreezeFeature]]:
        # noinspection PyUnresolvedReferences
        return {
            feature_class.identifier: feature_class
            for feature_class in SeaBreezeFeature.__subclasses__()
        }

    @classmethod
    def supports_protocol(cls, protocol: PySeaBreezeProtocol) -> bool:
        return isinstance(protocol, cls._required_protocol_cls)

    @classmethod
    def specialize(cls: type[FT], model_name: str, **kwargs: Any) -> type[FT]:
        assert set(kwargs) == set(cls._required_kwargs)
        specialized_class = type(
            f"{cls.__name__}{model_name}",
            (cls,),
            {"__init__": partialmethod(cls.__init__, **kwargs)},
        )
        return specialized_class
