import typing
from conversion.errors import str_to_write_main_info_request_error


class StrToWriteMainInfoRequestNoParameterError(str_to_write_main_info_request_error.StrToWriteMainInfoRequestError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], parameters: str) -> None:
        super().__init__(message, first_error, parameters)
