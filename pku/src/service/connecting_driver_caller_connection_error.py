import typing
import connecting_driver_caller_error


class ConnectingDriverCallerConnectionError(connecting_driver_caller_error.ConnectingDriverCallerError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any]) -> None:
        super().__init__(message, first_error, None)
