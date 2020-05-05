"""Implementation of the Protocol layer

ocean optics spectrometers use two different communication protocols:
- one is called 'OBP protocol'  # OceanBinaryProtocol
- the other one 'OOI protocol'  # ??? OceanOpticsInterface ??? maybe

"""
import collections
import functools
import hashlib
import struct
import time
import warnings

from seabreeze.pyseabreeze.exceptions import SeaBreezeError


class ProtocolInterface(object):

    def __init__(self, transport):
        if not (hasattr(transport, 'write')
                and isinstance(transport.write, collections.Callable)
                and hasattr(transport, 'read')
                and isinstance(transport.read, collections.Callable)):
            raise TypeError("transport does not implement read and write methods")
        self.transport = transport

    def send(self, msg_type, payload, timeout_ms=None, **kwargs):
        raise NotImplementedError("implement in derived classes")

    def receive(self, size=None, timeout_ms=None, **kwargs):
        raise NotImplementedError("implement in derived classes")

    def query(self, msg_type, payload, timeout_ms=None, **kwargs):
        raise NotImplementedError("implement in derived classes")


class OOIProtocol(ProtocolInterface):

    msgs = {code: functools.partial(struct.Struct(msg).pack, code) for code, msg in {
        0x01: '<B',    # OP_INITIALIZE
        0x02: '<BI',   # OP_ITIME
        0x03: '<BH',   # set Strobe/Lamp enable Line
        0x05: '<BB',   # OP_GETINFO
        0x09: '<B',    # OP_REQUESTSPEC
        0x0A: '<BH',   # OP_SETTRIGMODE
        0x71: '<BBB',  # OP_TECENABLE_QE
        0x72: '<B',    # OP_READTEC_QE
        0x73: '<Bh',   # OP_TECSETTEMP_QE
        0xFE: '<B',    # OP_USBMODE
    }.items()}  # add more here if you implement new features

    def __init__(self, transport):
        super(OOIProtocol, self).__init__(transport)
        # initialize the spectrometer
        self.send(0x01)
        time.sleep(0.1)  # wait shortly after init command

    def send(self, msg_type, payload=(), timeout_ms=None, **kwargs):
        """send a ooi message to the spectrometer

        Parameters
        ----------
        msg_type : int
            a message type as defined in `OOIProtocol.msgs`
        payload : optional
            dependent on `msg_type`. a singe value or a tuple of multiple values
        timeout_ms : int, optional
            the timeout after which the transport layer should error.
            `None` means no timeout (default)
        **kwargs :
            ignored and only present to provide compatible caller interfaces

        Returns
        -------
        bytes_written : int
            the number of bytes sent
        """
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        payload = payload if isinstance(payload, (tuple, list)) else (payload,)
        data = self.msgs[msg_type](*payload)
        return self.transport.write(data, timeout_ms=timeout_ms)

    def receive(self, size=None, timeout_ms=None, mode=None, **kwargs):
        """receive data from the spectrometer

        Parameters
        ----------
        size : int, optional
            number of bytes to receive. if `None` (default) uses the
            default size as specified in the transport layer.
        timeout_ms : int, optional
            the timeout after which the transport layer should error.
            `None` means no timeout (default)
        mode : str, optional
            transport layers can support different modes
            (i.e. {'low_speed', 'high_speed', 'high_speed_alt'} in the usb case)
        kwargs :
            ignored and only present to provide compatible caller interfaces

        Returns
        -------
        data : str
            data returned from the spectrometer
        """
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        return self.transport.read(size=size, timeout_ms=timeout_ms, mode=mode, **kwargs)

    def query(self, msg_type, payload, size=None, timeout_ms=None, mode=None, **kwargs):
        """convenience method combining send and receive

        Parameters
        ----------
        msg_type : int
            a message type as defined in `OOIProtocol.msgs`
        payload :
            the payload to be sent. Can be a singe value or a tuple, dependent
            `msg_type`.
        size : int, optional
            number of bytes to receive. if `None` (default) uses the
            default size as specified in the transport layer.
        timeout_ms : int, optional
            the timeout after which the transport layer should error.
            `None` means no timeout (default)
        mode : str, optional
            transport layers can support different modes
            (i.e. {'low_speed', 'high_speed', 'high_speed_alt'} in the usb case)
        kwargs :
            ignored and only present to provide compatible caller interfaces

        Returns
        -------
        data : str
            data returned from the spectrometer
        """
        self.send(msg_type, payload, timeout_ms=timeout_ms)
        return self.receive(size=size, timeout_ms=timeout_ms, **kwargs)


