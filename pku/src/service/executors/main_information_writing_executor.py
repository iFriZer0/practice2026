import enum
import logging
import pku_service_pb2
from executors import executor
from executors.errors import main_information_writing_executor_call_error
from executors.errors import main_information_writing_executor_conversion_error
from calls import driver_caller
from calls.errors import driver_caller_error
from conversion import converter
from conversion import str_to_write_main_info_request
from conversion import standard_response_to_command_response
from conversion.errors import converter_error
from application import decorate_with_logger
from factory import solution
from factory import simple_creator


@decorate_with_logger.decorate_with_logger
class MainInformationWritingExecutor(executor.Executor[str, pku_service_pb2.CommandResponse]):
    class Converters(enum.Enum):
        STR_TO_WRITE_MAIN_INFO_REQUEST = 1
        STANDARD_RESPONSE_TO_COMMAND_RESPONSE = 2

    converter_solution: solution.Solution[converter.Converter, Converters] = solution.Solution({
        Converters.STR_TO_WRITE_MAIN_INFO_REQUEST: lambda: simple_creator.SimpleCreator(
            str_to_write_main_info_request.StrToWriteMainInfoRequest
        ),
        Converters.STANDARD_RESPONSE_TO_COMMAND_RESPONSE: lambda: simple_creator.SimpleCreator(
            standard_response_to_command_response.StandardResponseToCommandResponse
        )
    })

    logger: logging.Logger

    caller: driver_caller.DriverCaller

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.caller = caller

    def execute(self, data: str) -> pku_service_pb2.CommandResponse:
        try:
            self.logger.info(f"Parameter \"{data:s}\" received.")
            return self.converter_solution.make(
                self.Converters.STANDARD_RESPONSE_TO_COMMAND_RESPONSE
            ).create().convert(
                self.caller.write_main_information(
                    self.converter_solution.make(self.Converters.STR_TO_WRITE_MAIN_INFO_REQUEST).create().convert(data)
                )
            )
        except driver_caller_error.DriverCallerError as exception:
            raise main_information_writing_executor_call_error.MainInformationWritingExecutorCallError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
        except converter_error.ConverterError as exception:
            raise main_information_writing_executor_conversion_error.MainInformationWritingExecutorConversionError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
