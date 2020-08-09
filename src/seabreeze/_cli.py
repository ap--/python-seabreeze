"""commandline interactions

Future versions could expand this into a full commandline interface
for scripting spectrometer things.

  TODO
    - implement main() to be used as an entry point
    - handle cmdline with argparse
    - basic spectrum acquisition should be enough
    - has to be stateless! open/close overhead is acceptable

WARNING
-------
This submodule is not stable. Anything in here can change even between
micro version releases. Do not rely on this yet. If you want this to
be a public interface open an issue on GitHub and/or work on a Pull
Request implementing the functionality.

author: Andreas Poehlmann
"""
from __future__ import print_function

import importlib
import itertools
import operator


def ls():
    """INTERNAL ONLY: print connected spectrometers"""
    connected = []
    for backend in ("cseabreeze", "pyseabreeze"):
        # noinspection PyBroadException
        try:
            sb_backend = importlib.import_module("seabreeze.{}".format(backend))
            api = sb_backend.SeaBreezeAPI()
            try:
                devices = api.list_devices()
                for d in devices:
                    # noinspection PyProtectedMember
                    connected.append((d.model, d.serial_number, sb_backend._backend_))
            finally:
                api.shutdown()
        except Exception:
            pass
        finally:
            # noinspection PyUnusedLocal
            sb_backend = None

    key_func = operator.itemgetter(0, 1)  # sort by (model, serial)
    connected = sorted(connected, key=key_func)

    # output format:
    # MY_MODEL   SERIAL_1234  cseabreeze,pyseabreeze
    for model_serial_number, info in itertools.groupby(connected, key=key_func):
        model, serial = model_serial_number
        print("{}\t{}\t{}".format(model, serial, ",".join(b[2] for b in info)))
