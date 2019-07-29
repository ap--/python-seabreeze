
from seabreeze.pyseabreeze.exceptions import SeaBreezeError
from seabreeze.pyseabreeze.communication import USBCommBase


class SeaBreezeFeature(object):

    identifier = "base_feature"
    required = False
    required_interface_cls = USBCommBase
    required_features = ()

    def __init__(self, device, feature_id):
        """SeaBreezeFeature base class

        Parameters
        ----------
        device : seabreeze.pyseabreeze.devices.SeaBreezeDevice
        feature_id : int
        """
        if self.identifier == "base_feature":
            raise SeaBreezeError("Don't instantiate SeaBreezeFeature directly. Use derived feature classes.")
        if not isinstance(device, self.required_interface_cls):
            raise SeaBreezeError("Feature can't be instantiated for device. Device not instance of {}".format(
                str(self.required_interface_cls)
            ))
        self.device = device
        self.feature_id = feature_id

    def __repr__(self):
        return "<{}:{}:{} id={}>".format(self.__class__.__name__,
                                         self.device.model, self.device.serial_number, self.feature_id)

    @classmethod
    def get_feature_class_registry(cls):
        # noinspection PyUnresolvedReferences
        return {feature_class.identifier: feature_class for feature_class in SeaBreezeFeature.__subclasses__()}
