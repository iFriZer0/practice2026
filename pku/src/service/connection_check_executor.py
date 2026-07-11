import enum
import pku_service_pb2
import connection_check_converter
from executors import executor
from calls import driver_caller
from conversion import converter
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
        return self.converter_solution.make(
            self.Converters.CONNECTION_CHECK
        ).create().convert_to_destination(self.caller.check_connection())
