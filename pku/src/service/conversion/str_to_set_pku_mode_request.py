import typing
import logging
import pku_driver_pb2
from conversion import converter
from conversion.errors import str_to_set_pku_mode_request_index_error
from conversion.errors import str_to_set_pku_mode_request_mode_error
from conversion.errors import str_to_set_pku_mode_request_no_parameter_error
from application import decorate_with_logger


@decorate_with_logger.decorate_with_logger
class StrToSetPkuModeRequest(converter.Converter[str, pku_driver_pb2.SetPkuModeRequest]):
    PKU_INDEX_INDEX: int = 0
    MODE_INDEX: int = 1
    OPERATION_ID_INDEX: int = 2

    DELIMITER: str = ";"

    MINIMUM_PKU_INDEX: int = 0

    MINIMUM_MODE: int = 0

    logger: logging.Logger

    def convert(self, source: str) -> pku_driver_pb2.SetPkuModeRequest:
        destination: pku_driver_pb2.SetPkuModeRequest = pku_driver_pb2.SetPkuModeRequest()
        parameters: typing.List[str] = source.split(self.DELIMITER)
        try:
            destination.pku_index = self.__parse_pku_index(parameters[self.PKU_INDEX_INDEX])
            destination.mode = self.__parse_mode(parameters[self.MODE_INDEX])
            destination.operation_id = parameters[self.OPERATION_ID_INDEX]
        except IndexError as exception:
            self.logger.error("Not enough parameters.")
            raise str_to_set_pku_mode_request_no_parameter_error.StrToSetPkuModeRequestNoParameterError(
                "Not enough parameters", str_to_set_pku_mode_request_no_parameter_error.StrToSetPkuModeRequestNoParameterError
            ) from exception
        self.logger.info(
            f"Index: {destination.pku_index:d}. Mode: {destination.mode:d}. Operation identifier: \"{destination.operation_id:s}\"."
        )
        return destination

    def __parse_pku_index(self, pku_index: str) -> int:
        result: int
        try:
            result = int(pku_index)
        except ValueError as exception:
            self.logger.error(f"Incorrect index \"{pku_index:s}\".")
            raise str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError(
                "Incorrect index", str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError, pku_index
            ) from exception
        if result < self.MINIMUM_PKU_INDEX:
            self.logger.error(f"Incorrect index \"{pku_index:s}\".")
            raise str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError(
                "Incorrect index", str_to_set_pku_mode_request_index_error.StrToSetPkuModeRequestIndexError, pku_index
            )
        return result

    def __parse_mode(self, mode: str) -> int:
        result: int
        try:
            result = int(mode)
        except ValueError as exception:
            self.logger.error(f"Incorrect mode \"{mode:s}\".")
            raise str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError(
                "Incorrect mode", str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError, mode
            ) from exception
        if result < self.MINIMUM_MODE:
            self.logger.error(f"Incorrect mode \"{mode:s}\".")
            raise str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError(
                "Incorrect mode", str_to_set_pku_mode_request_mode_error.StrToSetPkuModeRequestModeError, mode
            )
        return result
