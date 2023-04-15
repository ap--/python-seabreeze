"""pretty print pcapng files"""
from __future__ import annotations

import enum
import struct
from typing import Any
from typing import Iterable

try:
    from pcapng import FileScanner
    from pcapng.blocks import EnhancedPacket
    from pcapng.blocks import InterfaceDescription
    from pcapng.blocks import SectionHeader
except ImportError as err:
    print("missing dependency: pip install python-pcapng")
    print(err)
    raise SystemExit(1)

try:
    from seabreeze.pyseabreeze.exceptions import SeaBreezeError
    from seabreeze.pyseabreeze.protocol import OBPProtocol
except ImportError as err:
    print("missing dependency: pip install seabreeze[pyseabreeze]")
    print(err)
    raise SystemExit(1)


class URBFunction(enum.IntEnum):
    SELECT_CONFIGURATION = 0x0000
    SELECT_INTERFACE = 0x0001
    ABORT_PIPE = 0x0002
    TAKE_FRAME_LENGTH_CONTROL = 0x0003
    RELEASE_FRAME_LENGTH_CONTROL = 0x0004
    GET_FRAME_LENGTH = 0x0005
    SET_FRAME_LENGTH = 0x0006
    GET_CURRENT_FRAME_NUMBER = 0x0007
    CONTROL_TRANSFER = 0x0008
    BULK_OR_INTERRUPT_TRANSFER = 0x0009
    ISOCH_TRANSFER = 0x000A
    GET_DESCRIPTOR_FROM_DEVICE = 0x000B
    SET_DESCRIPTOR_TO_DEVICE = 0x000C
    SET_FEATURE_TO_DEVICE = 0x000D
    SET_FEATURE_TO_INTERFACE = 0x000E
    SET_FEATURE_TO_ENDPOINT = 0x000F
    CLEAR_FEATURE_TO_DEVICE = 0x0010
    CLEAR_FEATURE_TO_INTERFACE = 0x0011
    CLEAR_FEATURE_TO_ENDPOINT = 0x0012
    GET_STATUS_FROM_DEVICE = 0x0013
    GET_STATUS_FROM_INTERFACE = 0x0014
    GET_STATUS_FROM_ENDPOINT = 0x0015
    RESERVED_0X0016 = 0x0016
    VENDOR_DEVICE = 0x0017
    VENDOR_INTERFACE = 0x0018
    VENDOR_ENDPOINT = 0x0019
    CLASS_DEVICE = 0x001A
    CLASS_INTERFACE = 0x001B
    CLASS_ENDPOINT = 0x001C
    RESERVE_0X001D = 0x001D
    SYNC_RESET_PIPE_AND_CLEAR_STALL = 0x001E
    CLASS_OTHER = 0x001F
    VENDOR_OTHER = 0x0020
    GET_STATUS_FROM_OTHER = 0x0021
    CLEAR_FEATURE_TO_OTHER = 0x0022
    SET_FEATURE_TO_OTHER = 0x0023
    GET_DESCRIPTOR_FROM_ENDPOINT = 0x0024
    SET_DESCRIPTOR_TO_ENDPOINT = 0x0025
    GET_CONFIGURATION = 0x0026
    GET_INTERFACE = 0x0027
    GET_DESCRIPTOR_FROM_INTERFACE = 0x0028
    SET_DESCRIPTOR_TO_INTERFACE = 0x0029
    GET_MS_FEATURE_DESCRIPTOR = 0x002A
    RESERVE_0X002B = 0x002B
    RESERVE_0X002C = 0x002C
    RESERVE_0X002D = 0x002D
    RESERVE_0X002E = 0x002E
    RESERVE_0X002F = 0x002F
    SYNC_RESET_PIPE = 0x0030
    SYNC_CLEAR_STALL = 0x0031
    CONTROL_TRANSFER_EX = 0x0032
    RESERVE_0X0033 = 0x0033
    RESERVE_0X0034 = 0x0034


class InfoDirection(enum.IntEnum):
    FDO_TO_PDO = 0x00
    PDO_TO_FDO = 0x01


class EndpointDirection(enum.Enum):
    IN = enum.auto()
    OUT = enum.auto()


class USBPacket:
    def __init__(
        self,
        irp_id: int,
        usbd_status: int,
        urb_function: int,
        irp_info_direction: int,
        urb_bus_id: int,
        device_address: int,
        endpoint_number: int,
        transfer_type: int,
        recipient: int | None = None,
        data: bytes = b"",
    ) -> None:
        self.irp_id = int(irp_id)
        self.usbd_status = int(usbd_status)
        self.usb_function: URBFunction = URBFunction(urb_function)
        self.irp_info_direction: InfoDirection = InfoDirection(irp_info_direction)
        self.urb_bus_id: int = int(urb_bus_id)
        self.device_address: int = int(device_address)
        self.endpoint_number: int = int(endpoint_number)
        self.transfer_type: TransferType = TransferType(transfer_type)
        self.recipient = recipient
        self.data = data

    @property
    def endpoint_direction(self) -> EndpointDirection:
        if 0x80 & self.endpoint_number:
            return EndpointDirection.IN
        else:
            return EndpointDirection.OUT


class TransferType(enum.IntEnum):
    URB_CONTROL = 2
    URB_BULK = 3


