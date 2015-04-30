"""The oceanoptics spectrometers use two different communication protocols.

One is called 'OBP protocol'  # OceanBinaryProtocol
the other one 'OOI protocol'  # ??? OceanOpticsInterface ??? maybe

The only spectrometers using the OBP protocol via USB are: STS, Ventana and QEPro

"""
#----------------------------------------------------------
import usb.core
import usb.util
import struct
import hashlib
import warnings

from .common import SeaBreezeError
#----------------------------------------------------------

class USBCommBase(object):

    _ENDPOINT_MAP = None
    _device = None
    usbtimeout_ms = 0.0
    _mode = 'lowspeed'

    def open_device(self, device):
        # device.reset()
        self.usbtimeout_ms = device.default_timeout
        self._device = device
        self.usb_read = self.usb_read_lowspeed
        try:  # unavailable on some systems/backends
            if device.is_kernel_driver_active(0):
                device.detach_kernel_driver(0)
        except NotImplementedError:
            pass
        try:
            device.set_configuration()
        except usb.USBError as usberr:
            if usberr.errno == 16:
                self._opened = True
            raise
        else:
            self._opened = True

    def is_open(self):
        try:
            return self._opened
        except AttributeError:
            return False

    def close_device(self):
        usb.util.dispose_resources(self._device)
        self._device.reset()
        self._device = None
        self.usbtimeout_ms = 0.0
        self._opened = False

    def usb_send(self, data):
        self._device.write(self._ENDPOINT_MAP.ep_out, data)

    def usb_read_lowspeed(self, size=64, timeout=None):
        return self._device.read(self._ENDPOINT_MAP.lowspeed_in, size, timeout=timeout)

    def usb_read_highspeed(self, size=512, timeout=None):
        return self._device.read(self._ENDPOINT_MAP.highspeed_in, size, timeout=timeout)

    def usb_read_highspeed_alt(self, size=512, timeout=None):
        return self._device.read(self._ENDPOINT_MAP.highspeed_in2, size, timeout=timeout)

    def set_default_read(self, mode):
        if mode == "highspeed":
            self.usb_read = self.usb_read_highspeed
        else:  # mode == "lowspeed"
            self.usb_read = self.usb_read_lowspeed



class USBCommOOI(USBCommBase):

    def send_command(self, msgtype, payload):
        data = msgtype % payload
        return self.usb_send(data)

    def innitiate_query(self, msgtype, payload):
        data = msgtype % payload
        return self.usb_send(data)

    def receive_query(self, alternate_endpoint=False):
        if not alternate_endpoint:
            return self.usb_read()
        else:
            return self.usb_read_highspeed_alt()

    def query(self, msgtype, payload):
        self.innitiate_query(msgtype, payload)
        return self.usb_read()



