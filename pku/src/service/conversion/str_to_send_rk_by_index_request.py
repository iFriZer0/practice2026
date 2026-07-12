import logging
import typing
import pku_driver_pb2
from conversion import converter
from conversion.errors import str_to_send_rk_by_index_request_index_error
from conversion.errors import str_to_send_rk_by_index_request_duration_error
from conversion.errors import str_to_send_rk_by_index_request_no_parameter_error
from application import decorate_with_logger


@decorate_with_logger.decorate_with_logger
class StrToSendRkByIndexRequest(converter.Converter[str, pku_driver_pb2.SendRkByIndexRequest]):
    RK_INDEX_INDEX: int = 0
    DURATION_MS_INDEX: int = 1
    OPERATION_ID_INDEX: int = 2

    DELIMITER: str = ";"

    MINIMUM_RK_INDEX: int = 1
    MAXIMUM_RK_INDEX: int = 48

    MINIMUM_DURATION_MS: int = 0

    logger: logging.Logger

    def convert(self, source: str) -> pku_driver_pb2.SendRkByIndexRequest:
        destination: pku_driver_pb2.SendRkByIndexRequest = pku_driver_pb2.SendRkByIndexRequest()
        parameters: typing.List[str] = source.split(self.DELIMITER)
        try:
            destination.rk_index = self.__parse_rk_index(parameters[self.RK_INDEX_INDEX])
            destination.duration_ms = self.__parse_duration_ms(parameters[self.DURATION_MS_INDEX])
            destination.operation_id = parameters[self.OPERATION_ID_INDEX]
        except IndexError as exception:
            self.logger.error("Not enough parameters.")
            raise str_to_send_rk_by_index_request_no_parameter_error.StrToSendRkByIndexRequestNoParameterError(
                "Not enough parameters", str_to_send_rk_by_index_request_no_parameter_error.StrToSendRkByIndexRequestNoParameterError
            ) from exception
        self.logger.info(
            f"Index: {destination.rk_index:d}. "
            f"Duration: {destination.duration_ms:d} ms. "
            f"Operation identifier: \"{destination.operation_id:s}\"."
        )
        return destination

    def __parse_rk_index(self, rk_index: str) -> int:
        result: int
        try:
            result = int(rk_index)
        except ValueError as exception:
            self.logger.error(f"Incorrect index \"{rk_index:s}\".")
            raise str_to_send_rk_by_index_request_index_error.StrToSendRkByIndexRequestIndexError(
                "Incorrect index", str_to_send_rk_by_index_request_index_error.StrToSendRkByIndexRequestIndexError, rk_index
            ) from exception
        if result < self.MINIMUM_RK_INDEX or result > self.MAXIMUM_RK_INDEX:
            self.logger.error(f"Incorrect index \"{rk_index:s}\".")
            raise str_to_send_rk_by_index_request_index_error.StrToSendRkByIndexRequestIndexError(
                "Incorrect index", str_to_send_rk_by_index_request_index_error.StrToSendRkByIndexRequestIndexError, rk_index
            )
        return result

    def __parse_duration_ms(self, duration_ms: str) -> int:
        result: int
        try:
            result = int(duration_ms)
        except ValueError as exception:
            self.logger.error(f"Incorrect duration \"{duration_ms:s}\".")
            raise str_to_send_rk_by_index_request_duration_error.StrToSendRkByIndexRequestDurationError(
                "Incorrect duration",
                str_to_send_rk_by_index_request_duration_error.StrToSendRkByIndexRequestDurationError,
                duration_ms
            ) from exception
        if result < self.MINIMUM_DURATION_MS:
            self.logger.error(f"Incorrect duration \"{duration_ms:s}\".")
            raise str_to_send_rk_by_index_request_duration_error.StrToSendRkByIndexRequestDurationError(
                "Incorrect duration",
                str_to_send_rk_by_index_request_duration_error.StrToSendRkByIndexRequestDurationError,
                duration_ms
            )
        return result
