Contributing Guide
==================

.. _contrib:

If you want to contribute to seabreeze, please create a pull request on github.
Documentation improvements are very welcome!
Extending pyseabreeze is quite easy. Look at the examples below.

.. tip::

    also feel free to create a PR improving the documentation if anything is unclear <3


Adding a new spectrometer
-------------------------

To add a new spectrometer with basic spectrometer functionality to `pyseabreeze` you
first need to find a datasheet pdf of your spectrometer to get the necessary information
and then you need to do two things:

Add a new device class to `seabreeze/pyseabreeze/devices.py`:

.. code:: python

    class SPARK(SeaBreezeDevice):

        model_name = 'SPARK'

        # communication config
        transport = (USBTransport, )
        usb_product_id = 0x4200
        usb_endpoint_map = EndPointMap(ep_out=0x01, lowspeed_in=0x81)  # XXX: we'll ignore the alternative EPs
        usb_protocol = OBPProtocol

        # spectrometer config
        dark_pixel_indices = DarkPixelIndices.from_ranges()
        integration_time_min = 10
        integration_time_max = 85000000
        integration_time_base = 1
        spectrum_num_pixel = 1024
        spectrum_raw_length = (1024 * 2) + 64  # XXX: Metadata
        spectrum_max_value = 16383
        trigger_modes = TriggerMode.supported('OBP_NORMAL', 'OBP_EXTERNAL', 'OBP_INTERNAL')

        # features
        feature_classes = (
            sbf.spectrometer.SeaBreezeSpectrometerFeatureSPARK,  # need to implement this
            sbf.rawusb.SeaBreezeRawUSBBusAccessFeature,
        )

And implement the specialized spectrometer feature class in `seabreeze/pyseabreeze/features/spectrometer.py`

.. code:: python

    # we're lucky here, and the Spark just uses the default implementation
    # of the OBP Protocol. It's possible that for other spectrometers you
    # need to override the `get_intensities` method or others dependent on how
    # much they deviate from the default implementation
    # (refer to the spectrometer's datasheet for this)
    #
    class SeaBreezeSpectrometerFeatureSPARK(SeaBreezeSpectrometerFeatureOBP):
        pass


That's it. These few lines add basic `Spark` support to `pyseabreeze`.


Testing spectrometer models
---------------------------

You can help too, by running the provided tests for your spectrometer model and posting the
results in a github issue. To do this git clone the repository and:

