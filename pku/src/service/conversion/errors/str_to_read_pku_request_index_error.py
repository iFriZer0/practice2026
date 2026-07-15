import typing
from conversion.errors import str_to_read_pku_request_error


class StrToReadPkuRequestIndexError(str_to_read_pku_request_error.StrToReadPkuRequestError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any]) -> None:
        super().__init__(message, first_error, None)
