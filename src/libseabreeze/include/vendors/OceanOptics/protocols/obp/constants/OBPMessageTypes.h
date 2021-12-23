/***************************************************//**
 * @file    OBPMessageTypes.h
 * @date    January 2011
 * @author  Ocean Optics, Inc.
 *
 * This class contains legal values for the messageType
 * field in the OBPMessage class.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *******************************************************/

#ifndef OBPMESSAGETYPES_H
#define OBPMESSAGETYPES_H

namespace seabreeze {
  namespace oceanBinaryProtocol {
    class OBPMessageTypes {
    public:
// Basic hardware and firmware information
        static const unsigned int OBP_RESET								= 0x00000000;
        static const unsigned int OBP_RESET_DEFAULTS					= 0x00000001;
		static const unsigned int OBP_GET_DEVICE_STATUS					= 0x00000005;
		static const unsigned int OBP_GET_HEALTH_INDICATION				= 0x00000006;
		static const unsigned int OBP_SET_CHECKSUM_TYPE					= 0x00000010;
		static const unsigned int OBP_SET_PROTOCOL_REVISION				= 0x00000020;
        static const unsigned int OBP_GET_HARDWARE_REVISION				= 0x00000080;
		static const unsigned int OBP_GET_RECOVERY_STATUS				= 0x00000081;
		static const unsigned int OBP_GET_SUPPORTED_COMMAND_LIST		= 0x00000082;
        static const unsigned int OBP_GET_FIRMWARE_REVISION			    = 0x00000090;
		static const unsigned int OBP_GET_FIRMWARE_2_REVISION			= 0x00000091;
        static const unsigned int OBP_GET_SERIAL_NUMBER					= 0x00000100;
        static const unsigned int OBP_GET_SERIAL_NUMBER_LENGTH			= 0x00000101;
        static const unsigned int OBP_GET_DEVICE_ALIAS					= 0x00000200;
        static const unsigned int OBP_GET_DEVICE_ALIAS_LENGTH			= 0x00000201;
        static const unsigned int OBP_SET_DEVICE_ALIAS					= 0x00000210;
        static const unsigned int OBP_GET_USER_STRING_COUNT				= 0x00000300;
        static const unsigned int OBP_GET_USER_STRING_LENGTH			= 0x00000301;
        static const unsigned int OBP_GET_USER_STRING					= 0x00000302;
        static const unsigned int OBP_SET_USER_STRING					= 0x00000310;

// Time references
		static const unsigned int OBP_GET_CURRENT_TIME					= 0x00000400;
		static const unsigned int OBP_SET_CURRENT_TIME					= 0x00000410;



// Serial Interface
        static const unsigned int OBP_GET_RS232_BAUD_RATE				= 0x00000800;
        static const unsigned int OBP_GET_RS232_FLOW_COTROL_MODE		= 0x00000804;
        static const unsigned int OBP_SET_RS232_BAUD_RATE				= 0x00000810;
        static const unsigned int OBP_SET_RS232_FLOW_COTROL_MODE		= 0x00000814;
		static const unsigned int OBP_GET_RS232_BUS_MODE				= 0x000008C0;
		static const unsigned int OBP_SET_RS232_BUS_MODE				= 0x000008D0;
        static const unsigned int OBP_GET_RS232_SAVE_SETTINGS			= 0x000008F0;

// network configuration
		static const unsigned int OBP_GET_NETWORK_INTERFACE_COUNT		= 0x00000900;
		static const unsigned int OBP_GET_INTERFACE_CONNECTION_INFO		= 0x00000901;
		static const unsigned int OBP_GET_INTERFACE_ENABLE_STATE		= 0x00000902;
		static const unsigned int OBP_RUN_INTERFACE_SELFTEST			= 0x00000903;
		static const unsigned int OBP_SAVE_INTERFACE_CONNECTION_INFO	= 0x00000911;
		static const unsigned int OBP_SET_INTERFACE_ENABLE_STATE		= 0x00000912;

// ethernet
		static const unsigned int OBP_GET_GBE_ENABLE_STATE				= 0x00000920;
		static const unsigned int OBP_SET_GBE_ENABLE_STATE				= 0x00000930;
		static const unsigned int OBP_GET_MAC_ADDRESS					= 0x00000940;
		static const unsigned int OBP_SET_MAC_ADDRESS					= 0x00000950;

// wifi
		static const unsigned int OBP_GET_WIFI_SSID						= 0x00000960;
		static const unsigned int OBP_GET_WIFI_MODE						= 0x00000961;
		static const unsigned int OBP_GET_WIFI_SECURITY					= 0x00000962;
		static const unsigned int OBP_SET_WIFI_SSID						= 0x00000970;
		static const unsigned int OBP_SET_WIFI_MODE						= 0x00000971;
		static const unsigned int OBP_SET_WIFI_SECURITY					= 0x00000972;
		static const unsigned int OBP_SET_WIFI_PASSPHRASE				= 0x00000973;

// DHCP Server
		static const unsigned int OBP_GET_DHCP_SERVER_ENABLE_STATE		= 0x00000980;
		static const unsigned int OBP_GET_DHCP_SERVER_ADDRESS			= 0x00000981;
		static const unsigned int OBP_SET_DHCP_SERVER_ENABLE_STATE		= 0x00000990;
		static const unsigned int OBP_SET_DHCP_SERVER_ADDRESS			= 0x00000991;

// IPv4 Addressing
		static const unsigned int OBP_GET_DHCP_CLIENT_ENABLE_STATE		= 0x00000A00;
		static const unsigned int OBP_GET_IPV4_ADDRESS_COUNT			= 0x00000A01;
		static const unsigned int OBP_GET_IPV4_ADDRESS_CIDR				= 0x00000A02;
		static const unsigned int OBP_GET_IPV4_DEFAULT_GATEWAY			= 0x00000A08;
		static const unsigned int OBP_SET_DHCP_CLIENT_ENABLE_STATE		= 0x00000A10;
		static const unsigned int OBP_DELETE_IPV4_ADDRESS				= 0x00000A11;
		static const unsigned int OBP_ADD_IPV4_ADDRESS_CIDR				= 0x00000A12;
		static const unsigned int OBP_SET_IPV4_DEFAULT_GATEWAY			= 0x00000A18;

// IPv4 Multicast
		static const unsigned int OBP_GET_IPV4_MULTICAST_ENABLE_STATE	= 0x00000A80;
		static const unsigned int OBP_GET_IPV4_MULTICAST_GRP_ADDRESS	= 0x00000A81;
		static const unsigned int OBP_GET_IPV4_MULTICAST_GRP_PORT		= 0x00000A82;
		static const unsigned int OBP_GET_IPV4_MULTICAST_TTL			= 0x00000A83;
		static const unsigned int OBP_SET_IPV4_MULTICAST_ENABLE_STATE	= 0x00000A90;
		static const unsigned int OBP_SET_IPV4_MULTICAST_GRP_ADDRESS	= 0x00000A91;
		static const unsigned int OBP_SET_IPV4_MULTICAST_GRP_PORT		= 0x00000A92;
		static const unsigned int OBP_SET_IPV4_MULTICAST_TTL			= 0x00000A93;

// Device Identification
		static const unsigned int OBP_GET_ORIGINAL_VID					= 0x00000E00;
		static const unsigned int OBP_GET_ORIGINAL_PID					= 0x00000E01;
		static const unsigned int OBP_GET_ORIGINAL_MFR_STRING			= 0x00000E10;
		static const unsigned int OBP_GET_ORIGINAL_DEVICE_STRING		= 0x00000E11;
		static const unsigned int OBP_GET_VID							= 0x00000E20;
		static const unsigned int OBP_GET_PID							= 0x00000E21;
		static const unsigned int OBP_SET_VID							= 0x00000E30;
		static const unsigned int OBP_SET_PID							= 0x00000E31;
		static const unsigned int OBP_GET_MANUFACTURER_STRING			= 0x00000E40;
		static const unsigned int OBP_GET_DEVICE_STRING					= 0x00000E41;
		static const unsigned int OBP_SET_MANUFACTURER_STRING			= 0x00000E50;
		static const unsigned int OBP_SET_DEVICE_STRING					= 0x00000E51;

// bluetooth
		static const unsigned int OBP_GET_BLUETOOTH_ENABLE_STATE		= 0x00000F00;
		static const unsigned int OBP_GET_INQUIRY_ACCESS_CODE			= 0x00000F02;
		static const unsigned int OBP_SET_BLUET00TH_ENABLE_STATE		= 0x00000F10;
		static const unsigned int OBP_SET_BLUETOOTH_PIN					= 0x00000F11;
		static const unsigned int OBP_SET_INQUIRY_ACCESS_CODE			= 0x00000F12;
		static const unsigned int OBP_GET_MAXIMUM_PAIRED_DEVICE_COUNT	= 0x00000F20;
		static const unsigned int OBP_GET_PAIRED_DEVICE_COUNT			= 0x00000F21;
		static const unsigned int OBP_GET_PAIRED_DEVICE_MAC				= 0x00000F22;
		static const unsigned int OBP_REMOVE_PAIRED_DEVIE_MAC			= 0x00000F32;
		static const unsigned int OBP_MAKE_DEVICE_DISCOVERABLE			= 0x00000F33;
		static const unsigned int OBP_SAVE_BLUETOOTH_SETTINGS			= 0x00000FD0;
		static const unsigned int OBP_GET_BLUETOOTH_ADDRESS				= 0x00000FE0;
		static const unsigned int OBP_GET_BLUETOOTH_DEVICE_NAME			= 0x00000FE1;
		static const unsigned int OBP_GET_RFCOMM_CHANNEL_NUMBER			= 0x00000FE2;
		static const unsigned int OBP_GET_L2CAP_PORT_NUMBER				= 0x00000FE3;

// LED states
		static const unsigned int OBP_GET_STATUS_LED_COUNT				= 0x00001000;
		static const unsigned int OBP_GET_STATUS_LED_MODE				= 0x00001001;
		static const unsigned int OBP_GET_STATUS_LED_PATTERN			= 0x00001002;
        static const unsigned int OBP_CONFIGURE_STATUS_LED				= 0x00001010; // legacy
		static const unsigned int OBP_SET_STATUS_LED_PATTERN			= 0x00001010;
		static const unsigned int OBP_SET_STATUS_LED_MODE				= 0x00001011;

// Buffering
		static const unsigned int OBP_ABORT_ACQUISITION					= 0x00100000;
        static const unsigned int OBP_GET_BUFFER_SIZE_MAX				= 0x00100820;
        static const unsigned int OBP_GET_BUFFER_SIZE_ACTIVE			= 0x00100822;
        static const unsigned int OBP_CLEAR_BUFFER_ALL					= 0x00100830;
		static const unsigned int OBP_REMOVE_OLDEST_SPECTRA				= 0x00100831;
        static const unsigned int OBP_SET_BUFFER_SIZE_ACTIVE			= 0x00100832;
        static const unsigned int OBP_GET_BUFFERED_SPEC_COUNT			= 0x00100900;


// Fast Buffering
		static const unsigned int OBP_GET_BUFFERING_ENABLED				= 0x00100800;
		static const unsigned int OBP_SET_BUFFERING_ENABLED				= 0x00100810;
		static const unsigned int OBP_SET_BACK_TO_BACK_SAMPLE_COUNT		= 0x00110112;
		static const unsigned int OBP_GET_BACK_TO_BACK_SAMPLE_COUNT		= 0x00110102;

//Spectra queries
		static const unsigned int OBP_ACQUIRE_SPECTRA_INTO_BUFFER		= 0x00100902;
		static const unsigned int OBP_GET_DEVICE_IDLE_STATE				= 0x00100908;
        static const unsigned int OBP_GET_BUF_SPEC32_META				= 0x00100928;
		static const unsigned int OBP_GET_N_BUF_RAW_SPECTRA_META		= 0x00100980;
        static const unsigned int OBP_GET_CORRECTED_SPECTRUM_NOW		= 0x00101000;
		static const unsigned int OBP_GET_MAXIMUM_ADC_COUNTS			= 0x00101010;


// Saturation
        static const unsigned int OBP_GET_SATURATION_LEVEL				= 0x001010A0;
		static const unsigned int OBP_GET_BASELINE_LEVEL				= 0x001010A1;
		static const unsigned int OBP_GET_MAXIMUM_SATURATION_LEVEL		= 0x001010A2;
		static const unsigned int OBP_GET_MAXIMUM_BASELINE_LEVEL		= 0x001010A3;



// Partial and raw spectra
		static const unsigned int OBP_GET_RAW_SPECTRUM_NOW				= 0x00101100;
        static const unsigned int OBP_GET_PARTIAL_SPECTRUM_MODE			= 0x00102000;
        static const unsigned int OBP_SET_PARTIAL_SPECTRUM_MODE			= 0x00102010;
        static const unsigned int OBP_GET_PARTIAL_SPECTRUM_NOW			= 0x00102080;


// Integration time
		static const unsigned int OBP_GET_INTEGRATION_TIME_US			= 0x00110000;
		static const unsigned int OBP_GET_MINIMUM_INTEGRATION_TIME_US	= 0x00110001;
		static const unsigned int OBP_GET_MAXIMUM_INTEGRATION_TIME_US	= 0x00110002;
		static const unsigned int OBP_GET_INTEGRATION_TIME_STEP_SIZE_US	= 0x00110003;
        static const unsigned int OBP_SET_ITIME_USEC					= 0x00110010;

// trigger modes
		static const unsigned int OBP_GET_TRIGGER_MODE					= 0x00110100;
        static const unsigned int OBP_SET_TRIG_MODE						= 0x00110110;
        static const unsigned int OBP_SIMULATE_TRIG_PULSE				= 0x00110120;

// optical detector characteristics
		static const unsigned int OBP_GET_NUMBER_OF_PIXELS			= 0x00110220;
		static const unsigned int OBP_GET_ACTIVE_PIXEL_RANGES		= 0x00110221;
		static const unsigned int OBP_GET_OPTICAL_DARK_PIXEL_RANGES	= 0x00110222;
		static const unsigned int OBP_GET_ELECTRIC_DARK_PIXEL_RANGES= 0x00110223;

// Pixel binning
        static const unsigned int OBP_GET_PIXEL_BINNING_FACTOR      = 0x00110280;
        static const unsigned int OBP_GET_MAX_BINNING_FACTOR        = 0x00110281;
        static const unsigned int OBP_GET_DEFAULT_BINNING_FACTOR    = 0x00110285;
        static const unsigned int OBP_SET_PIXEL_BINNING_FACTOR      = 0x00110290;
        static const unsigned int OBP_SET_DEFAULT_BINNING_FACTOR    = 0x00110295;

// lamp enable
		static const unsigned int OBP_GET_LAMP_ENABLE_STATE			= 0x00110400;
        static const unsigned int OBP_SET_LAMP_ENABLE               = 0x00110410; // LEGACY
		static const unsigned int OBP_SET_LAMP_ENABLE_STATE			= 0x00110410;

// acquisition (trigger) delay
		static const unsigned int OBP_ACQUISITION_DELAY_US			= 0x00110500;
		static const unsigned int OBP_GET_MINIMUM_ACQUISITION_DELAY = 0x00110501;
		static const unsigned int OBP_GET_MAXIMUM_ACQUISITION_DELAY = 0x00110502;
		static const unsigned int OBP_GET_ACQUISITION_DELAY_STEP	= 0x00110503;
        static const unsigned int OBP_SET_TRIG_DELAY_USEC           = 0x00110510;

// shutter control
        static const unsigned int OBP_SET_SHUTTER                   = 0x00110610;

// Spectrum filtering and averaging
        static const unsigned int OBP_GET_SCANS_TO_AVERAGE          = 0x00120000;
        static const unsigned int OBP_SET_SCANS_TO_AVERAGE          = 0x00120010;
        static const unsigned int OBP_GET_BOXCAR_WIDTH              = 0x00121000;
        static const unsigned int OBP_SET_BOXCAR_WIDTH              = 0x00121010;

// Spectral correction and calibration
        static const unsigned int OBP_GET_WL_COEFF_COUNT            = 0x00180100;
        static const unsigned int OBP_GET_WL_COEFF                  = 0x00180101;
        static const unsigned int OBP_SET_WL_COEFF                  = 0x00180111;
        static const unsigned int OBP_GET_NL_COEFF_COUNT            = 0x00181100;
        static const unsigned int OBP_GET_NL_COEFF                  = 0x00181101;
        static const unsigned int OBP_SET_NL_COEFF                  = 0x00181111;
		static const unsigned int OBP_GET_IRRADIANCE_CAL_AT_PIXEL	= 0x00182000;
        static const unsigned int OBP_GET_IRRAD_CAL_ALL             = 0x00182001;
        static const unsigned int OBP_GET_IRRAD_CAL_COUNT           = 0x00182002;
        static const unsigned int OBP_GET_IRRAD_CAL_COLL_AREA       = 0x00182003;
		static const unsigned int OBP_SET_IRRADIANCE_CAL_AT_PIXEL	= 0x00182010;
        static const unsigned int OBP_SET_IRRAD_CAL_ALL             = 0x00182011;
        static const unsigned int OBP_SET_IRRAD_CAL_COLL_AREA       = 0x00182013;
        static const unsigned int OBP_GET_STRAY_COEFF_COUNT         = 0x00183100;
        static const unsigned int OBP_GET_STRAY_COEFF               = 0x00183101;
        static const unsigned int OBP_SET_STRAY_COEFF               = 0x00183111;
        static const unsigned int OBP_SPEC_GET_HOT_PIXEL_INDICES    = 0x00186000;
        static const unsigned int OBP_SPEC_SET_HOT_PIXEL_INDICES    = 0x00186010;

// Optical Bench
        static const unsigned int OBP_GET_BENCH_ID                  = 0x001B0000;
        static const unsigned int OBP_GET_BENCH_SERIAL_NUMBER       = 0x001B0100;
        static const unsigned int OBP_GET_BENCH_SLIT_WIDTH_MICRONS  = 0x001B0200;
		static const unsigned int OBP_SET_BENCH_SLIT_WIDTH_MICRONS	= 0x001B0210;
        static const unsigned int OBP_GET_BENCH_FIBER_DIAM_MICRONS  = 0x001B0300;
        static const unsigned int OBP_GET_BENCH_GRATING             = 0x001B0400;
		static const unsigned int OBP_SET_BENCH_GRATING				= 0x001B0410;
        static const unsigned int OBP_GET_BENCH_FILTER              = 0x001B0500;
		static const unsigned int OBP_SET_BENCH_FILTER				= 0x001B0510;
        static const unsigned int OBP_GET_BENCH_COATING             = 0x001B0600;
		static const unsigned int OBP_GET_DETECTOR_SERIAL_NUMBER	= 0x001B0700;
		static const unsigned int OBP_SET_DETECTOR_SERIAL_NUMBER	= 0x001B0710;
		static const unsigned int OBP_GET_DETECTOR_TYPE				= 0x001C0000;
		static const unsigned int OBP_SET_DETECTOR_TYPE				= 0x001C0010;

// GPIO
        static const unsigned int OBP_GET_GPIO_NUMBER_OF_PINS       = 0x00200000;
        static const unsigned int OBP_GET_GPIO_OUTPUT_ENABLE_VECTOR = 0x00200100;
        static const unsigned int OBP_SET_GPIO_OUTPUT_ENABLE_VECTOR = 0x00200110;
		static const unsigned int OBP_GET_MULTIPLEXER_VALUE			= 0x00200200;
		static const unsigned int OBP_SET_MULTIPLEXER_VALUE			= 0x00200210;
        static const unsigned int OBP_GET_GPIO_VALUE_VECTOR         = 0x00200300;
        static const unsigned int OBP_SET_GPIO_VALUE_VECTOR 	    = 0x00200310;

