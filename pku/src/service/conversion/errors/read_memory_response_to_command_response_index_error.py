import typing
from conversion.errors import read_memory_response_to_command_response_error


class ReadMemoryResponseToCommandResponseIndexError(
    read_memory_response_to_command_response_error.ReadMemoryResponseToCommandResponseError
):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], data: bytes) -> None:
        super().__init__(message, first_error, data)
