import enum
import pku_service_pb2
from executors import executor
from executors.errors import pku_mode_setting_executor_call_error
from executors.errors import pku_mode_setting_executor_conversion_error
from calls import driver_caller
from calls.errors import driver_caller_error
from conversion import converter
from conversion import str_to_set_pku_mode_request
from conversion import standard_response_to_command_response
from conversion.errors import converter_error
from factory import solution
from factory import simple_creator


class PkuModeSettingExecutor(executor.Executor[str, pku_service_pb2.CommandResponse]):
    class Converters(enum.Enum):
        STR_TO_SET_PKU_MODE_REQUEST = 1
        STANDARD_RESPONSE_TO_COMMAND_RESPONSE = 2

    converter_solution: solution.Solution[converter.Converter, Converters] = solution.Solution({
        Converters.STR_TO_SET_PKU_MODE_REQUEST: lambda: simple_creator.SimpleCreator(
            str_to_set_pku_mode_request.StrToSetPkuModeRequest
        ),
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
            ).create().convert(
                self.caller.set_pku_mode(
                    self.converter_solution.make(self.Converters.STR_TO_SET_PKU_MODE_REQUEST).create().convert(data)
                )
            )
        except driver_caller_error.DriverCallerError as exception:
            raise pku_mode_setting_executor_call_error.PkuModeSettingExecutorCallError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
        except converter_error.ConverterError as exception:
            raise pku_mode_setting_executor_conversion_error.PkuModeSettingExecutorConversionError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
