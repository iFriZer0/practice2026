import collections
import typing
import grpc
import google.protobuf.empty_pb2
import driver_caller
import pku_driver_pb2
import pku_driver_pb2_grpc
import connecting_driver_caller_ping_error


class ResponseProtocol(typing.Protocol):
    success: bool
    error_message: str


Parameters = typing.ParamSpec("Parameters")
Response = typing.TypeVar("Response", bound=ResponseProtocol)


class ConnectingDriverCaller(driver_caller.DriverCaller):
    ip: str
    port: str
    channel: grpc.Channel
    stub: pku_driver_pb2_grpc.PkuDriverStub
    connected: bool

    TIMEOUT: int = 3

    def __init__(self, ip: str, port: str) -> None:
        self.ip = ip
        self.port = port
        self.connected = False

    def __enter__(self) -> "ConnectingDriverCaller":
        self.channel = grpc.insecure_channel(self.ip + ":" + self.port)
        self.__ping()
        self.stub = pku_driver_pb2_grpc.PkuDriverStub(self.channel)
        self.connected = True
        return self

    def __exit__(self, exc_type: typing.Any, exc_val: typing.Any, exc_tb: typing.Any) -> None:
        self.channel.close()
        self.connected = False

    @staticmethod
    def __decorate_call_with_connection_check(
        response_type: typing.Type[Response]
    ) -> collections.abc.Callable[
        [collections.abc.Callable[typing.Concatenate["ConnectingDriverCaller", Parameters], Response]],
        collections.abc.Callable[typing.Concatenate["ConnectingDriverCaller", Parameters], Response]
    ]:
        def call_with_connection_check(
            input_function: collections.abc.Callable[typing.Concatenate["ConnectingDriverCaller", Parameters], Response]
        ) -> collections.abc.Callable[typing.Concatenate["ConnectingDriverCaller", Parameters], Response]:
            def output_function(self: "ConnectingDriverCaller", *args: Parameters.args, **kwargs: Parameters.kwargs) -> Response:
                response: Response = response_type()
                if self.connected:
                    response = input_function(self, *args, **kwargs)
                else:
                    response.success = False
                    response.error_message = "ConnectingDriverCaller is not connected."
                return response
            return output_function
        return call_with_connection_check

    @__decorate_call_with_connection_check(pku_driver_pb2.StandardResponse)
    def check_connection(self) -> pku_driver_pb2.StandardResponse:
        return self.stub.CheckConnection(google.protobuf.empty_pb2.Empty())

    @__decorate_call_with_connection_check(pku_driver_pb2.StandardResponse)
    def get_hardware_status(self) -> pku_driver_pb2.StandardResponse:
        return self.stub.GetHardwareStatus(google.protobuf.empty_pb2.Empty())

    @__decorate_call_with_connection_check(pku_driver_pb2.ReadMemoryResponse)
    def read_main_information(self) -> pku_driver_pb2.ReadMemoryResponse:
        return self.stub.ReadMainInfo(google.protobuf.empty_pb2.Empty())

    @__decorate_call_with_connection_check(pku_driver_pb2.StandardResponse)
    def write_main_information(self, request: pku_driver_pb2.WriteMainInfoRequest) -> pku_driver_pb2.StandardResponse:
        return self.stub.WriteMainInfo(request)

    @__decorate_call_with_connection_check(pku_driver_pb2.ReadPkuResponse)
    def read_pku(self, request: pku_driver_pb2.ReadPkuRequest) -> pku_driver_pb2.ReadPkuResponse:
        return self.stub.ReadPku(request)

    @__decorate_call_with_connection_check(pku_driver_pb2.StandardResponse)
    def set_pku_mode(self, request: pku_driver_pb2.SetPkuModeRequest) -> pku_driver_pb2.StandardResponse:
        return self.stub.SetPkuMode(request)

    @__decorate_call_with_connection_check(pku_driver_pb2.StandardResponse)
    def send_rk_by_index(self, request: pku_driver_pb2.SendRkByIndexRequest) -> pku_driver_pb2.StandardResponse:
        return self.stub.SendRkByIndex(request)

    def get_version(self) -> pku_driver_pb2.VersionInfo:
        response: pku_driver_pb2.VersionInfo = pku_driver_pb2.VersionInfo()
        if self.connected:
            response = self.stub.GetVersion(google.protobuf.empty_pb2.Empty())
        else:
            response.version = "ConnectingDriverCaller is not connected."
        return response

    def __ping(self) -> None:
        try:
            grpc.channel_ready_future(self.channel).result(timeout=self.TIMEOUT)
        except grpc.FutureTimeoutError as exception:
            raise connecting_driver_caller_ping_error.ConnectingDriverCallerPingError(
                "Connection was not established.", connecting_driver_caller_ping_error.ConnectingDriverCallerPingError
            ) from exception
