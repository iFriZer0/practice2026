import pku_service_pb2
from executors import executor
from calls import driver_caller


class ConnectionCheckExecutor(executor.Executor[str, pku_service_pb2.CommandResponse]):
    caller: driver_caller.DriverCaller

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.caller = caller

    def execute(self, data: str) -> pku_service_pb2.CommandResponse:
        self.caller.check_connection()

