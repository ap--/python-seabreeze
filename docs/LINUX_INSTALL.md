# Installing on Linux

This should really run anywhere. I have confirmation of people running this on
ubuntu, debian, arch and others. On 32bit and 64bit systems, as well as on ARM
on the Raspberry PI.

Let's get started:

1. Clone/download this repository 
2. Install AND/OR upgrade [pip](https://pip.pypa.io/en/stable/installing/). Do
   not file any issues if `pip --version` returns less than version _8.x.x_!

## Backends

### Installing requirements for cseabreeze backend on Linux

You need to build and install _libseabreeze.so_. There's a helper script in this
repositories misc folder (requires wget).

Before running it you need to make sure that you have a working build
environment and libusb installed. Please refer to your distribution how to
install these requirements. Here are one-liners for some distributions (let me
know if it's different for your distribution):

```
# UBUNTU
sudo apt-get install build-essential python-numpy libusb-0.1-4 libusb-dev
# ARCH LINUX (try one or the other)
sudo pacman -S base-devel libusb libusb-devel 
sudo pacman -S base-devel libusb-compat make gcc python2-pip python2-setuptools
# RHEL   
sudo yum install gcc gcc-c++ make libusb libusb-devel 
```

After that run 
```
./install_libseabreeze.sh
```
from the misc folder.

This will install _libseabreeze.so_ to /usr/local/lib/. You can check if python
will find the shared library later, by running `ldconfig -v`. If it shows
libseabreeze you're done. If not, you need to "add /usr/local/lib" to your
library search path. Google "add /usr/local/lib", and you'll find how to do
that for your distribution. Most likely you do `echo "/usr/local/lib" >>
/etc/ld.so.conf.d/usrlocal.conf` and run ldconfig to rebuild the cache.


### Installing requirements for pyseabreeze backend on Linux

You need to get pyUSB up and running. Check if your distributions package manager
has a pyusb packaged first. If not:

```
pip install pyusb==1.0.0b2
```

If it is correctly installed, you should be able to run

```python
import usb.core
usb.core.find()
```

without any exception being raised. If it raises an exception google how to
get pyusb up and running for your distribution first.


## python-seabreeze

Now you need to setup the udev rules. Theres another helper script in the misc
folder:

```
./install_udev_rules.sh
```

You also need to make sure that your user is in the plugdev group!  Also please
double check that you are using the 10-oceanoptics.rules file from this
repository! There are several different ones floating around.

After that go to the repository root and run:

```
pip install .
```

or if you do not want to build the cseabreeze wrapper:

```
python setup.py install --without-cseabreeze
```

You made it. Congratulations.


## Common problems

#### python-seabreeze can't find my spectrometer

Please double check that you are using the 10-oceanoptics.rules file from this
repository! There are several different ones floating around. If you setup
everything correctly, after pluging in your spectrometer, you should see a
symlink created in /dev that is named like your spectrometer model and owned by
the "plugdev" group.

Also make sure you add your user to the plugdev group (and create the group if
it doesn't exist on your machine). Google is your friend.

