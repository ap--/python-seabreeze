import struct
from unittest import mock

import pytest

from seabreeze.pyseabreeze.types import PySeaBreezeTransport


@pytest.fixture
def obp_message(pyseabreeze):
    """create a valid OBP message"""
    from seabreeze.pyseabreeze.protocol import OBPProtocol

    _o = OBPProtocol.OBP
    yield struct.pack(
        _o.HEADER_FMT + "0s" + _o.FOOTER_FMT,
        _o.HEADER_START_BYTES,
        _o.HEADER_PROTOCOL_VERSION,
        0,
        _o.NO_ERROR,
        0x00000100,
        0,
        _o.RESERVED,
        _o.CHECKSUM_TYPE_NONE,
        1,
        b"c",
        20,
        b"",
        _o.NO_CHECKSUM,
        _o.FOOTER,
    )


@pytest.fixture
def mock_transport(obp_message):
    mock_transport = mock.Mock(spec=PySeaBreezeTransport)
    mock_transport.read.return_value = obp_message
    yield mock_transport


def test_pyseabreeze_protocol_messages(mock_transport):
    """test the parsing code for the OBP protocol implementation of pyseabreeze"""
    from seabreeze.pyseabreeze.protocol import OBPProtocol

    obp = OBPProtocol(mock_transport)
    obp.send(0x00000100)
