import typing
import txt_address_loader_error


class TXTAddressLoaderParseError(txt_address_loader_error.TXTAddressLoaderError):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], path: str) -> None:
        super().__init__(message, first_error, path)
