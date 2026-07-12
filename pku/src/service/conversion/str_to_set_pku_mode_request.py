import typing
import pku_driver_pb2
from conversion import converter
from conversion.errors import str_to_set_pku_mode_request_index_error
from conversion.errors import str_to_set_pku_mode_request_mode_error
from conversion.errors import str_to_set_pku_mode_request_no_parameter_error


class StrToSetPkuModeRequest(converter.Converter[str, pku_driver_pb2.SetPkuModeRequest]):
    PKU_INDEX_INDEX: int = 0
    MODE_INDEX: int = 1
    OPERATION_ID_INDEX: int = 2

    DELIMITER: str = ";"

    MINIMUM_PKU_INDEX: int = 1
    MAXIMUM_PKU_INDEX: int = 64

    MINIMUM_MODE: int = 0
    MAXIMUM_MODE: int = 2

    def convert(self, source: str) -> pku_driver_pb2.SetPkuModeRequest:
        destination: pku_driver_pb2.SetPkuModeRequest = pku_driver_pb2.SetPkuModeRequest()
        parameters: typing.List[str] = source.split(self.DELIMITER)
        try:
            destination.pku_index = self.__parse_pku_index(parameters[self.PKU_INDEX_INDEX])
            destination.mode = self.__parse_mode(parameters[self.MODE_INDEX])
            destination.operation_id = parameters[self.OPERATION_ID_INDEX]
        except IndexError as exception:
            raise str_to_set_pku_mode_request_no_parameter_error.StrToSetPkuModeRequestNoParameterError(
                "Not enough parameters", str_to_set_pku_mode_request_no_parameter_error.StrToSetPkuModeRequestNoParameterError
            ) from exception
        return destination

    def __parse_pku_index(self, pku_index: str) -> int:
        result: int
        try:
            result = int(pku_index)
        except ValueError as exception:
            raise str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError(
                "Incorrect index", str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError, pku_index
            ) from exception
        if result < self.MINIMUM_PKU_INDEX or result > self.MAXIMUM_PKU_INDEX:
            raise str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError(
                "Incorrect index", str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError, pku_index
            )
        return result

    def __parse_mode(self, mode: str) -> int:
        result: int
        try:
            result = int(mode)
        except ValueError as exception:
            raise str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError(
                "Incorrect mode", str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError, mode
            ) from exception
        if result < self.MINIMUM_MODE or result > self.MAXIMUM_MODE:
            raise str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError(
                "Incorrect mode", str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError, mode
            )
        return result
