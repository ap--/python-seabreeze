import struct

import mock
import pytest

try:
    import seabreeze.pyseabreeze
except ImportError:
    pytestmark = pytest.mark.skip("Can't import pyseabreeze")
    OBP_VALID_MSG = None
else:
    from seabreeze.pyseabreeze.protocol import OBPProtocol

    _o = OBPProtocol.OBP
    OBP_VALID_MSG = struct.pack(
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
def mock_transport():
    mock_transport = mock.Mock()
    mock_transport.read.return_value = OBP_VALID_MSG
    yield mock_transport


def test_obp_protocol_messages(mock_transport):
    obp = OBPProtocol(mock_transport)
    obp.send(0x00000100)