class USBCommOBP(USBCommBase):

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
        RESERVED = ""
        CHECKSUM_TYPE_NONE = 0x00
        CHECKSUM_TYPE_MD5 = 0x01
        NO_CHECKSUM = ""
        FOOTER = 0xC2C3C4C5  # the datasheet specifies it in this order...

        HEADER_FMT = ("<H"    # start_bytes
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

        FOOTER_FMT = ("16s"  # checksum
                      "L"    # footer
                     )


    def send_command(self, msgtype, payload, timeout=None):
        """OBP command function"""
        # Constructing message and querying usb.
        msg = self._construct_outgoing_message(msgtype, payload, requestACK=True)
        self.usb_send(msg)
        ret = self.usb_read(timeout=timeout)

        _, checksumtype = self._check_incoming_message_header(ret[:44])

        checksum = self._check_incoming_message_footer(ret[-20:])
        if ((checksumtype == self.OBP.CHECKSUM_TYPE_MD5) and
            (checksum != hashlib.md5(ret[:-20]).digest())):
            # TODO: raise Error
            warnings.warn("WARNING OBP: The checksums differ, but we ignore this for now.")
        return

    def innitiate_query(self, msgtype, payload):
        msg = self._construct_outgoing_message(msgtype, payload, requestACK=False)
        self.usb_send(msg)

    def receive_query(self, timeout=None):
        """receive_query needs to be called after innitiate_query"""
        ret = self.usb_read(size=64, timeout=timeout)

        remaining_bytes, checksumtype = self._check_incoming_message_header(ret[:44])
        length_payload_footer = remaining_bytes

        # we already received some data
        remaining_bytes -= len(ret[44:])
        if remaining_bytes > 0:
            ret += self.usb_read(size=remaining_bytes, timeout=timeout)

        if length_payload_footer != len(ret[44:]):
            raise SeaBreezeError("remaining packet length mismatch: %d != %d" %
                                 (remaining_bytes, len(ret[44:])))

        checksum = self._check_incoming_message_footer(ret[-20:])
        if ((checksumtype == self.OBP.CHECKSUM_TYPE_MD5) and
            (checksum != hashlib.md5(ret[:-20]).digest())):
            # TODO: raise Error
            warnings.warn("WARNING OBP: The checksums differ, but we ignore this for now.")
        data = self._extract_message_data(ret)
        return data


    def query(self, msgtype, payload, timeout=None):
        """OBP query function"""
        self.innitiate_query(msgtype, payload)
        return self.receive_query(timeout)


    def _construct_outgoing_message(self, msgtype, payload, requestACK=False, regarding=None):
        """message layout, see STS datasheet

        """
        if requestACK == True:
            flags = self.OBP.FLAG_REQUEST_ACK
        else:
            flags = 0

        if regarding is None:
            regarding = 0

        if len(payload) <= 16:
            payload_fmt = "0s"
            immediate_length = len(payload)
            immediate_data = payload
            payload = ""
            bytes_remaining = 20  # Checksum + footer
        else:
            payload_fmt = "%ds" % len(payload)
            immediate_length = 0
            immediate_data = ""
            bytes_remaining = 20 + len(payload)

        FMT = self.OBP.HEADER_FMT + payload_fmt + self.OBP.FOOTER_FMT

        msg = struct.pack(FMT,
                          self.OBP.HEADER_START_BYTES,
                          self.OBP.HEADER_PROTOCOL_VERSION,
                          flags,
                          self.OBP.NO_ERROR,
                          msgtype,
                          regarding,
                          self.OBP.RESERVED,
                          self.OBP.CHECKSUM_TYPE_NONE,
                          immediate_length,
                          immediate_data,
                          bytes_remaining,
                          payload,
                          self.OBP.NO_CHECKSUM,
                          self.OBP.FOOTER)
        return msg

    def _check_incoming_message_header(self, header):
        """message layout, see STS datasheet

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
                pass  # TODO: should we do simething here?
        if flags & self.OBP.FLAG_PROTOCOL_DEPRECATED:
            raise SeaBreezeError("Protocol deprecated?!?")

        # msgtype = data[4]
        # regarding = data[5]

        checksumtype = data[7]  # TODO: implement checksums.
        if checksumtype not in [self.OBP.CHECKSUM_TYPE_NONE, self.OBP.CHECKSUM_TYPE_MD5]:
            raise SeaBreezeError('the checksum type is unkown: "%d"' % checksumtype)

        # immediate_length = data[8]
        # immediate_data = data[9]
        bytes_remaining = data[10]

        return bytes_remaining, checksumtype

    def _check_incoming_message_footer(self, footer):
        """message layout, see STS datasheet

        """
        assert len(footer) == 20, "footer has wrong length! len(footer): %d" % len(footer)

        data = struct.unpack("<" + self.OBP.FOOTER_FMT, footer)

        checksum = data[0]
        assert data[1] == self.OBP.FOOTER, "the device returned a wrong footer: %d" % data[1]

        return checksum

    def _extract_message_data(self, msg):
        """message layout, see STS datasheet

        """
        payload_length = len(msg) - 44 - 20  # - HeaderLength - FooterLength
        if not (payload_length >= 0):
            raise SeaBreezeError("Received message < 64 bytes: %d" % payload_length)
        payload_fmt = "%ds" % payload_length
        FMT = self.OBP.HEADER_FMT + payload_fmt + self.OBP.FOOTER_FMT

        data = struct.unpack(FMT, msg)

        msgtype = data[4]

        immediate_length = data[8]
        immediate_data = data[9]
        payload = data[11]

        if (immediate_length > 0) and len(payload) > 0:
            raise SeaBreezeError("Got immediate AND payload data? cmd: '%d'" % msgtype)
        elif immediate_length > 0:
            return immediate_data[:immediate_length]
        elif payload_length > 0:
            return payload
        else:
            return ""
