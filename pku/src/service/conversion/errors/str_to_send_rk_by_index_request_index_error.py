import typing
from conversion.errors import str_to_send_rk_by_index_request_error


class StrToSendRkByIndexRequestIndexError(str_to_send_rk_by_index_request_error.StrToSendRkByIndexRequestError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], index: str) -> None:
        super().__init__(message, first_error, index)
