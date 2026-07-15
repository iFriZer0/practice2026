import typing
import logging
import pku_driver_pb2
from conversion import converter
from conversion.errors import str_to_write_main_info_request_no_parameter_error
from application import decorate_with_logger


@decorate_with_logger.decorate_with_logger
class StrToWriteMainInfoRequest(converter.Converter[str, pku_driver_pb2.WriteMainInfoRequest]):
    DESCRIPTION_INDEX: int = 0
    MAC_INDEX: int = 1
    IP_INDEX: int = 2
    NETMASK_INDEX: int = 3
    GATEWAY_INDEX: int = 4
    DNS_INDEX: int = 5
    USE_DHCP_INDEX: int = 6
    OPERATION_ID_INDEX: int = 7

    DELIMITER: str = ";"

    TRUE_STRING: str = "1"

    DESCRIPTION_SIZE: int = 120

    logger: logging.Logger

    def convert(self, source: str) -> pku_driver_pb2.WriteMainInfoRequest:
        destination: pku_driver_pb2.WriteMainInfoRequest = pku_driver_pb2.WriteMainInfoRequest()
        parameters: typing.List[str] = source.split(self.DELIMITER)
        try:
            destination.description = (
                parameters[self.DESCRIPTION_INDEX] + "\0" * max(0, (self.DESCRIPTION_SIZE - len(parameters[self.DESCRIPTION_INDEX])))
            )
            destination.mac = parameters[self.MAC_INDEX]
            destination.ip = parameters[self.IP_INDEX]
            destination.netmask = parameters[self.NETMASK_INDEX]
            destination.gateway = parameters[self.GATEWAY_INDEX]
            destination.dns = parameters[self.DNS_INDEX]
            destination.use_dhcp = parameters[self.USE_DHCP_INDEX] == "1"
            destination.operation_id = parameters[self.OPERATION_ID_INDEX]
        except IndexError as exception:
            self.logger.error("Not enough parameters.")
            raise str_to_write_main_info_request_no_parameter_error.StrToWriteMainInfoRequestNoParameterError(
                "Not enough parameters",
                str_to_write_main_info_request_no_parameter_error.StrToWriteMainInfoRequestNoParameterError,
                source
            ) from exception
        self.logger.info(
            f"Description: \"{destination.description:s}\". "
            f"MAC: \"{destination.mac:s}\". "
            f"IP: \"{destination.ip:s}\". "
            f"Netmask: \"{destination.netmask:s}\". "
            f"Gateway: \"{destination.gateway:s}\". "
            f"DNS: \"{destination.dns:s}\". "
            f"DHCP: \"{destination.use_dhcp:b}\". "
            f"Operation identifier: \"{destination.operation_id:s}\"."
        )
        return destination
