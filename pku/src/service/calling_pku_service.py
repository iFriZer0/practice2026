import enum
import typing
import logging
import grpc
import pku_service_pb2_grpc
import pku_service_pb2
from application import decorate_with_logger
from calls import driver_caller
from executors import executor
from executors import connection_check_executor
from executors import hardware_status_executor
from executors import main_information_reading_executor
from executors import main_information_writing_executor
from executors import pku_reading_executor
from executors import pku_mode_setting_executor
from executors import rk_sending_executor
from executors import version_executor
from executors.errors import executor_error
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
        HARDWARE_STATUS = 2
        MAIN_INFORMATION_READING = 3
        MAIN_INFORMATION_WRITING = 4
        PKU_READING = 5
        PKU_MODE_SETTING = 6
        RK_SENDING = 7
        VERSION = 8

    executor_solution: solution.Solution[executor.Executor[str, pku_service_pb2.CommandResponse], Executors] = solution.Solution({
        Executors.CONNECTION_CHECK: lambda: simple_creator.SimpleCreator(
            connection_check_executor.ConnectionCheckExecutor
        ),
        Executors.HARDWARE_STATUS: lambda: simple_creator.SimpleCreator(
            hardware_status_executor.HardwareStatusExecutor
        ),
        Executors.MAIN_INFORMATION_READING: lambda: simple_creator.SimpleCreator(
            main_information_reading_executor.MainInformationReadingExecutor
        ),
        Executors.MAIN_INFORMATION_WRITING: lambda: simple_creator.SimpleCreator(
            main_information_writing_executor.MainInformationWritingExecutor
        ),
        Executors.PKU_READING: lambda: simple_creator.SimpleCreator(
            pku_reading_executor.PkuReadingExecutor
        ),
        Executors.PKU_MODE_SETTING: lambda: simple_creator.SimpleCreator(
            pku_mode_setting_executor.PkuModeSettingExecutor
        ),
        Executors.RK_SENDING: lambda: simple_creator.SimpleCreator(
            rk_sending_executor.RkSendingExecutor
        ),
        Executors.VERSION: lambda: simple_creator.SimpleCreator(
            version_executor.VersionExecutor
        )
    })

    logger: logging.Logger

    executors: typing.Dict[Executors, executor.Executor[str, pku_service_pb2.CommandResponse]]

    def __init__(self, caller: driver_caller.DriverCaller) -> None:
        self.executors = {
            self.Executors.CONNECTION_CHECK: self.executor_solution.make(
                self.Executors.CONNECTION_CHECK
            ).create(caller),
            self.Executors.HARDWARE_STATUS: self.executor_solution.make(
                self.Executors.HARDWARE_STATUS
            ).create(caller),
            self.Executors.MAIN_INFORMATION_READING: self.executor_solution.make(
                self.Executors.MAIN_INFORMATION_READING
            ).create(caller),
            self.Executors.MAIN_INFORMATION_WRITING: self.executor_solution.make(
                self.Executors.MAIN_INFORMATION_WRITING
            ).create(caller),
            self.Executors.PKU_READING: self.executor_solution.make(
                self.Executors.PKU_READING
            ).create(caller),
            self.Executors.PKU_MODE_SETTING: self.executor_solution.make(
                self.Executors.PKU_MODE_SETTING
            ).create(caller),
            self.Executors.RK_SENDING: self.executor_solution.make(
                self.Executors.RK_SENDING
            ).create(caller),
            self.Executors.VERSION: self.executor_solution.make(
                self.Executors.VERSION
            ).create(caller)
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
            self.logger.warning(f"Unkown command {request.command_id:d}.")
        except executor_error.ExecutorError as exception:
            response = pku_service_pb2.CommandResponse()
            response.success = False
            response.result_text = str(exception)
            self.logger.warning(f"Execution error: \"{response.result_text:s}\".")
        return response