def parse_usb_packet(pkt: Any) -> USBPacket:
    (
        pcap_header_len,
        irp_id,
        usbd_status,
        urb_function,
        irp_info_direction,
        urb_bus_id,
        device_address,
        endpoint_number,
        transfer_type,
        packet_data_len,
    ) = struct.unpack("<HQIHBHHBBI", pkt[:27])
    if pcap_header_len == 27 and transfer_type == 3:
        assert len(pkt) == 27 + packet_data_len
        return USBPacket(
            irp_id=irp_id,
            usbd_status=usbd_status,
            urb_function=urb_function,
            irp_info_direction=irp_info_direction,
            urb_bus_id=urb_bus_id,
            device_address=device_address,
            endpoint_number=endpoint_number,
            transfer_type=transfer_type,
            data=pkt[27:],
        )
    elif pcap_header_len == 28 and transfer_type == 2:
        recipient = int(pkt[27])
        assert len(pkt) == 28 + packet_data_len
        return USBPacket(
            irp_id=irp_id,
            usbd_status=usbd_status,
            urb_function=urb_function,
            irp_info_direction=irp_info_direction,
            urb_bus_id=urb_bus_id,
            device_address=device_address,
            endpoint_number=endpoint_number,
            transfer_type=transfer_type,
            recipient=recipient,
            data=pkt[28:],
        )
    else:
        raise NotImplementedError("yet")


def _abbreviate(x: str) -> str:
    if len(x) > 79:
        return x[:37] + " ... " + x[-37:]
    else:
        return x


class RawMessage:
    def __init__(
        self,
        endpoint: int,
        direction: EndpointDirection,
        data: bytes,
    ):
        self.endpoint = endpoint
        self.direction = direction
        self.data = data

    def __repr__(self) -> str:
        return f"RawMessage(0x{self.endpoint:02x}, {str(self.direction).ljust(21)}, data={_abbreviate(self.data.hex())})"


def iterate_raw_messages(packets: Iterable[USBPacket]) -> Iterable[RawMessage]:
    def _is_handshake_packet(x: USBPacket) -> bool:
        return (
            x.endpoint_direction,
            x.irp_info_direction,
        ) in {
            (EndpointDirection.IN, InfoDirection.FDO_TO_PDO),
            (EndpointDirection.OUT, InfoDirection.PDO_TO_FDO),
        }

    it = iter(packets)
    # skip all non BULK transfers
    idx = -1
    while True:
        idx += 1
        p = next(it)
        if p is None:
            continue
        if _is_handshake_packet(p):
            continue
        elif p.transfer_type != TransferType.URB_BULK:
            continue
        else:
            print(idx)
            break

    # group messages
    grp = [p]
    while True:
        idx += 1
        try:
            p = next(it)
        except StopIteration:
            break
        if p is None:
            continue
        if _is_handshake_packet(p):
            continue

        if p.transfer_type == TransferType.URB_BULK:
            if not grp or p.endpoint_direction == grp[-1].endpoint_direction:
                grp.append(p)
            else:
                yield RawMessage(
                    grp[-1].endpoint_number,
                    grp[-1].endpoint_direction,
                    b"".join([g.data for g in grp]),
                )
                grp[:] = [p]
        elif p.transfer_type == TransferType.URB_CONTROL:
            if grp:
                yield RawMessage(
                    grp[-1].endpoint_number,
                    grp[-1].endpoint_direction,
                    b"".join([g.data for g in grp]),
                )
                grp[:] = []
            else:
                continue

        else:
            raise RuntimeError
    if grp:
        yield RawMessage(
            grp[0].endpoint_number,
            grp[0].endpoint_direction,
            b"".join([g.data for g in grp]),
        )


def main(pth: str, *, debug: bool = False) -> None:
    with open(pth, "rb") as f:
        scanner = FileScanner(f)
        it = iter(scanner)

        section_header: SectionHeader = next(it)
        if_description: InterfaceDescription = next(it)
        assert isinstance(section_header, SectionHeader)
        assert isinstance(if_description, InterfaceDescription)

        packets = []
        for idx, block in enumerate(it, start=2):
            assert isinstance(block, EnhancedPacket)

            packet = parse_usb_packet(block.packet_data)
            packets.append(packet)
            if debug and packet.endpoint_number == 1:
                print(
                    f"{packet.endpoint_number:02x}",
                    str(packet.endpoint_direction).ljust(21),
                    packet.irp_info_direction,
                    packet.data.hex(),
                )

        for msg in iterate_raw_messages(packets):
            # print(msg)
            try:
                data = OBPProtocol._check_incoming_message_header(
                    OBPProtocol, msg.data[:44], _raw=True
                )
            except SeaBreezeError as err:
                print("ERROR", *err.args)
                continue
            try:
                _ = OBPProtocol._check_incoming_message_footer(
                    OBPProtocol, msg.data[-20:]
                )
            except (AssertionError, SeaBreezeError):
                print("ERROR", data)
                continue
            payload = OBPProtocol._extract_message_data(OBPProtocol, msg.data)

            if msg.endpoint == 0x01:
                if payload:
                    print("OBP PC->DEV", f"{data[4]:08x}", len(payload), payload)
                else:
                    print("OBP PC->DEV", f"{data[4]:08x}")
            elif msg.endpoint == 0x81:
                if len(payload) <= 32:
                    print("OBP DEV->PC", f"{data[4]:08x}", len(payload), payload.hex())
                else:
                    print(
                        "OBP DEV->PC",
                        f"{data[4]:08x}",
                        len(payload),
                        f"{payload[:32].hex()}...",
                        f"...{payload[-12:].hex()}",
                    )
            else:
                print(f"WARNING not displaying endpoint: 0x{msg.endpoint:02x}")


if __name__ == "__main__":
    import sys

    try:
        _, pth = sys.argv
    except ValueError:
        print(f"call via: `python {sys.argv[0]} <pcapng filename>`")
        raise SystemExit(1)

    main(pth)
