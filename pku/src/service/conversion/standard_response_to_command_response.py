import pku_driver_pb2
import pku_service_pb2
from conversion import converter


class StandardResponseToCommandResponse(converter.Converter[pku_driver_pb2.StandardResponse, pku_service_pb2.CommandResponse]):
    def convert(self, source: pku_driver_pb2.StandardResponse) -> pku_service_pb2.CommandResponse:
        destination: pku_service_pb2.CommandResponse = pku_service_pb2.CommandResponse()
        destination.success = source.success
        destination.result_text = source.error_message
        return destination
