import pku_service_pb2_grpc
import main_service_servicer_builder
from factory import creator


class MainServiceServicerDirector(creator.Creator[pku_service_pb2_grpc.MainServiceServicer]):
    def create(self, builder: main_service_servicer_builder.MainServiceServicerBuilder) -> pku_service_pb2_grpc.MainServiceServicer:
        return builder.build_servicer()
