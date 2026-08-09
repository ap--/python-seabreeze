# OS support files

To use your spectrometer with python-seabreeze your operating system may
require additional setup. Python-seabreeze provides a command line tool for
os specific setup. Simply run the following command after you've installed
python-seabreeze in your Python environment:

```bash
seabreeze_os_setup
```

It will download the necessary files and run the setup commands required.
You may have to unplug and replug your usb spectrometer afterwards.

---

#### What does seabreeze_os_setup do?

That depends on your operating system. Currently only Linux and Windows require
additional setup. OSX works ootb and is a no-op when running
`seabreeze_os_setup`

**On Linux** it downloads the `10-oceanoptics.rules` file, prints its contents
for review, and, after you confirm, copies it to `/etc/udev/rules.d/` with mode
`644` and runs `sudo udevadm control --reload-rules`. If you decline, the rules
are not installed and the commands for installing them manually are printed.

**On Windows** it downloads and extracts the `windows-driver-files.zip` archive
and runs `pnputil -i -a *.inf` inside the extracted folder in an admin shell.
