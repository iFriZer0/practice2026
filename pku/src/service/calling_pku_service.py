import enum
import grpc
import pku_service_pb2_grpc
import pku_service_pb2


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

    def SendCommand(self, request: pku_service_pb2.CommandRequest, context: grpc.ServicerContext) -> pku_service_pb2.CommandResponse:
        pass
