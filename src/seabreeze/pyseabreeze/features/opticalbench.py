from seabreeze.pyseabreeze.features._base import SeaBreezeFeature


# Definition
# ==========
#
# TODO: This feature needs to be implemented for pyseabreeze
#
class SeaBreezeOpticalBenchFeature(SeaBreezeFeature):
    identifier = "optical_bench"

    def get_fiber_diameter_microns(self):
        raise NotImplementedError("implement in derived class")

    def get_slit_width_microns(self):
        raise NotImplementedError("implement in derived class")

    def get_id(self):
        raise NotImplementedError("implement in derived class")

    def get_serial_number(self):
        raise NotImplementedError("implement in derived class")

    def get_coating(self):
        raise NotImplementedError("implement in derived class")

    def get_filter(self):
        raise NotImplementedError("implement in derived class")

    def get_grating(self):
        raise NotImplementedError("implement in derived class")
