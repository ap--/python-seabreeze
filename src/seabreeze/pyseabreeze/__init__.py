"""This is a wrapper for the pyusb-implementation of the seabreeze-library

"""
from seabreeze.pyseabreeze.api import SeaBreezeAPI
from seabreeze.pyseabreeze.devices import SeaBreezeDevice
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.exceptions import SeaBreezeNumFeaturesError

# noinspection PyUnresolvedReferences
from seabreeze.pyseabreeze.features import SeaBreezeAcquisitionDelayFeature
from seabreeze.pyseabreeze.features import SeaBreezeContinuousStrobeFeature
from seabreeze.pyseabreeze.features import SeaBreezeDataBufferFeature
from seabreeze.pyseabreeze.features import SeaBreezeDHCPServerFeature
from seabreeze.pyseabreeze.features import SeaBreezeEEPROMFeature
from seabreeze.pyseabreeze.features import SeaBreezeEthernetConfigurationFeature
from seabreeze.pyseabreeze.features import SeaBreezeFastBufferFeature
from seabreeze.pyseabreeze.features import SeaBreezeGPIOFeature
from seabreeze.pyseabreeze.features import SeaBreezeI2CMasterFeature
from seabreeze.pyseabreeze.features import SeaBreezeIntrospectionFeature
from seabreeze.pyseabreeze.features import SeaBreezeIPv4Feature
from seabreeze.pyseabreeze.features import SeaBreezeIrradCalFeature
from seabreeze.pyseabreeze.features import SeaBreezeLightSourceFeature
from seabreeze.pyseabreeze.features import SeaBreezeMulticastFeature
from seabreeze.pyseabreeze.features import SeaBreezeNetworkConfigurationFeature
from seabreeze.pyseabreeze.features import SeaBreezeNonlinearityCoefficientsFeature
from seabreeze.pyseabreeze.features import SeaBreezeOpticalBenchFeature
from seabreeze.pyseabreeze.features import SeaBreezePixelBinningFeature
from seabreeze.pyseabreeze.features import SeaBreezeRawUSBBusAccessFeature
from seabreeze.pyseabreeze.features import SeaBreezeRevisionFeature
from seabreeze.pyseabreeze.features import SeaBreezeShutterFeature
from seabreeze.pyseabreeze.features import SeaBreezeSpectrometerFeature
from seabreeze.pyseabreeze.features import SeaBreezeSpectrumProcessingFeature
from seabreeze.pyseabreeze.features import SeaBreezeStrayLightCoefficientsFeature
from seabreeze.pyseabreeze.features import SeaBreezeStrobeLampFeature
from seabreeze.pyseabreeze.features import SeaBreezeTemperatureFeature
from seabreeze.pyseabreeze.features import SeaBreezeThermoElectricFeature
from seabreeze.pyseabreeze.features import SeaBreezeWifiConfigurationFeature

# noinspection PyUnresolvedReferences,PyProtectedMember
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature

__all__ = [
    "_backend_",
    "SeaBreezeAcquisitionDelayFeature",
    "SeaBreezeAPI",
    "SeaBreezeContinuousStrobeFeature",
    "SeaBreezeDataBufferFeature",
    "SeaBreezeDevice",
    "SeaBreezeDHCPServerFeature",
    "SeaBreezeEEPROMFeature",
    "SeaBreezeError",
    "SeaBreezeEthernetConfigurationFeature",
    "SeaBreezeFastBufferFeature",
    "SeaBreezeFeature",
    "SeaBreezeGPIOFeature",
    "SeaBreezeI2CMasterFeature",
    "SeaBreezeIntrospectionFeature",
    "SeaBreezeIPv4Feature",
    "SeaBreezeIrradCalFeature",
    "SeaBreezeLightSourceFeature",
    "SeaBreezeMulticastFeature",
    "SeaBreezeNetworkConfigurationFeature",
    "SeaBreezeNonlinearityCoefficientsFeature",
    "SeaBreezeNumFeaturesError",
    "SeaBreezeOpticalBenchFeature",
    "SeaBreezePixelBinningFeature",
    "SeaBreezeRawUSBBusAccessFeature",
    "SeaBreezeRevisionFeature",
    "SeaBreezeShutterFeature",
    "SeaBreezeSpectrometerFeature",
    "SeaBreezeSpectrumProcessingFeature",
    "SeaBreezeStrayLightCoefficientsFeature",
    "SeaBreezeStrobeLampFeature",
    "SeaBreezeTemperatureFeature",
    "SeaBreezeThermoElectricFeature",
    "SeaBreezeWifiConfigurationFeature",
]

_backend_ = "pyseabreeze"
