from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeSpectrumProcessingFeature(SeaBreezeFeature):
    identifier = "spectrum_processing"

    def get_boxcar_width(self):
        raise NotImplementedError("implement in derived class")

    def set_boxcar_width(self, boxcar_width):
        raise NotImplementedError("implement in derived class")

    boxcar_width = property(get_boxcar_width, set_boxcar_width)

    def get_scans_to_average(self):
        raise NotImplementedError("implement in derived class")

    def set_scans_to_average(self, scans_to_average):
        raise NotImplementedError("implement in derived class")

    scans_to_average = property(get_scans_to_average, set_scans_to_average)
