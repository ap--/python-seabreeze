"""operating system relevant deploy functions

author: Andreas Poehlmann
"""
import pkg_resources
import logging
import platform
import os
import argparse
import subprocess
import sys
import tempfile
import zipfile

from future.standard_library import hooks

with hooks():
    from urllib.request import urlopen
    from urllib.error import HTTPError

try:
    from textwrap import indent as _indent
except ImportError:
    def _indent(text, prefix, predicate=None):
        return "".join(prefix + line for line in text.splitlines(True))


_GITHUB_REPO_URL = 'https://raw.githubusercontent.com/ap--/python-seabreeze/master/os_support'
_UDEV_RULES_PATH = '/etc/udev/rules.d/10-oceanoptics.rules'
_log = logging.getLogger(__name__)


def _diff_files(file1, file2):
    """diff two files using linux `diff`"""
    try:
        return subprocess.check_output(['diff', file1, file2])
    except subprocess.CalledProcessError as err:
        return err.output


def linux_install_udev_rules():
    """verify and install the udev rules"""
    parser = argparse.ArgumentParser()
    parser.add_argument('--overwrite-existing', help="overwrite rules if already present", action='store_true')
    parser.add_argument('rules', help="rules file (default: download from github)", default='', nargs='?')
    args = parser.parse_args()

    if args.rules:
        if not os.path.exists(args.rules):
            raise IOError("rules file '{}' doesn't exist".format(args.rules))
        udev_tmp_file = None
        udev_fn = args.rules

    else:
        udev_tmp_file = tempfile.NamedTemporaryFile()
        udev_fn = udev_tmp_file.name

    try:
        # download rules from github if no file is provided
        if udev_tmp_file is not None:
            url = '{}/{}'.format(_GITHUB_REPO_URL, os.path.basename(_UDEV_RULES_PATH))
            try:
                _log.info("downloading rules from github")
                udev_data = urlopen(url).read()
            except HTTPError:
                _log.error("can't download '{}'".format(url))
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
                _log.info(_indent(rules_differ, '  ').rstrip())
                _log.info("udev rules differ. To overwrite run with '--overwrite-existing'")
                sys.exit(1)

        # cp rules and execute
        _log.info('Copying udev rules to {}'.format(_UDEV_RULES_PATH))
        subprocess.call(['sudo', 'cp', udev_fn, _UDEV_RULES_PATH])
        _log.info('Calling udevadm control --reload-rules')
        subprocess.call(['sudo', 'udevadm', 'control', '--reload-rules'])
        _log.info('Success')
        sys.exit(0)

    finally:
        if udev_tmp_file is not None:
            udev_tmp_file.close()  # removes tempfile


def windows_install_drivers():
    """install driver inf files via pnputil"""
    parser = argparse.ArgumentParser()
    parser.add_argument('--ignore-platform', help="", action='store_true')
    args = parser.parse_args()

    # download zip
    # create tempdir and extract zip
    # install via pnputil
    # TODO: runas admin?
    cmd = ['pnputil', '-a', '-i']
    # remove tempdir


def main():
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    system = platform.system()
    if system == "Windows":
        windows_install_drivers()
    elif system == "Linux":
        linux_install_udev_rules()
    else:
        _log.info("Nothing to do for system '{}'".format(system))
    sys.exit(0)
