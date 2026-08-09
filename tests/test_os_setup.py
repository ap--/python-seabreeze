"""tests for the `seabreeze_os_setup` command line tool"""

import logging
import sys
from unittest import mock

import pytest

from seabreeze import os_setup


@pytest.fixture
def udev_rules_file(tmp_path):
    """a local rules file provided to the setup script"""
    rules_file = tmp_path / "10-oceanoptics.rules"
    rules_file.write_text("# oceanoptics test rules\n")
    yield str(rules_file)


@pytest.fixture
def udev_rules_path(tmp_path, monkeypatch):
    """a not yet existing udev rules install location"""
    rules_path = str(tmp_path / "rules.d" / "10-oceanoptics.rules")
    monkeypatch.setattr(os_setup, "_UDEV_RULES_PATH", rules_path)
    yield rules_path


@pytest.fixture
def subprocess_call():
    with mock.patch.object(os_setup.subprocess, "call", return_value=0) as call:
        yield call


def _run_linux_install_udev_rules(rules_file, monkeypatch, confirm):
    monkeypatch.setattr(sys, "argv", ["seabreeze_os_setup", rules_file])
    monkeypatch.setattr(os_setup, "_request_confirmation", lambda question: confirm)
    with pytest.raises(SystemExit) as exc_info:
        os_setup.linux_install_udev_rules()
    return exc_info.value.code


def test_linux_install_udev_rules_previews_rules(
    udev_rules_file, udev_rules_path, subprocess_call, monkeypatch, caplog
):
    """the rules are printed before they get installed"""
    caplog.set_level(logging.INFO)
    code = _run_linux_install_udev_rules(udev_rules_file, monkeypatch, confirm=True)

    assert code == 0
    assert "# oceanoptics test rules" in caplog.text
    assert udev_rules_path in caplog.text


def test_linux_install_udev_rules_installs_with_644(
    udev_rules_file, udev_rules_path, subprocess_call, monkeypatch
):
    """the rules are installed world readable"""
    code = _run_linux_install_udev_rules(udev_rules_file, monkeypatch, confirm=True)

    assert code == 0
    assert (
        mock.call(["sudo", "install", "-m", "644", udev_rules_file, udev_rules_path])
        in subprocess_call.call_args_list
    )


def test_linux_install_udev_rules_declined(
    udev_rules_file, udev_rules_path, subprocess_call, monkeypatch, caplog
):
    """nothing is installed and manual instructions are shown"""
    caplog.set_level(logging.INFO)
    code = _run_linux_install_udev_rules(udev_rules_file, monkeypatch, confirm=False)

    assert code == 0
    assert subprocess_call.call_args_list == []
    assert f"sudo chmod 644 {udev_rules_path}" in caplog.text


def test_linux_install_udev_rules_install_error(
    udev_rules_file, udev_rules_path, subprocess_call, monkeypatch
):
    """a failing install command is reported as an error"""
    subprocess_call.return_value = 1
    code = _run_linux_install_udev_rules(udev_rules_file, monkeypatch, confirm=True)

    assert code == 1
