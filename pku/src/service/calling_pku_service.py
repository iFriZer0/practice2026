import enum
import typing
import logging
import grpc
import pku_service_pb2_grpc
import pku_service_pb2
import connection_check_executor
import decorate_with_logger
from calls import driver_caller
from executors import executor
from factory import solution
from factory import simple_creator


@decorate_with_logger.decorate_with_logger
class CallingPkuService(pku_service_pb2_grpc.MainServiceServicer):
    class Commands(enum.Enum):
        CHECK_CONNECTION = 1
        GET_HARDWARE_STATUS = 2
        READ_MAIN_INFO = 3
        WRITE_MAIN_INFO = 4
        READ_PKU = 5
        SET_PKU_MODE = 6
        SEND_RK_BY_INDEX = 7
        GET_VERSION = 8

    class Executors(enum.Enum):
        CONNECTION_CHECK = 1
        HARDWARE_STATUS_CHECK = 2
        MAIN_INFO_READ = 3
        MAIN_INFO_WRITE = 4
        PKU_READ = 5
        PKU_MODE_SET = 6
        RK_SENDING = 7
        VERSION_RECEIVING = 8

    executor_solution: solution.Solution[executor.Executor[str, pku_service_pb2.CommandResponse], Executors] = solution.Solution({
        Executors.CONNECTION_CHECK: lambda: simple_creator.SimpleCreator(connection_check_executor.ConnectionCheckExecutor)
    })

    logger: logging.Logger

    executors: typing.Dict[Executors, executor.Executor[str, pku_service_pb2.CommandResponse]]

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.executors = {
            self.Executors.CONNECTION_CHECK: self.executor_solution.make(self.Executors.CONNECTION_CHECK).create(caller)
        }

    def SendCommand(self, request: pku_service_pb2.CommandRequest, context: grpc.ServicerContext) -> pku_service_pb2.CommandResponse:
        response: pku_service_pb2.CommandResponse
        try:
            command_id: CallingPkuService.Executors = self.Executors(request.command_id)
            response = self.executors[command_id].execute(request.command_param)
        except ValueError:
            response = pku_service_pb2.CommandResponse()
            response.success = False
            response.result_text = f"Unknown command {request.command_id:d}"
            self.logger.warning(f"Unkown command {request.command_id:d}")
        return response
