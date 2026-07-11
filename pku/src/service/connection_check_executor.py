import enum
import pku_service_pb2
import connection_check_converter
from executors import executor
from executors.errors import connection_check_executor_call_error
from executors.errors import connection_check_executor_conversion_error
from calls import driver_caller
from calls.errors import driver_caller_error
from conversion import converter
from conversion.errors import converter_error
from factory import solution
from factory import simple_creator


class ConnectionCheckExecutor(executor.Executor[str, pku_service_pb2.CommandResponse]):
    class Converters(enum.Enum):
        CONNECTION_CHECK = 1

    converter_solution: solution.Solution[converter.Converter, Converters] = solution.Solution({
        Converters.CONNECTION_CHECK: lambda: simple_creator.SimpleCreator(connection_check_converter.ConnectionCheckConverter)
    })

    caller: driver_caller.DriverCaller

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.caller = caller

    def execute(self, data: str) -> pku_service_pb2.CommandResponse:
        try:
            return self.converter_solution.make(
                self.Converters.CONNECTION_CHECK
            ).create().convert_to_destination(self.caller.check_connection())
        except driver_caller_error.DriverCallerError as exception:
            raise connection_check_executor_call_error.ConnectionCheckExecutorCallError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
        except converter_error.ConverterError as exception:
            raise connection_check_executor_conversion_error.ConnectionCheckExecutorConversionError(
                str(exception), exception.get_first_error(), exception.get_data()
            ) from exception
