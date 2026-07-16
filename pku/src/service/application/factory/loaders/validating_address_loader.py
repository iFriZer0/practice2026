import ipaddress
from application.factory.loaders import address_loader
from application.factory.loaders.errors import validating_address_loader_ip_error
from application.factory.loaders.errors import validating_address_loader_port_error


class ValidatingAddressLoader(address_loader.AddressLoader):
    MINIMUM_PORT: int = 0
    MAXIMUM_PORT: int = 65535

    def _parse_ip(self, line: str) -> str:
        try:
            if line != "localhost":
                ipaddress.ip_address(line)
        except ValueError as exception:
            raise validating_address_loader_ip_error.ValidatingAddressLoaderIPError(
                "Incorrect IP",
                validating_address_loader_ip_error.ValidatingAddressLoaderIPError,
                line
            ) from exception
        return line

    def _parse_port(self, line: str) -> str:
        port: int
        try:
            port = int(line)
        except (ValueError, TypeError) as exception:
            raise validating_address_loader_port_error.ValidatingAddressLoaderPortError(
                "Incorrect port",
                validating_address_loader_port_error.ValidatingAddressLoaderPortError,
                line
            ) from exception
        if port < self.MINIMUM_PORT or self.MAXIMUM_PORT < port:
            raise validating_address_loader_port_error.ValidatingAddressLoaderPortError(
                "Incorrect port",
                validating_address_loader_port_error.ValidatingAddressLoaderPortError,
                line
            )
        return line
