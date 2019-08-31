"""This is a wrapper for the pyusb-implementation of the seabreeze-library

"""
from seabreeze.pyseabreeze.api import SeaBreezeAPI
from seabreeze.pyseabreeze.exceptions import (SeaBreezeError,
                                              SeaBreezeNumFeaturesError)
from seabreeze.pyseabreeze.devices import SeaBreezeDevice
# noinspection PyUnresolvedReferences,PyProtectedMember
from seabreeze.pyseabreeze.features._base import SeaBreezeFeature
# noinspection PyUnresolvedReferences
from seabreeze.pyseabreeze.features import (SeaBreezeRawUSBBusAccessFeature,
                                            SeaBreezeSpectrometerFeature,
                                            SeaBreezePixelBinningFeature,
                                            SeaBreezeThermoElectricFeature,
                                            SeaBreezeIrradCalFeature,
                                            SeaBreezeEthernetConfigurationFeature,
                                            SeaBreezeMulticastFeature,
                                            SeaBreezeIPv4Feature,
                                            SeaBreezeDHCPServerFeature,
                                            SeaBreezeNetworkConfigurationFeature,
                                            SeaBreezeWifiConfigurationFeature,
                                            SeaBreezeGPIOFeature,
                                            SeaBreezeEEPROMFeature,
                                            SeaBreezeLightSourceFeature,
                                            SeaBreezeStrobeLampFeature,
                                            SeaBreezeContinuousStrobeFeature,
                                            SeaBreezeShutterFeature,
                                            SeaBreezeNonlinearityCoefficientsFeature,
                                            SeaBreezeTemperatureFeature,
                                            SeaBreezeIntrospectionFeature,
                                            SeaBreezeSpectrumProcessingFeature,
                                            SeaBreezeRevisionFeature,
                                            SeaBreezeOpticalBenchFeature,
                                            SeaBreezeStrayLightCoefficientsFeature,
                                            SeaBreezeDataBufferFeature,
                                            SeaBreezeFastBufferFeature,
                                            SeaBreezeAcquisitionDelayFeature,
                                            SeaBreezeI2CMasterFeature)
_backend_ = 'pyseabreeze'
