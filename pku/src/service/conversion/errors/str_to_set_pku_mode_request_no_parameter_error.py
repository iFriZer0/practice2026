import typing
from conversion.errors import str_to_set_pku_mode_request_error


class StrToSetPkuModeRequestNoParameterError(str_to_set_pku_mode_request_error.StrToSetPkuModeRequestError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any]) -> None:
        super().__init__(message, first_error, None)
