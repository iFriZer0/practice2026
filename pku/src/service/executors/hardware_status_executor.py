import enum
import pku_service_pb2
from executors import executor
from executors.errors import hardware_status_executor_call_error
from executors.errors import hardware_status_executor_conversion_error
from calls import driver_caller
from calls.errors import driver_caller_error
from conversion import converter
from conversion import standard_response_to_command_response
from conversion.errors import converter_error
from factory import solution
from factory import simple_creator


class HardwareStatusExecutor(executor.Executor[str, pku_service_pb2.CommandResponse]):
    class Converters(enum.Enum):
        STANDARD_RESPONSE_TO_COMMAND_RESPONSE = 1

    converter_solution: solution.Solution[converter.Converter, Converters] = solution.Solution({
        Converters.STANDARD_RESPONSE_TO_COMMAND_RESPONSE: lambda: simple_creator.SimpleCreator(
            standard_response_to_command_response.StandardResponseToCommandResponse
        )
    })

    caller: driver_caller.DriverCaller

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.caller = caller

    def execute(self, data: str) -> pku_service_pb2.CommandResponse:
        try:
            return self.converter_solution.make(
                self.Converters.STANDARD_RESPONSE_TO_COMMAND_RESPONSE
            ).create().convert(self.caller.get_hardware_status())
        except driver_caller_error.DriverCallerError as exception:
            raise hardware_status_executor_call_error.HardwareStatusExecutorCallError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
        except converter_error.ConverterError as exception:
            raise hardware_status_executor_conversion_error.HardwareStatusExecutorConversionError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
