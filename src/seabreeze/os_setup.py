# type: ignore
"""operating system relevant deploy functions

author: Andreas Poehlmann
"""
import argparse
import ctypes
import logging
import os
import platform
import shutil
import subprocess
import sys
import tempfile
import time
import zipfile
from textwrap import dedent
from textwrap import indent
from urllib.error import HTTPError
from urllib.request import urlopen

_GITHUB_REPO_URL = (
    "https://raw.githubusercontent.com/ap--/python-seabreeze/master/os_support"
)
_UDEV_RULES_PATH = "/etc/udev/rules.d/10-oceanoptics.rules"
_DRIVERS_ZIP_FN = "windows-driver-files.zip"
_log = logging.getLogger(__name__)


def _diff_files(file1, file2):
    """diff two files using linux `diff`"""
    try:
        return subprocess.check_output(["diff", file1, file2]).decode("utf8")
    except subprocess.CalledProcessError as err:
        return err.output.decode("utf8")


def _request_confirmation(question):
    """require user input to continue"""
    while True:
        user_input = input(f"{question} [y/n] ").lower()
        if user_input not in {"y", "n"}:
            _log.info("Please enter 'y' or 'n'.")
        elif user_input[0] == "n":
            return False
        else:
            return True
        time.sleep(0.1)


def linux_install_udev_rules():
    """verify and install the udev rules"""
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--overwrite-existing",
        help="overwrite rules if already present",
        action="store_true",
    )
    parser.add_argument(
        "rules",
        help="rules file (default: download from github)",
        default="",
        nargs="?",
    )
    args = parser.parse_args()

    if args.rules:
        if not os.path.exists(args.rules):
            raise OSError(f"rules file '{args.rules}' doesn't exist")
        udev_tmp_file = None
        udev_fn = args.rules

    else:
        udev_tmp_file = tempfile.NamedTemporaryFile()
        udev_fn = udev_tmp_file.name

    try:
        # download rules from github if no file is provided
        if udev_tmp_file is not None:
            url = f"{_GITHUB_REPO_URL}/{os.path.basename(_UDEV_RULES_PATH)}"
            try:
                _log.info("downloading rules from github")
                udev_data = urlopen(url).read()
            except HTTPError:
                _log.error(f"can't download '{url}'")
                sys.exit(1)
            udev_tmp_file.write(udev_data)
            udev_tmp_file.flush()

        # check if rules need to be overwritten
        if os.path.exists(_UDEV_RULES_PATH) and not args.overwrite_existing:
            rules_differ = _diff_files(_UDEV_RULES_PATH, udev_fn)
            if not rules_differ:
                _log.info("udev rules already newest version")
                sys.exit(0)
            else:
                _log.info(indent(rules_differ, "  ").rstrip())
                _log.info(
                    "udev rules differ. To overwrite run with '--overwrite-existing'"
                )
                sys.exit(1)

        if not _request_confirmation("Install udev rules?"):
            sys.exit(0)

        # cp rules and execute
        _log.info(f"Copying udev rules to {_UDEV_RULES_PATH}")
        subprocess.call(["sudo", "cp", udev_fn, _UDEV_RULES_PATH])
        _log.info("Calling udevadm control --reload-rules")
        subprocess.call(["sudo", "udevadm", "control", "--reload-rules"])
        _log.info("Success")
        sys.exit(0)

    finally:
        if udev_tmp_file is not None:
            udev_tmp_file.close()  # removes tempfile


def _windows_is_admin():
    """windows only: check if running as admin"""
    # noinspection PyBroadException
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except Exception:
        return False


def _is_contained_in_dir(files, cdir=None):
    cdir = os.path.abspath(cdir or os.path.curdir)
    for f in files:
        f_abs = os.path.abspath(f)
        if not os.path.commonprefix((f_abs, cdir)).startswith(cdir):
            return False
    return True


def windows_install_drivers():
    """install driver inf files via pnputil in an elevated shell"""
    if not _request_confirmation("Install windows drivers?"):
        sys.exit(0)

    if not _windows_is_admin():
        # Re-run the program with admin rights
        argv = [__file__] + sys.argv[1:]
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
        else:
            _log.info(f"Failed to launch admin shell. Error code {ret}")
        sys.exit(0 if ret > 32 else 1)

    # running as admin
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "drivers_zip",
        help="drivers zip file (default: download from github)",
        default="",
        nargs="?",
    )
    args = parser.parse_args()

    if args.drivers_zip:
        if not os.path.exists(args.drivers_zip):
            raise OSError(f"drivers_zip file '{args.drivers_zip}' doesn't exist")
        drivers_zip = args.drivers_zip
    else:
        drivers_zip = None

    tmp_dir = tempfile.mkdtemp(prefix="seabreeze-os-")
    # noinspection PyBroadException
    try:
        # download driver files
        if drivers_zip is None:
            url = f"{_GITHUB_REPO_URL}/{os.path.basename(_DRIVERS_ZIP_FN)}"
            drivers_zip = os.path.join(tmp_dir, _DRIVERS_ZIP_FN)
            with open(drivers_zip, "wb") as dzip:
                try:
                    _log.info("Downloading windows drivers from github")
                    drivers_zip_data = urlopen(url).read()
                except HTTPError:
                    _log.error(f"Can't download '{url}'")
                    sys.exit(1)
                dzip.write(drivers_zip_data)

        # extract driver files
        with zipfile.ZipFile(drivers_zip, "r") as dzip:
            if not _is_contained_in_dir(dzip.namelist()):
                raise Exception("Zipfile contains non subdir paths")
            dzip.extractall(tmp_dir)
        _log.info(f"Extracted to temporary directory {tmp_dir}")

        # use correct pnputil with 32bit pythons
        if "32bit" in platform.architecture():
            pnputil = r"%systemroot%\Sysnative\pnputil.exe"
        else:
            pnputil = "pnputil.exe"

        # install with pnp util
        cmd = [pnputil, "-i", "-a", os.path.join(tmp_dir, "*.inf")]
        return_code = subprocess.call(cmd, shell=True)

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

    except Exception:
        _log.error("Error when installing drivers", exc_info=True)

    finally:
        shutil.rmtree(tmp_dir)
        input("Press [enter] to close.")


def main():
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    system = platform.system()
    if system == "Windows":
        windows_install_drivers()
    elif system == "Linux":
        linux_install_udev_rules()
    else:
        _log.info(f"Nothing to do for system '{system}'")
    sys.exit(0)


if __name__ == "__main__":
    # fix for windows entry_point shims, which are actually .exe files...
    main()
