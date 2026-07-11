import abc
import pku_service_pb2_grpc


class MainServiceServicerBuilder(abc.ABC):
    @abc.abstractmethod
    def build_servicer(self) -> pku_service_pb2_grpc.MainServiceServicer:
        pass