class OBPProtocol(ProtocolInterface):

    msgs = {code: struct.Struct(msg).pack for code, msg in {
        0x00000100: "",    # GET_SERIAL
        0x00100928: "",    # GET_BUF_SPEC32_META
        0x00101100: "",    # GET_RAW_SPECTRUM_NOW
        0x00110010: "<L",  # SET_ITIME_USEC
        0x00110110: "<B",  # SET_TRIG_MODE
        0x00180100: "",    # GET_WL_COEFF_COUNT
        0x00180101: "<B",  # GET_WL_COEFF
        0x00181100: "",    # GET_NL_COEFF_COUNT
        0x00181101: "<B",  # GET_NL_COEFF
        0x00420004: "",    # GET_TE_TEMPERATURE
        0x00420010: "<B",  # SET_TE_ENABLE
        0x00420011: "<f",  # SET_TE_SETPOINT
    }.items()}  # add more here if you implement new features

    class OBP(object):
        """All relevant constants are stored here"""

        HEADER_START_BYTES = 0xC0C1
        HEADER_PROTOCOL_VERSION = 0x1100  # XXX: seems to be the newest protocol version

        FLAG_RESPONSE_TO_REQUEST = 0x0001
        FLAG_ACK = 0x0002
        FLAG_REQUEST_ACK = 0x0004
        FLAG_NACK = 0x0008
        FLAG_HW_EXCEPTION = 0x0010
        FLAG_PROTOCOL_DEPRECATED = 0x0020

        ERROR_CODES = {
            0: 'Success (no detectable errors)',
            1: 'Invalid/unsupported protocol',
            2: 'Unknown message type',
            3: 'Bad checksum',
            4: 'Message too large',
            5: 'Payload length does not match message type',
            6: 'Payload data invalid',
            7: 'Device not ready for given message type',
            8: 'Unknown checksum type',
            9: 'Device reset unexpectedly',
            10: 'Too many buses (Commands have come from too many bus interfaces)',
            11: 'Out of memory. Failed to allocate enough space to complete request.',
            12: 'Command is valid, but desired information does not exist.',
            13: 'Int Device Error. May be unrecoverable.',
            100: 'Could not decrypt properly',
            101: 'Firmware layout invalid',
            102: 'Data packet was wrong size',
            103: 'hardware revision not compatible with firmware',
            104: 'Existing flash map not compatible with firmware',
            255: 'Operation/Response Deferred. Will take some time. Do not ACK or NACK yet.',
        }

        NO_ERROR = 0x0000
        RESERVED = b""
        CHECKSUM_TYPE_NONE = 0x00
        CHECKSUM_TYPE_MD5 = 0x01
        NO_CHECKSUM = b""
        FOOTER = 0xC2C3C4C5  # the datasheet specifies it in this order...

        HEADER_FMT = (
            "<H"   # start_bytes
            "H"    # protocol_version
            "H"    # flags
            "H"    # error number
            "L"    # message type
            "L"    # regarding
            "6s"   # reserved
            "B"    # checksum type
            "B"    # immediate length
            "16s"  # immediate data
            "L"    # bytes remaining
        )

        FOOTER_FMT = (
            "16s"  # checksum
            "L"    # footer
        )

    # noinspection DuplicatedCode
    def send(self, msg_type, payload=(), timeout_ms=None, request_ack=True, **kwargs):
        """send a obp message to the spectrometer

        Parameters
        ----------
        msg_type : int
            a message type as defined in `OBPProtocol.msgs`
        payload : optional
            dependent on `msg_type`. Can be a single value or a tuple of multiple values
        timeout_ms : int, optional
            the timeout after which the transport layer should error.
            `None` means no timeout (default)
        request_ack : bool, default `True`
            request an ack for the sent command from the spectrometer.
        **kwargs :
            ignored and only present to provide compatible caller interfaces

        Returns
        -------
        bytes_written : int
            the number of bytes sent
        """
        if kwargs:
            warnings.warn("kwargs provided but ignored: {}".format(kwargs))
        payload = payload if isinstance(payload, (tuple, list)) else (payload,)
        data = self.msgs[msg_type](*payload)

        # Constructing message and querying usb.
        message = self._construct_outgoing_message(msg_type, data, request_ack=request_ack)  # TODO
        bytes_written = self.transport.write(message)  # ? assert bytes_written == len(message)

        if not request_ack:
            return bytes_written

        response = self.transport.read(timeout_ms=timeout_ms)
        remaining_bytes, checksum_type = self._check_incoming_message_header(response[:44])
        # ? assert remaining_bytes == 20
        checksum = self._check_incoming_message_footer(response[-20:])

        if checksum_type == self.OBP.CHECKSUM_TYPE_MD5 and checksum != hashlib.md5(response[:-20]).digest():
            warnings.warn("WARNING OBP: The checksums differ, but we ignore this for now.")
        return bytes_written

    def receive(self, size=None, timeout_ms=None, **kwargs):
        """receive data from the spectrometer

        Parameters
        ----------
        size : int, optional
            number of bytes to receive. if `None` (default) uses the
            default size as specified in the transport layer.
        timeout_ms : int, optional
            the timeout after which the transport layer should error.
            `None` means no timeout (default)
        kwargs :
            ignored and only present to provide compatible caller interfaces

        Returns
        -------
        data : str
            data returned from the spectrometer
        """
        response = self.transport.read(size=64, timeout_ms=timeout_ms)
        remaining_bytes, checksum_type = self._check_incoming_message_header(response[:44])
        length_payload_footer = remaining_bytes

        # we already received some data
        remaining_bytes -= len(response[44:])
        if remaining_bytes > 0:
            response += self.transport.read(size=remaining_bytes, timeout_ms=timeout_ms)

        if length_payload_footer != len(response[44:]):
            raise SeaBreezeError("remaining packet length mismatch: {:d} != {:d}".format(
                remaining_bytes, len(response[44:])
            ))

        checksum = self._check_incoming_message_footer(response[-20:])
        if checksum_type == self.OBP.CHECKSUM_TYPE_MD5 and checksum != hashlib.md5(response[:-20]).digest():
            warnings.warn("WARNING OBP: The checksums differ, but we ignore this for now.")

        return self._extract_message_data(response)

    def query(self, msg_type, payload=(), size=None, timeout_ms=None, **kwargs):
        """convenience method combining send and receive

        Parameters
        ----------
        msg_type : int
            a message type as defined in `OBPProtocol.msgs`
        payload :
            the payload to be sent. Can be a single value or a tuple, dependent
            `msg_type`.
        size : int, optional
            number of bytes to receive. if `None` (default) uses the
            default size as specified in the transport layer.
        timeout_ms : int, optional
            the timeout after which the transport layer should error.
            `None` means no timeout (default)
        kwargs :
            ignored and only present to provide compatible caller interfaces

        Returns
        -------
        data : str
            data returned from the spectrometer
        """
        self.send(msg_type, payload, request_ack=False)
        return self.receive(timeout_ms=timeout_ms)

    def _construct_outgoing_message(self, msg_type, payload_string, request_ack=False, regarding=None):
        """construct an outgoing OBP message

        Parameters
        ----------
        msg_type : `int`
            the obp message type, a 4 byte integer
        payload_string : `bytes`
            a compiled payload_string
        request_ack : `bool`
            request an ack for the sent command from the spectrometer.
        regarding : `int`
            see ocean optics obp protocol documentation

        Returns
        -------
        message : `str`
            compiled message
        """
        if request_ack is True:
            flags = self.OBP.FLAG_REQUEST_ACK
        else:
            flags = 0

        if regarding is None:
            regarding = 0

        if len(payload_string) <= 16:
            payload_string_fmt = "0s"
            immediate_length = len(payload_string)
            immediate_data = payload_string
            payload_string = b""
            bytes_remaining = 20  # Checksum + footer
        else:
            payload_string_fmt = "%ds" % len(payload_string)
            immediate_length = 0
            immediate_data = b""
            bytes_remaining = 20 + len(payload_string)

        FMT = self.OBP.HEADER_FMT + payload_string_fmt + self.OBP.FOOTER_FMT

        msg = struct.pack(FMT,
                          self.OBP.HEADER_START_BYTES,
                          self.OBP.HEADER_PROTOCOL_VERSION,
                          flags,
                          self.OBP.NO_ERROR,
                          msg_type,
                          regarding,
                          self.OBP.RESERVED,
                          self.OBP.CHECKSUM_TYPE_NONE,
                          immediate_length,
                          immediate_data,
                          bytes_remaining,
                          payload_string,
                          self.OBP.NO_CHECKSUM,
                          self.OBP.FOOTER)
        return msg

    def _check_incoming_message_header(self, header):
        """check the incoming message header

        Parameters
        ----------
        header : `str`
            a obp header of length 44

        Returns
        -------
        bytes_and_checksum_type : tuple[`int`, `int`]
            bytes_remaining after the header (returns 20 for a 64 byte message)
            checksum_type only supports self.OBP.CHECKSUM_TYPE_MD5 for now
        """
        if len(header) != 44:
            raise SeaBreezeError("header has wrong length! len(header): %d" % len(header))

        data = struct.unpack(self.OBP.HEADER_FMT, header)

        if data[0] != self.OBP.HEADER_START_BYTES:
            raise SeaBreezeError('Header start_bytes wrong: "%d"' % data[0])
        if data[1] != self.OBP.HEADER_PROTOCOL_VERSION:
            raise SeaBreezeError('Header protocol version wrong: %d' % data[1])

        flags = data[2]
        if flags == 0:
            pass
        if flags & self.OBP.FLAG_RESPONSE_TO_REQUEST:
            pass  # TODO: propagate?
        if flags & self.OBP.FLAG_ACK:
            pass  # TODO: propagate?
        if flags & self.OBP.FLAG_REQUEST_ACK:
            pass  # TODO: only the host should be able to set this?
        if (flags & self.OBP.FLAG_NACK) or (flags & self.OBP.FLAG_HW_EXCEPTION):
            error = data[3]
            if error != 0:  # != SUCCESS
                raise SeaBreezeError(self.OBP.ERROR_CODES[error])
            else:
                pass  # TODO: should we do something here?
        if flags & self.OBP.FLAG_PROTOCOL_DEPRECATED:
            raise SeaBreezeError("Protocol deprecated?!?")

        # msg_type = data[4]
        # regarding = data[5]

        checksum_type = data[7]  # TODO: implement checksums.
        if checksum_type not in [self.OBP.CHECKSUM_TYPE_NONE, self.OBP.CHECKSUM_TYPE_MD5]:
            raise SeaBreezeError('the checksum type is unknown: "%d"' % checksum_type)

        # immediate_length = data[8]
        # immediate_data = data[9]
        bytes_remaining = data[10]

        return bytes_remaining, checksum_type

    def _check_incoming_message_footer(self, footer):
        """check the incoming message header

        Parameters
        ----------
        footer : `str`
            a obp footer of length 20

        Returns
        -------
        checksum: `str`
            the 16 byte checksum of the message
        """
        assert len(footer) == 20, "footer has wrong length! len(footer): %d" % len(footer)

        data = struct.unpack("<" + self.OBP.FOOTER_FMT, footer)

        checksum = data[0]
        assert data[1] == self.OBP.FOOTER, "the device returned a wrong footer: %d" % data[1]

        return checksum

    def _extract_message_data(self, msg):
        """extract the payload data from a obp message

        Parameters
        ----------
        msg : `str`
            a obp message

        Returns
        -------
        data : `str`
            the payload contained in the message
        """
        payload_length = len(msg) - 44 - 20  # - HeaderLength - FooterLength
        if not (payload_length >= 0):
            raise SeaBreezeError("Received message < 64 bytes: %d" % payload_length)
        payload_fmt = "%ds" % payload_length
        FMT = self.OBP.HEADER_FMT + payload_fmt + self.OBP.FOOTER_FMT

        data = struct.unpack(FMT, msg)

        msg_type = data[4]

        immediate_length = data[8]
        immediate_data = data[9]
        payload = data[11]

        if (immediate_length > 0) and len(payload) > 0:
            raise SeaBreezeError("Got immediate AND payload data? cmd: '%d'" % msg_type)
        elif immediate_length > 0:
            return immediate_data[:immediate_length]
        elif payload_length > 0:
            return payload
        else:
            return b""
