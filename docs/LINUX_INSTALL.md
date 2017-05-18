# Installing on Linux

_Okay, I really need to emphasize, that using Anaconda is the recommended way
to install this module. But ok:_

This should really run anywhere. I have confirmation of people running this on
ubuntu, debian, arch and others. On 32bit and 64bit systems, as well as on ARM
on the Raspberry PI.

#### Before you can start 

Stuff you need for installing python-seabreeze without conda.

```bash
# if your distro is a deb flavor
sudo apt-get install git-all build-essential libusb-dev
```

```bash
# if your distro is a rpm flavor
sudo yum install git-all gcc gcc-c++ make libusb-devel
```

```bash
# if your distro is a arch flavor
pacman -S base-devel libusb libusb-compat libusb-devel make gcc
```

```bash
# Make sure to install pip and:
pip --version  # has to be >= 8.0.0
```


#### Install the C library backend

You need to build and install _libseabreeze.so_. There's a helper script in this
repositories misc folder.

```bash
# script is located in the misc folder
./install_libseabreeze.sh
```


#### Install pyUSB

If you want to use pyseabreeze, you'll need this.

```bash
pip install pyusb
```

You can check, if it is installed correctly by running:

```python
>>> import usb.core
>>> usb.core.find()
```

If this code raises an exception google how to get pyUSB up and running for
your distribution first.


#### python-seabreeze

Now you need to setup the udev rules. Theres another helper script in the misc
folder:

```
./install_udev_rules.sh
```

You also need to make sure that your user is in the plugdev group! Also please
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

Note: If you want to build cseabreeze，you should install Cython.

```
pip install cython
```

## Common problems

#### python-seabreeze can't find my spectrometer

Please double check that you are using the 10-oceanoptics.rules file from this
repository! There are several different ones floating around. If you setup
everything correctly, after pluging in your spectrometer, you should see a
symlink created in /dev that is named like your spectrometer model and owned by
the "plugdev" group.

Also make sure you add your user to the plugdev group (and create the group if
it doesn't exist on your machine). Google is your friend.

#### Can't find libseabreeze.so 

The library is installed to /usr/local/lib/. 
Some distributions don't seem to have this location in their default search path.
You can check if python will find the shared library later, by running ldconfig -v.
If it shows libseabreeze you're done. If not, you need to "add /usr/local/lib" to your library search path. Google "add /usr/local/lib", and you'll find how to do that for your distribution.
Most likely you'll have to do:

```bash
"/usr/local/lib" >> /etc/ld.so.conf.d/usrlocal.conf
sudo ldconfig -v
```

