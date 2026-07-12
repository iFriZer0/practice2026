import logging
import pku_driver_pb2
import pku_service_pb2
from conversion import converter
from application import decorate_with_logger


@decorate_with_logger.decorate_with_logger
class VersionInfoToCommandResponse(converter.Converter[pku_driver_pb2.VersionInfo, pku_service_pb2.CommandResponse]):
    logger: logging.Logger

    def convert(self, source: pku_driver_pb2.VersionInfo) -> pku_service_pb2.CommandResponse:
        destination: pku_service_pb2.CommandResponse = pku_service_pb2.CommandResponse()
        destination.success = True
        destination.result_text = source.version
        self.logger.info(f"Success: {destination.success:b}. Result: \"{destination.result_text:s}\".")
        return destination