.. code:: bash

    pip install 'pytest-runner' 'pytest<5'
    python setup.py pytest

    running pytest
    running egg_info
    writing requirements to src/seabreeze.egg-info/requires.txt
    writing src/seabreeze.egg-info/PKG-INFO
    writing top-level names to src/seabreeze.egg-info/top_level.txt
    writing dependency_links to src/seabreeze.egg-info/dependency_links.txt
    writing entry points to src/seabreeze.egg-info/entry_points.txt
    reading manifest template 'MANIFEST.in'
    writing manifest file 'src/seabreeze.egg-info/SOURCES.txt'
    running build_ext
    skipping 'src/seabreeze/cseabreeze/c_seabreeze_wrapper.cpp' Cython extension (up-to-date)
    copying build/lib.linux-x86_64-2.7/seabreeze/cseabreeze/_wrapper.so -> src/seabreeze/cseabreeze
    ========================================== test session starts ==========================================
    platform linux2 -- Python 2.7.15+, pytest-4.6.5, py-1.8.0, pluggy-0.12.0 -- /python
    cachedir: .pytest_cache
    rootdir: /home/poehlmann/Development/python-seabreeze, inifile: pytest.ini
    collected 23 items

    tests/test_backends.py::test_backend_features_interface PASSED                                    [  4%]
    tests/test_backends.py::test_cseabreeze_seabreezeapi PASSED                                       [  8%]
    tests/test_backends.py::test_pyseabreeze_seabreezeapi PASSED                                      [ 13%]
    tests/test_spectrometers.py::test_read_model[cseabreeze:SPARK:00061] PASSED                       [ 17%]
    tests/test_spectrometers.py::test_read_model[pyseabreeze:SPARK:00061] PASSED                      [ 21%]
    tests/test_spectrometers.py::test_read_serial_number[cseabreeze:SPARK:00061] PASSED               [ 26%]
    tests/test_spectrometers.py::test_read_serial_number[pyseabreeze:SPARK:00061] PASSED              [ 30%]
    tests/test_spectrometers.py::test_read_intensities[cseabreeze:SPARK:00061] PASSED                 [ 34%]
    tests/test_spectrometers.py::test_read_intensities[pyseabreeze:SPARK:00061] PASSED                [ 39%]
    tests/test_spectrometers.py::test_read_wavelengths[cseabreeze:SPARK:00061] PASSED                 [ 43%]
    tests/test_spectrometers.py::test_read_wavelengths[pyseabreeze:SPARK:00061] PASSED                [ 47%]
    tests/test_spectrometers.py::test_read_spectrum[cseabreeze:SPARK:00061] PASSED                    [ 52%]
    tests/test_spectrometers.py::test_read_spectrum[pyseabreeze:SPARK:00061] PASSED                   [ 56%]
    tests/test_spectrometers.py::test_max_intensity[cseabreeze:SPARK:00061] PASSED                    [ 60%]
    tests/test_spectrometers.py::test_max_intensity[pyseabreeze:SPARK:00061] PASSED                   [ 65%]
    tests/test_spectrometers.py::test_integration_time_limits[cseabreeze:SPARK:00061] PASSED          [ 69%]
    tests/test_spectrometers.py::test_integration_time_limits[pyseabreeze:SPARK:00061] PASSED         [ 73%]
    tests/test_spectrometers.py::test_integration_time[cseabreeze:SPARK:00061] PASSED                 [ 78%]
    tests/test_spectrometers.py::test_integration_time[pyseabreeze:SPARK:00061] PASSED                [ 82%]
    tests/test_spectrometers.py::test_trigger_mode[cseabreeze:SPARK:00061] PASSED                     [ 86%]
    tests/test_spectrometers.py::test_trigger_mode[pyseabreeze:SPARK:00061] FAILED                    [ 91%]
    tests/test_spectrometers.py::test_cant_find_serial[cseabreeze] PASSED                             [ 95%]
    tests/test_spectrometers.py::test_cant_find_serial[pyseabreeze] PASSED                            [100%]

    =============================================== FAILURES ================================================
    ______________________________ test_trigger_mode[pyseabreeze:SPARK:00061] _______________________________

    backendlified_serial = '00061'

        def test_trigger_mode(backendlified_serial):
            devices = list(list_devices())
            if len(devices) == 0:
                pytest.skip("no supported device connected")

            exc = Spectrometer._backend.SeaBreezeError
            spec = Spectrometer.from_serial_number(backendlified_serial)

            with pytest.raises(exc):
                spec.trigger_mode(0xF0)  # <- should be unsupported for all specs

    >       spec.trigger_mode(0x00)  # <- normal mode

    tests/test_spectrometers.py:177:
    _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
    src/seabreeze/spectrometers.py:269: in trigger_mode
        self._dev.f.spectrometer.set_trigger_mode(mode)
    src/seabreeze/pyseabreeze/features/spectrometer.py:298: in set_trigger_mode
        self.protocol.send(0x00110110, mode)
    src/seabreeze/pyseabreeze/protocol.py:256: in send
        remaining_bytes, checksum_type = self._check_incoming_message_header(response[:44])
    _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

    self = <seabreeze.pyseabreeze.protocol.OBPProtocol object at 0x7f7ae94e4690>
    header = array('B', [193, 192, 0, 17, 11, ...])

        def _check_incoming_message_header(self, header):
            """check the incoming message header

            Parameters
            ----------
            header : `str`
                a obp header of length 44

            Returns
            -------
            bytes_and_checksum_type : tuple[`int`, `int`]
                bytes_remaining after the header (returns 20 for a 64 byte message)
                checksum_type only supports self.OBP.CHECKSUM_TYPE_MD5 for now
            """
            if len(header) != 44:
                raise SeaBreezeError("header has wrong length! len(header): %d" % len(header))

            data = struct.unpack(self.OBP.HEADER_FMT, header)

            if data[0] != self.OBP.HEADER_START_BYTES:
                raise SeaBreezeError('Header start_bytes wrong: "%d"' % data[0])
            if data[1] != self.OBP.HEADER_PROTOCOL_VERSION:
                raise SeaBreezeError('Header protocol version wrong: %d' % data[1])

            flags = data[2]
            if flags == 0:
                pass
            if flags & self.OBP.FLAG_RESPONSE_TO_REQUEST:
                pass  # TODO: propagate?
            if flags & self.OBP.FLAG_ACK:
                pass  # TODO: propagate?
            if flags & self.OBP.FLAG_REQUEST_ACK:
                pass  # TODO: only the host should be able to set this?
            if (flags & self.OBP.FLAG_NACK) or (flags & self.OBP.FLAG_HW_EXCEPTION):
                error = data[3]
                if error != 0:  # != SUCCESS
    >               raise SeaBreezeError(self.OBP.ERROR_CODES[error])
    E               SeaBreezeError: Unknown message type

    src/seabreeze/pyseabreeze/protocol.py:424: SeaBreezeError
    =========================================== warnings summary ============================================
    tests/test_spectrometers.py::test_read_intensities[pyseabreeze:SPARK:00061]
    tests/test_spectrometers.py::test_read_spectrum[pyseabreeze:SPARK:00061]
      /home/poehlmann/Development/python-seabreeze/src/seabreeze/pyseabreeze/features/spectrometer.py:345:
        DeprecationWarning: The binary mode of fromstring is deprecated, as it behaves surprisingly on
        unicode inputs. Use frombuffer instead return numpy.fromstring(datastring, dtype=numpy.uint8)

    -- Docs: https://docs.pytest.org/en/latest/warnings.html
    =========================== 1 failed, 22 passed, 2 warnings in 34.78 seconds ============================
    Exception AttributeError: "'NoneType' object has no attribute '_ctx'" in
        <bound method SPARK.__del__ of <SeaBreezeDevice SPARK:00061>> ignored
