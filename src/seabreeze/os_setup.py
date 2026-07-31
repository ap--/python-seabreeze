"""Operating-system specific setup helpers for seabreeze."""

from __future__ import annotations

import argparse
import ctypes
import logging
import os
import platform
import subprocess
import sys
import tempfile
import time
import zipfile
from collections.abc import Iterable
from pathlib import Path
from textwrap import dedent
from textwrap import indent
from urllib.error import HTTPError
from urllib.request import urlopen

_GITHUB_REPO_URL = (
    "https://raw.githubusercontent.com/ap--/python-seabreeze/master/os_support"
)
_UDEV_RULES_PATH = Path("/etc/udev/rules.d/10-oceanoptics.rules")
_DRIVERS_ZIP_FN = "windows-driver-files.zip"
_log = logging.getLogger(__name__)


def _diff_files(file1: Path, file2: Path) -> str:
    """Diff two files using the system `diff` command."""
    proc = subprocess.run(
        ["diff", str(file1), str(file2)],
        check=False,
        capture_output=True,
        text=True,
    )
    return proc.stdout


def _request_confirmation(question: str, assume_yes: bool = False) -> bool:
    """Require user input to continue, unless auto-confirmed."""
    if assume_yes:
        return True

    while True:
        user_input = input(f"{question} [y/n] ").lower()
        if user_input not in {"y", "n"}:
            _log.info("Please enter 'y' or 'n'.")
            time.sleep(0.1)
            continue
        return user_input == "y"


def _download_rules_file(destination: Path) -> None:
    url = f"{_GITHUB_REPO_URL}/{_UDEV_RULES_PATH.name}"
    try:
        _log.info("Downloading rules from GitHub")
        udev_data = urlopen(url).read()
    except HTTPError:
        raise RuntimeError(f"Can't download '{url}'") from None
    destination.write_bytes(udev_data)


def linux_install_udev_rules(
    rules: Path | None = None,
    overwrite_existing: bool = False,
    assume_yes: bool = False,
) -> int:
    """Verify and install Linux udev rules."""
    with tempfile.TemporaryDirectory(prefix="seabreeze-udev-") as tmp_dir:
        temp_rules = Path(tmp_dir) / _UDEV_RULES_PATH.name
        rules_path = rules

        if rules_path is not None:
            if not rules_path.exists():
                raise OSError(f"rules file '{rules_path}' doesn't exist")
        else:
            _download_rules_file(temp_rules)
            rules_path = temp_rules

        assert rules_path is not None

        if _UDEV_RULES_PATH.exists() and not overwrite_existing:
            rules_differ = _diff_files(_UDEV_RULES_PATH, rules_path)
            if not rules_differ:
                _log.info("udev rules already newest version")
                return 0
            _log.info(indent(rules_differ, "  ").rstrip())
            _log.info("udev rules differ. To overwrite run with '--overwrite-existing'")
            return 1

        if not _request_confirmation("Install udev rules?", assume_yes=assume_yes):
            return 0

        _log.info(f"Copying udev rules to {_UDEV_RULES_PATH}")
        copy_rc = subprocess.call(["sudo", "cp", str(rules_path), str(_UDEV_RULES_PATH)])
        _log.info("Calling udevadm control --reload-rules")
        reload_rc = subprocess.call(["sudo", "udevadm", "control", "--reload-rules"])

        if copy_rc == 0 and reload_rc == 0:
            _log.info("Success")
            return 0
        return copy_rc or reload_rc


def _windows_is_admin() -> bool:
    """Windows only: check if running as admin."""
    # noinspection PyBroadException
    try:
        return bool(ctypes.windll.shell32.IsUserAnAdmin())
    except Exception:
        return False


def _is_contained_in_dir(files: Iterable[str], cdir: str | Path) -> bool:
    """Return whether all files resolve under the provided directory."""
    base = Path(cdir).resolve()
    for file_name in files:
        file_path = Path(file_name)
        if file_path.is_absolute():
            return False
        resolved = (base / file_path).resolve()
        if not str(resolved).startswith(f"{base}{os.sep}") and resolved != base:
            return False
    return True


