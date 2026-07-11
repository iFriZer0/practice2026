import ipaddress
import address_loader
import txt_address_loader_file_not_found_error
import txt_address_loader_permission_error
import txt_address_loader_is_a_directory_error
import txt_address_loader_parse_error


class TXTAddressLoader(address_loader.AddressLoader):
    MINIMUM_PORT: int = 0
    MAXIMUM_PORT: int = 65535

    ip: str
    port: str

    def __init__(self, path: str) -> None:
        try:
            with open(path, "r", encoding="utf-8") as file:
                self.__parse_ip(file.readline().rstrip(), path)
                self.__parse_port(file.readline().rstrip(), path)
        except FileNotFoundError as exception:
            raise txt_address_loader_file_not_found_error.TXTAddressLoaderFileNotFoundError(
                "File was not found",
                txt_address_loader_file_not_found_error.TXTAddressLoaderFileNotFoundError,
                path
            ) from exception
        except PermissionError as exception:
            raise txt_address_loader_permission_error.TXTAddressLoaderPermissionError(
                "Not enough permissions",
                txt_address_loader_permission_error.TXTAddressLoaderPermissionError,
                path
            ) from exception
        except IsADirectoryError as exception:
            raise txt_address_loader_is_a_directory_error.TXTAddressLoaderIsADirectoryError(
                "File is a directory",
                txt_address_loader_is_a_directory_error.TXTAddressLoaderIsADirectoryError,
                path
            ) from exception
        except ValueError as exception:
            raise txt_address_loader_parse_error.TXTAddressLoaderParseError(
                "Incorrect file format",
                txt_address_loader_parse_error.TXTAddressLoaderParseError,
                path
            ) from exception

    def load_ip(self) -> str:
        return self.ip

    def load_port(self) -> str:
        return self.port

    def __parse_ip(self, line: str, path: str) -> None:
        try:
            ipaddress.ip_address(line)
        except ValueError as exception:
            raise txt_address_loader_parse_error.TXTAddressLoaderParseError(
                "Incorrect file format",
                txt_address_loader_parse_error.TXTAddressLoaderParseError,
                path
            ) from exception
        self.ip = line

    def __parse_port(self, line: str, path: str) -> None:
        try:
            port: int = int(line)
            if port < self.MINIMUM_PORT or self.MAXIMUM_PORT < port:
                print(line)
                print(1)
                raise txt_address_loader_parse_error.TXTAddressLoaderParseError(
                    "Incorrect file format",
                    txt_address_loader_parse_error.TXTAddressLoaderParseError,
                    path
                )
        except (ValueError, TypeError) as exception:
            raise txt_address_loader_parse_error.TXTAddressLoaderParseError(
                "Incorrect file format",
                txt_address_loader_parse_error.TXTAddressLoaderParseError,
                path
            ) from exception
        self.port = line
