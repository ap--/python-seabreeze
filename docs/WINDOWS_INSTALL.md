# Installing on Windows

I write this, because of some e-mails I received from Windows users:

* If you contact me, be nice!
* I do not run Windows.
* I am not an oceanoptics employee and do this in my free time.
* I have confirmation of several people that run python-seabreeze on Windows
  XP, Windows 7 and Windows 8. I do not know of people who run this on Windows
  10.
* If you run into problems while trying to install this module, it is most
  likely that the issues are on your side. Please, please, please spend some
  time using google to find solutions.
* And if you manage to get it up and running, and you think that the install
  instructions here are bad, send me better ones.


## Backends

On windows you can't easily run both backends side by side, because you have to
switch drivers when switching backends. So choose now.

### Installing requirements for cseabreeze backend on Windows

I require that you have read the [oceanoptics install
guide](http://oceanoptics.com/api/seabreeze/) before you file any issue!

1. Download the seabreeze installer from sourceforge for your architecture
   [here](https://sourceforge.net/projects/seabreeze/files/SeaBreeze/installers/).
2. Run the downloaded file and install the library.

When you connect your spectrometer you have to use the driver files that come
with this installer. So you have to decide to either use **seabreeze OR
OmniDriver**. Since you want to use python-seabreeze you have to go for the
seebreeze drivers, which means OceanView is most likely to stop working for
you.

### Installing requirements for pyseabreeze backend on Windows

You need to get [pyUSB](http://walac.github.io/pyusb/) up and running. As soon
as pyUSB is working on your machine and you are using a libusb driver for your
spectrometer the pyseabreeze backend will work (I am going to asume that you
use libusb. Other pyUSB-supported implementations should work too).

I will only help you, if you can run the following in your python interpreter
and it does not raise any exception:

```python
import usb.core
usb.core.find()
```

If it does raise an exception, you did not install pyUSB correctly. Please use
google to find a pyUSB installation guide that works. If you have one, let me
know, and I'll add it here for other Windows users.

When you connect your spectrometer you have to use the driver files that come
with libusb. So you have to decide to either use **pyseabreeze OR
OmniDriver**. Since you want to use python-seabreeze you have to go for the
libusb drivers, which means OceanView is most likely to stop working for
you.


## python-seabreeze

1. Install AND/OR upgrade [pip](https://pip.pypa.io/en/stable/installing/). Do
   not file any issues if `pip --version` returns less than version _8.x.x_!
2. Download the correct wheel from the [releases
   page](https://github.com/ap--/python-seabreeze/releases). You need to choose
   the wheel matching you python version (i.e. Python2.7.3 == cp27) and your
   python interpreter architecture (i.e. 64bit python == win_amd64)

Then you can just run:

```
pip install NAME-OF-THE-PYTHON-WHEEL-I-JUST-DOWNLOADED.whl
```

You made it. Congratulations.


## Common problems

#### I can't import seabreeze

So most likely you want to use cseabreeze and the import fails because the
python-seabreeze cseabreeze wrapper can't be imported. If you run:

```
import seabreeze.cseabreeze
```

it raises an exception. The most common cause why this happens because
SeaBreeze.dll can't be found. If you used any of the UNOFFICIAL seabreeze
installers, get rid of them. Also get rid of every SeaBreeze.dll you can find
on your computer (seriously). And then follow the install instructions above.
If it still doesn't work, try adding the folder that has the Seabreeze.dll to
your %PATH% environment variable. If you have no clue what I am talking about,
google the previous sentence and make sure that you do it right. If it still
doesn't work try copying the Seabreeze.dll to the same folder that you run your
script from (But don't blame me if at some point in time something breaks.).

#### python-seabreeze can't find my spectrometer

It is most likely that you are still using the OmniDriver for your
spectrometer. Please switch to the drivers that the seabreeze installer
provides. (Or libusb, dependent on what you are using.)

#### Because (some reason), I want to build the module myself, but can't

Look at the appveyor install scripts. And then you need to figure out what's
installed on appveyor virtual machines. I would not be able to help you setup
your environment. As I said I am not a windows user.

