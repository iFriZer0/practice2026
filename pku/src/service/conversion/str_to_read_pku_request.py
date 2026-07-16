import logging
import typing
import collections
import pku_driver_pb2
from conversion import converter
from conversion.errors import str_to_read_pku_request_no_parameter_error
from conversion.errors import str_to_read_pku_request_index_error
from application import decorate_with_logger


@decorate_with_logger.decorate_with_logger
class StrToReadPkuRequest(converter.Converter[str, pku_driver_pb2.ReadPkuRequest]):
    DELIMITER: str = ";"

    logger: logging.Logger

    def convert(self, source: str) -> pku_driver_pb2.ReadPkuRequest:
        destination: pku_driver_pb2.ReadPkuRequest = pku_driver_pb2.ReadPkuRequest()
        parameters: typing.List[str] = source.split(self.DELIMITER)
        try:
            destination.indices.extend(self.__parse_indices(parameters))
            destination.operation_id = parameters[-1]
        except ValueError as exception:
            self.logger.error("Incorrect index.")
            raise str_to_read_pku_request_index_error.StrToReadPkuRequestIndexError(
                "Incorrect index", str_to_read_pku_request_index_error.StrToReadPkuRequestIndexError
            ) from exception
        except IndexError as exception:
            self.logger.error("No parameters.")
            raise str_to_read_pku_request_no_parameter_error.StrToReadPkuRequestNoParameterError(
                "No parameters", str_to_read_pku_request_no_parameter_error.StrToReadPkuRequestNoParameterError
            ) from exception
        self.logger.info(
            "Indices: {:s}. Operation identifier: \"{:s}\".".format(
                ", ".join([str(index) for index in destination.indices]),
                destination.operation_id
            )
        )
        return destination

    def __parse_indices(self, parameters: collections.abc.Sequence[str]) -> typing.List[int]:
        indices: typing.List[int] = [int(parameters[index]) for index in range(len(parameters) - 1)]
        if any(index < 0 for index in indices):
            self.logger.error("Negative index")
            raise str_to_read_pku_request_index_error.StrToReadPkuRequestIndexError(
                "Negative index", str_to_read_pku_request_index_error.StrToReadPkuRequestIndexError
            )
        return indices
