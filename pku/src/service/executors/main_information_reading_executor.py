import enum
import pku_service_pb2
from executors import executor
from executors.errors import main_information_reading_executor_call_error
from executors.errors import main_information_reading_executor_conversion_error
from calls import driver_caller
from calls.errors import driver_caller_error
from conversion import converter
from conversion import read_memory_response_to_command_response
from conversion.errors import converter_error
from factory import solution
from factory import simple_creator


class MainInformationReadingExecutor(executor.Executor[str, pku_service_pb2.CommandResponse]):
    class Converters(enum.Enum):
        READ_MEMORY_RESPONSE_TO_COMMAND_RESPONSE = 1

    converter_solution: solution.Solution[converter.Converter, Converters] = solution.Solution({
        Converters.READ_MEMORY_RESPONSE_TO_COMMAND_RESPONSE: lambda: simple_creator.SimpleCreator(
            read_memory_response_to_command_response.ReadMemoryResponseToCommandResponse
        )
    })

    caller: driver_caller.DriverCaller

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.caller = caller

    def execute(self, data: str) -> pku_service_pb2.CommandResponse:
        try:
            return self.converter_solution.make(
                self.Converters.READ_MEMORY_RESPONSE_TO_COMMAND_RESPONSE
            ).create().convert(self.caller.read_main_information())
        except driver_caller_error.DriverCallerError as exception:
            raise main_information_reading_executor_call_error.MainInformationReadingExecutorCallError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
        except converter_error.ConverterError as exception:
            raise main_information_reading_executor_conversion_error.MainInformationReadingExecutorConversionError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
