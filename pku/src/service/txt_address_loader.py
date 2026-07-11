import validating_address_loader
import txt_address_loader_file_not_found_error
import txt_address_loader_permission_error
import txt_address_loader_is_a_directory_error


class TXTAddressLoader(validating_address_loader.ValidatingAddressLoader):
    ip: str
    port: str

    def __init__(self, path: str) -> None:
        try:
            with open(path, "r", encoding="utf-8") as file:
                self.ip = self._parse_ip(file.readline().rstrip())
                self.port = self._parse_port(file.readline().rstrip())
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

    def load_ip(self) -> str:
        return self.ip

    def load_port(self) -> str:
        return self.port
