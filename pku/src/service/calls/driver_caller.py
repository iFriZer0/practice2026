import abc
import pku_driver_pb2


class DriverCaller(abc.ABC):
    @abc.abstractmethod
    def check_connection(self) -> pku_driver_pb2.StandardResponse:
        pass

    @abc.abstractmethod
    def get_hardware_status(self) -> pku_driver_pb2.StandardResponse:
        pass

    @abc.abstractmethod
    def read_main_information(self) -> pku_driver_pb2.ReadMemoryResponse:
        pass

    @abc.abstractmethod
    def write_main_information(self, request: pku_driver_pb2.WriteMainInfoRequest) -> pku_driver_pb2.StandardResponse:
        pass

    @abc.abstractmethod
    def read_pku(self, request: pku_driver_pb2.ReadPkuRequest) -> pku_driver_pb2.ReadPkuResponse:
        pass

    @abc.abstractmethod
    def set_pku_mode(self, request: pku_driver_pb2.SetPkuModeRequest) -> pku_driver_pb2.StandardResponse:
        pass

    @abc.abstractmethod
    def send_rk_by_index(self, request: pku_driver_pb2.SendRkByIndexRequest) -> pku_driver_pb2.StandardResponse:
        pass

    @abc.abstractmethod
    def get_version(self) -> pku_driver_pb2.VersionInfo:
        pass
