import typing
from conversion.errors import str_to_send_rk_by_index_request_error


class StrToSendRkByIndexRequestNoParameterError(str_to_send_rk_by_index_request_error.StrToSendRkByIndexRequestError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any]) -> None:
        super().__init__(message, first_error, None)
