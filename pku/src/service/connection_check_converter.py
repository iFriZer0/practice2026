import pku_driver_pb2
import pku_service_pb2
from conversion import converter


class ConnectionCheckConverter(converter.Converter[pku_driver_pb2.StandardResponse, pku_service_pb2.CommandResponse]):
    def convert_to_destination(self, source: pku_driver_pb2.StandardResponse) -> pku_service_pb2.CommandResponse:
        destination: pku_service_pb2.CommandResponse = pku_service_pb2.CommandResponse()
        destination.success = source.success
        destination.result_text = source.error_message
        return destination

    def convert_to_source(self, destination: pku_service_pb2.CommandResponse) -> pku_driver_pb2.StandardResponse:
        source: pku_driver_pb2.StandardResponse = pku_driver_pb2.StandardResponse()
        source.success = destination.success
        source.error_message = destination.result_text
        return source
