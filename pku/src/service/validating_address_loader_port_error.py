import typing
import validating_address_loader_error


class ValidatingAddressLoaderPortError(validating_address_loader_error.ValidatingAddressLoaderError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], port: str) -> None:
        super().__init__(message, first_error, port)