def _download_driver_zip(destination: Path) -> None:
    url = f"{_GITHUB_REPO_URL}/{_DRIVERS_ZIP_FN}"
    try:
        _log.info("Downloading windows drivers from GitHub")
        drivers_zip_data = urlopen(url).read()
    except HTTPError:
        raise RuntimeError(f"Can't download '{url}'") from None
    destination.write_bytes(drivers_zip_data)


def windows_install_drivers(
    drivers_zip: Path | None = None,
    assume_yes: bool = False,
) -> int:
    """Install driver inf files via pnputil in an elevated shell."""
    if not _request_confirmation("Install windows drivers?", assume_yes=assume_yes):
        return 0

    if not _windows_is_admin():
        argv = [__file__, *sys.argv[1:]]
        ret = ctypes.windll.shell32.ShellExecuteW(
            None,
            "runas",
            sys.executable,
            subprocess.list2cmdline(argv),
            None,
            1,
        )
        if ret > 32:
            _log.info("Launched admin shell")
            return 0
        _log.info(f"Failed to launch admin shell. Error code {ret}")
        return 1

    if drivers_zip is not None and not drivers_zip.exists():
        raise OSError(f"drivers_zip file '{drivers_zip}' doesn't exist")

    with tempfile.TemporaryDirectory(prefix="seabreeze-os-") as tmp_dir:
        tmp_dir_path = Path(tmp_dir)
        zip_path = drivers_zip if drivers_zip is not None else tmp_dir_path / _DRIVERS_ZIP_FN

        if drivers_zip is None:
            _download_driver_zip(zip_path)

        with zipfile.ZipFile(zip_path, "r") as dzip:
            names = dzip.namelist()
            if not _is_contained_in_dir(names, tmp_dir_path):
                raise RuntimeError("zip file contains path traversal entries")
            dzip.extractall(tmp_dir_path)
        _log.info(f"Extracted to temporary directory {tmp_dir_path}")

        pnputil = (
            r"%systemroot%\Sysnative\pnputil.exe"
            if "32bit" in platform.architecture()
            else "pnputil.exe"
        )
        return_code = subprocess.call(
            [pnputil, "-i", "-a", str(tmp_dir_path / "*.inf")], shell=True
        )

        _log.warning(
            dedent(
                """\
                Note: Some of the drivers currently don't have valid signatures.
                Look at the output above. If the spectrometer you want to use only
                provides an unsigned driver, you might have to install it manually.
                If you encounter this issue, please report it on github."""
            )
        )

        if return_code == 0:
            _log.info("Success")
        elif return_code == 3010:
            _log.info("Success! REBOOT REQUIRED!")
        else:
            _log.error(f"pnputil returned with {return_code}")
        return return_code


def _build_linux_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--overwrite-existing",
        help="overwrite rules if already present",
        action="store_true",
    )
    parser.add_argument(
        "--yes",
        help="skip interactive confirmation prompts",
        action="store_true",
    )
    parser.add_argument(
        "rules",
        help="rules file (default: download from github)",
        default="",
        nargs="?",
    )
    return parser


def _build_windows_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--yes",
        help="skip interactive confirmation prompts",
        action="store_true",
    )
    parser.add_argument(
        "drivers_zip",
        help="drivers zip file (default: download from github)",
        default="",
        nargs="?",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    argsv = list(argv) if argv is not None else sys.argv[1:]
    system = platform.system()

    if system == "Windows":
        args = _build_windows_parser().parse_args(argsv)
        drivers_zip = Path(args.drivers_zip) if args.drivers_zip else None
        return windows_install_drivers(drivers_zip=drivers_zip, assume_yes=args.yes)

    if system == "Linux":
        args = _build_linux_parser().parse_args(argsv)
        rules = Path(args.rules) if args.rules else None
        return linux_install_udev_rules(
            rules=rules,
            overwrite_existing=bool(args.overwrite_existing),
            assume_yes=bool(args.yes),
        )

    _log.info(f"Nothing to do for system '{system}'")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
