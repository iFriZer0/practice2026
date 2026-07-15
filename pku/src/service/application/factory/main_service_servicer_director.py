import pku_service_pb2_grpc
from application.factory import main_service_servicer_builder
from application.factory.errors import main_service_servicer_builder_error
from application.factory.errors import main_service_servicer_director_build_error
from factory import creator


class MainServiceServicerDirector(creator.Creator[pku_service_pb2_grpc.MainServiceServicer]):
    def create(self, builder: main_service_servicer_builder.MainServiceServicerBuilder) -> pku_service_pb2_grpc.MainServiceServicer:
        try:
            return builder.build_servicer()
        except main_service_servicer_builder_error.MainServiceServicerBuilderError as exception:
            raise main_service_servicer_director_build_error.MainServiceServicerDirectorBuildError(
                str(exception), exception.get_first_error(), exception.get_data()
            )
