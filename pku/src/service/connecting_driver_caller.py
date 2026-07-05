import collections
import typing
import functools
import grpc
import google.protobuf.empty_pb2
import driver_caller
import pku_driver_pb2
import pku_driver_pb2_grpc

Request = typing.TypeVar("Request")


class Response(typing.Protocol):
    success: bool
    error_message: str


class ConnectingDriverCaller(driver_caller.DriverCaller):
    ip: str
    port: str
    channel: grpc.Channel
    stub: pku_driver_pb2_grpc.PkuDriverStub
    connected: bool

    def __init__(self, ip: str, port: str) -> None:
        self.ip = ip
        self.port = port
        self.connected = False

    def __enter__(self) -> "ConnectingDriverCaller":
        self.channel = grpc.insecure_channel(self.ip + ":" + self.port)
        self.connected = True
        return self

    def __exit__(self) -> bool:
        self.channel.close()
        self.connected = False
        return False

    @staticmethod
    def __call(
        input_function: collections.abc.Callable[["ConnectingDriverCaller", Request], Response]
    ) -> collections.abc.Callable[["ConnectingDriverCaller", Request], Response]:
        @functools.wraps(input_function)
        def output_function(self: "ConnectingDriverCaller", argument: Request) -> Response:
            response: Response
            if self.connected:
                response = input_function(self, argument)
            else:
                response.success = False
                response.error_message = "ConnectingDriverCaller is not connected."
            return response
        return output_function

    @__call
    def check_connection(self) -> pku_driver_pb2.StandardResponse:
        return self.stub.CheckConnection(google.protobuf.empty_pb2.Empty())

    @__call
    def get_hardware_status(self) -> pku_driver_pb2.StandardResponse:
        return self.stub.GetHardwareStatus(google.protobuf.empty_pb2.Empty())

    @__call
    def read_main_information(self) -> pku_driver_pb2.ReadMemoryResponse:
        return self.stub.ReadMainInfo(google.protobuf.empty_pb2.Empty())

    @__call
    def write_main_information(self, request: pku_driver_pb2.WriteMainInfoRequest) -> pku_driver_pb2.StandardResponse:
        return self.stub.WriteMainInfo(request)

    @__call
    def read_pku(self, request: pku_driver_pb2.ReadPkuRequest) -> pku_driver_pb2.ReadPkuResponse:
        return self.stub.ReadPku(request)

    @__call
    def set_pku_mode(self, request: pku_driver_pb2.SetPkuModeRequest) -> pku_driver_pb2.StandardResponse:
        return self.stub.SetPkuMode(request)

    @__call
    def send_rk_by_index(self, request: pku_driver_pb2.SendRkByIndexRequest) -> pku_driver_pb2.StandardResponse:
        return self.stub.SendRkByIndex(request)

    @__call
    def get_version(self) -> pku_driver_pb2.VersionInfo:
        return self.stub.GetVersion(google.protobuf.empty_pb2.Empty())