		static const unsigned int OBP_GET_EGPIO_NUMBER_OF_PINS		= 0x00280000;
		static const unsigned int OBP_GET_EGPIO_AVAILABLE_MODES		= 0x00280001;
		static const unsigned int OBP_GET_EGPIO_CURRENT_MODE		= 0x00280002;
		static const unsigned int OBP_SET_EGPIO_MODE				= 0x00280012;
		static const unsigned int OBP_GET_EGPIO_OUTPUT_VECTOR		= 0x00284000;
		static const unsigned int OBP_GET_EGPIO_VALUE				= 0x00284001;
		static const unsigned int OBP_SET_EGPIO_OUTPUT_VECTOR		= 0x00284010;
		static const unsigned int OBP_SET_EGPIO_VALUE				= 0x00284011;

// Single strobe
		static const unsigned int OBP_SINGLE_STROBE_PULSE_DELAY_US	= 0x00300000;
		static const unsigned int OBP_GET_SINGLE_STROBE_PULSE_WIDTH = 0x00300001;
		static const unsigned int OBP_GET_SINGLE_STROBE_PULSE_ENABLE= 0x00300002;
		static const unsigned int OBP_GET_SINGLE_STROBE_MIN_PULSE_DLY=0x00300003;
		static const unsigned int OBP_GET_SINGLE_STROBE_MAX_PULSE_DLY=0x00300004;
		static const unsigned int OBP_GET_SINGLE_STROBE_PULSE_DLY_STEP=0x00300005;
		static const unsigned int OBP_GET_SINGLE_STROBE_MIN_PULSE_WIDTH=0x00300006;
		static const unsigned int OBP_GET_SINGLE_STROBE_MAX_PULSE_WIDTH=0x00300007;
		static const unsigned int OBP_GET_SINGLE_STROBE_PULSE_WIDTH_STEP=0x00300008;
        static const unsigned int OBP_SET_SINGLE_STROBE_DELAY       = 0x00300010;
        static const unsigned int OBP_SET_SINGLE_STROBE_WIDTH       = 0x00300011;
        static const unsigned int OBP_SET_SINGLE_STROBE_ENABLE      = 0x00300012;

// Continuouse strobe
		static const unsigned int OBP_GET_CONT_STROBE_PERIOD_US		= 0x00310000;
		static const unsigned int OBP_GET_CONT_STROBE_ENABLE_STATE	= 0x00310001;
		static const unsigned int OBP_GET_CONT_STROBE_MIN_PERIOD_US	= 0x00310002;
		static const unsigned int OBP_GET_CONT_STROBE_MAX_PERIOD_US	= 0x00310003;
		static const unsigned int OBP_GET_CONT_STROBE_STEP_SIZE_US	= 0x00310004;
		static const unsigned int OBP_GET_CONT_STROBE_WIDTH_US		= 0x00310005;
        static const unsigned int OBP_SET_CONT_STROBE_PERIOD_US     = 0x00310010;
		static const unsigned int OBP_SET_CONT_STROBE_PERIOD		= 0x00310010; // legacy
        static const unsigned int OBP_SET_CONT_STROBE_ENABLE_US     = 0x00310011;
		static const unsigned int OBP_SET_CONT_STROBE_ENABLE		= 0x00310011; // Legacy

