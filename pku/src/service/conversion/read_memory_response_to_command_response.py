import pku_driver_pb2
import pku_service_pb2
from conversion import converter
from conversion.errors import read_memory_response_to_command_response_index_error


class ReadMemoryResponseToCommandResponse(converter.Converter[pku_driver_pb2.ReadMemoryResponse, pku_service_pb2.CommandResponse]):
    IDENTIFIER_OFFSET: int = 0
    BUFFER_SIZE_OFFSET: int = 4
    DESCRIPTION_OFFSET: int = 8
    MAC_OFFSET: int = 128
    IP_OFFSET: int = 134
    NETMASK_OFFSET: int = 138
    GATEWAY_OFFSET: int = 142
    DNS_OFFSET: int = 146
    USE_DHCP_OFFSET: int = 150

    DELIMITER: str = ";"

    def convert(self, source: pku_driver_pb2.ReadMemoryResponse) -> pku_service_pb2.CommandResponse:
        destination: pku_service_pb2.CommandResponse = pku_service_pb2.CommandResponse()
        destination.success = source.success
        if not source.success:
            destination.result_text = source.error_message
        else:
            view: memoryview = memoryview(source.data)
            try:
                destination.result_text = (
                    view[self.IDENTIFIER_OFFSET:self.BUFFER_SIZE_OFFSET].decode() + self.DELIMITER
                    + view[self.BUFFER_SIZE_OFFSET:self.DESCRIPTION_OFFSET].decode() + self.DELIMITER
                    + view[self.DESCRIPTION_OFFSET:self.MAC_OFFSET].decode() + self.DELIMITER
                    + view[self.MAC_OFFSET:self.IP_OFFSET].decode() + self.DELIMITER
                    + view[self.IP_OFFSET:self.NETMASK_OFFSET].decode() + self.DELIMITER
                    + view[self.NETMASK_OFFSET:self.GATEWAY_OFFSET].decode() + self.DELIMITER
                    + view[self.GATEWAY_OFFSET:self.DNS_OFFSET].decode() + self.DELIMITER
                    + view[self.DNS_OFFSET:self.USE_DHCP_OFFSET].decode() + self.DELIMITER
                    + view[self.USE_DHCP_OFFSET:].decode()
                )
            except IndexError as exception:
                raise read_memory_response_to_command_response_index_error.ReadMemoryResponseToCommandResponseIndexError(
                    "Main information is incomplete",
                    read_memory_response_to_command_response_index_error.ReadMemoryResponseToCommandResponseIndexError,
                    source.data
                ) from exception
        return destination
