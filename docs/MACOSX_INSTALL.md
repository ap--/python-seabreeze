# Installing on MacOSX

This should work on OSX versions 10.6 up to the most recent. But only on INTEL
architectures, not on PowerPC. I am testing on 10.10. And I have confirmation
for 10.9. Let me know if you can confirm it working on a different version.

## Backends

I really recommend using the cseabreeze backend, because you don't have to do
anything.

### Installing requirements for cseabreeze backend on OSX

Nothing. You are done :) The library ships with the wheel.

### Installing requirements for pyseabreeze backend on OSX

You need to find a tutorial for running pyUSB on OSX. If you find a good one,
let me know, then I'll add it here. But I really recommend running cseabreeze.

## python-seabreeze

1. Install AND/OR upgrade [pip](https://pip.pypa.io/en/stable/installing/). Do
   not file any issues if `pip --version` returns less than version _8.x.x_!
   This is crucial because the wheels are built for both architectures and only
   recent pip versions allow the installation (as far as I understand)
2. Download the correct wheel from the [releases
   page](https://github.com/ap--/python-seabreeze/releases). You need to choose
   the wheel matching you python version (i.e. Python2.7.3 == cp27).

Then you can just run:

```
pip install NAME-OF-THE-PYTHON-WHEEL-I-JUST-DOWNLOADED.whl
```

You made it. Congratulations.


## Common problems

#### I can't install the wheel

Upgrade your pip version to something above 8.x.x. That should fix it. If not,
try running a python interpreter downloaded from
[python.org](https://www.python.org/downloads/). The wheels are built for
those.

