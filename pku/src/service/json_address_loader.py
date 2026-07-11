import typing
import json
import address_loader
import json_address_loader_file_not_found_error
import json_address_loader_permission_error
import json_address_loader_is_a_directory_error
import json_address_loader_parse_error


class JSONAddressLoader(address_loader.AddressLoader):
    IP_KEY: str = "ip"
    PORT_KEY: str = "port"

    ip: str
    port: str

    def __init__(self, path: str) -> None:
        try:
            with open(path, "r", encoding="utf-8") as file:
                data: typing.Dict[typing.Any, typing.Any] = json.load(file)
                self.ip = data[self.IP_KEY]
                self.port = data[self.PORT_KEY]
        except FileNotFoundError as exception:
            raise json_address_loader_file_not_found_error.JSONAddressLoaderFileNotFoundError(
                "File was not found",
                json_address_loader_file_not_found_error.JSONAddressLoaderFileNotFoundError,
                path
            ) from exception
        except PermissionError as exception:
            raise json_address_loader_permission_error.JSONAddressLoaderPermissionError(
                "Not enough permissions",
                json_address_loader_permission_error.JSONAddressLoaderPermissionError,
                path
            ) from exception
        except IsADirectoryError as exception:
            raise json_address_loader_is_a_directory_error.JSONAddressLoaderIsADirectoryError(
                "File is a directory",
                json_address_loader_is_a_directory_error.JSONAddressLoaderIsADirectoryError,
                path
            ) from exception
        except (json.JSONDecodeError, KeyError) as exception:
            raise json_address_loader_parse_error.JSONAddressLoaderParseError(
                "Incorrect file format",
                json_address_loader_parse_error.JSONAddressLoaderParseError,
                path
            ) from exception

    def load_ip(self) -> str:
        return self.ip

    def load_port(self) -> str:
        return self.port