		static const unsigned int OBP_SET_CONT_STROBE_WIDTH_US		= 0x00310015;

// Temperature control and sensing
        static const unsigned int OBP_GET_TEMPERATURE_COUNT         = 0x00400000;
        static const unsigned int OBP_GET_TEMPERATURE               = 0x00400001;
        static const unsigned int OBP_GET_TEMPERATURE_ALL           = 0x00400002;
		static const unsigned int OBP_GET_TEMPERATURE_EXTREMES		= 0x00410000;
        static const unsigned int OBP_GET_TE_ENABLE                 = 0x00420000;
        static const unsigned int OBP_GET_TE_SETPOINT               = 0x00420001;
        static const unsigned int OBP_GET_TE_FAN_ENABLE             = 0x00420002;
        static const unsigned int OBP_GET_TE_IS_STABLE              = 0x00420003;
        static const unsigned int OBP_GET_TE_TEMPERATURE            = 0x00420004;
        static const unsigned int OBP_SET_TE_ENABLE                 = 0x00420010;
        static const unsigned int OBP_SET_TE_SETPOINT               = 0x00420011;
        static const unsigned int OBP_SET_TE_FAN_ENABLE             = 0x00420012;
		static const unsigned int OBP_GET_TEC_DAC_COEFFICIENT_COUNT	= 0x00428000;
		static const unsigned int OBP_GET_TEC_DAC_COEFFICIENT		= 0x00428001;
		static const unsigned int OBP_SET_TEC_DAC_COEFFICIENT		= 0x00428011;

// serial peripheral interface bus
		static const unsigned int OBP_GET_SPI_BUS_COUNT				= 0x00500000;
		static const unsigned int OBP_GET_SPI_CHIP_SELECTS			= 0x00500001;
		static const unsigned int OBP_GET_MAX_SPI_MASTER_CLOCK_HZ	= 0x00500002;
		static const unsigned int OBP_GET_MAX_SPI_SLAVE_CLOCK_HZ	= 0x00500003;
		static const unsigned int OBP_SPI_FULL_DUPLEX_TRANSFER		= 0x00500010;
		static const unsigned int OBP_SET_SPI_CLOCK_LIMIT_HZ		= 0x00500090;
		static const unsigned int OBP_GET_SPI_CONTROLLER_MODE		= 0x00500100;

/// Intra Integrated Circuit bus
		static const unsigned int OBP_GET_I2C_MASTER_BUS_COUNT				= 0x00600000;
		static const unsigned int OBP_READ_12C_MASTER_BUS					= 0x00600010;
		static const unsigned int OBP_WRITE_I2C_MASTER_BUS					= 0x00600020;
		static const unsigned int OBP_SET_I2C_CLOCK_LIMIT_HZ		= 0x00600080;


// light source control
        static const unsigned int OBP_GET_LIGHT_SOURCE_ENABLE       = 0x00810021;
        static const unsigned int OBP_SET_LIGHT_SOURCE_ENABLE       = 0x00810031;
        static const unsigned int OBP_GET_LIGHT_SOURCE_INTENSITY    = 0x00810041;
        static const unsigned int OBP_SET_LIGHT_SOURCE_INTENSITY    = 0x00810051;
    };
  }
}

#endif /* OBPMESSAGETYPES_H */
