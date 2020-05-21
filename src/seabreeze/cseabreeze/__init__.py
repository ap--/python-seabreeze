"""This is a wrapper for the c-implementation of the seabreeze-library

"""
# noinspection PyUnresolvedReferences,PyProtectedMember
from seabreeze.cseabreeze._wrapper import (
    SeaBreezeAcquisitionDelayFeature,
    SeaBreezeAPI,
    SeaBreezeContinuousStrobeFeature,
    SeaBreezeDataBufferFeature,
    SeaBreezeDevice,
    SeaBreezeDHCPServerFeature,
    SeaBreezeEEPROMFeature,
    SeaBreezeError,
    SeaBreezeEthernetConfigurationFeature,
    SeaBreezeFastBufferFeature,
    SeaBreezeFeature,
    SeaBreezeGPIOFeature,
    SeaBreezeI2CMasterFeature,
    SeaBreezeIntrospectionFeature,
    SeaBreezeIPv4Feature,
    SeaBreezeIrradCalFeature,
    SeaBreezeLightSourceFeature,
    SeaBreezeMulticastFeature,
    SeaBreezeNetworkConfigurationFeature,
    SeaBreezeNonlinearityCoefficientsFeature,
    SeaBreezeNumFeaturesError,
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

_backend_ = "cseabreeze"
