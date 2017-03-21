from .common import SeaBreezeError, get_pyseabreeze_decorator
from .communication import USBCommOBP

import struct

convert_exceptions = get_pyseabreeze_decorator('interfaces.spectrumprocessing')


class SpectrumProcessingFeatureOBP(USBCommOBP):

    def has_spectrum_processing_feature(self):
        return [True]

    # TODO: implement the feature for STS and Spark
