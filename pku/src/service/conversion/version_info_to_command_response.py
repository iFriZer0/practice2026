import pku_driver_pb2
import pku_service_pb2
from conversion import converter


class VersionInfoToCommandResponse(converter.Converter[pku_driver_pb2.VersionInfo, pku_service_pb2.CommandResponse]):
    def convert(self, source: pku_driver_pb2.VersionInfo) -> pku_service_pb2.CommandResponse:
        destination: pku_service_pb2.CommandResponse = pku_service_pb2.CommandResponse()
        destination.success = True
        destination.result_text = source.version
        return destination
