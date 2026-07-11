import typing
import json_address_loader_error


class JSONAddressLoaderParseError(json_address_loader_error.JSONAddressLoaderError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], path: str) -> None:
        super().__init__(message, first_error, path)
