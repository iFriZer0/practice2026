import typing
from application.factory.loaders.errors import validating_address_loader_error


class ValidatingAddressLoaderIPError(validating_address_loader_error.ValidatingAddressLoaderError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], ip: str) -> None:
        super().__init__(message, first_error, ip)
