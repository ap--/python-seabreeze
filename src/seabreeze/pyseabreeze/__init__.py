"""This is a wrapper for the pyusb-implementation of the seabreeze-library

"""
from seabreeze.pyseabreeze.api import SeaBreezeAPI
from seabreeze.pyseabreeze.devices import SeaBreezeDevice
from seabreeze.pyseabreeze.exceptions import SeaBreezeError, SeaBreezeNumFeaturesError

# noinspection PyUnresolvedReferences
from seabreeze.pyseabreeze.features import (
    SeaBreezeAcquisitionDelayFeature,
    SeaBreezeContinuousStrobeFeature,
    SeaBreezeDataBufferFeature,
    SeaBreezeDHCPServerFeature,
    SeaBreezeEEPROMFeature,
    SeaBreezeEthernetConfigurationFeature,
    SeaBreezeFastBufferFeature,
    SeaBreezeGPIOFeature,
    SeaBreezeI2CMasterFeature,
    SeaBreezeIntrospectionFeature,
    SeaBreezeIPv4Feature,
    SeaBreezeIrradCalFeature,
    SeaBreezeLightSourceFeature,
    SeaBreezeMulticastFeature,
    SeaBreezeNetworkConfigurationFeature,
    SeaBreezeNonlinearityCoefficientsFeature,
    SeaBreezeOpticalBenchFeature,
    SeaBreezePixelBinningFeature,
    SeaBreezeRawUSBBusAccessFeature,
    SeaBreezeRevisionFeature,
    SeaBreezeShutterFeature,
    SeaBreezeSpectrometerFeature,
    SeaBreezeSpectrumProcessingFeature,
    SeaBreezeStrayLightCoefficientsFeature,
    SeaBreezeStrobeLampFeature,
    SeaBreezeTemperatureFeature,
    SeaBreezeThermoElectricFeature,
    SeaBreezeWifiConfigurationFeature,
)

# noinspection PyUnresolvedReferences,PyProtectedMember
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature

_backend_ = "pyseabreeze"
