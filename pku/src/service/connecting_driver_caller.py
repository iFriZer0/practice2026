import collections
import typing
import logging
import grpc
import google.protobuf.empty_pb2
import pku_driver_pb2
import pku_driver_pb2_grpc
import decorate_with_logger
import connecting_driver_caller_ping_error
import connecting_driver_caller_connection_error
from calls import driver_caller

Parameters = typing.ParamSpec("Parameters")
Response = typing.TypeVar("Response")


@decorate_with_logger.decorate_with_logger
class ConnectingDriverCaller(driver_caller.DriverCaller):
    channel: grpc.Channel
    stub: pku_driver_pb2_grpc.PkuDriverStub

    TIMEOUT: int = 3

    logger: logging.Logger

    def __init__(self, ip: str, port: str) -> None:
        self.channel = grpc.insecure_channel(ip + ":" + port)
        self.__ping()
        self.stub = pku_driver_pb2_grpc.PkuDriverStub(self.channel)
        self.logger.info("Connection was established.")

    def __del__(self) -> None:
        self.channel.close()
        self.logger.info("Connection was broken.")

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
                try:
                    return input_function(self, *args, **kwargs)
                except grpc.RpcError as exception:
                    self.logger.error("Connection was lost before the call.")
                    raise connecting_driver_caller_connection_error.ConnectingDriverCallerConnectionError(
                        "Connection is lost.", connecting_driver_caller_connection_error.ConnectingDriverCallerConnectionError
                    ) from exception
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

    @__decorate_call_with_connection_check(pku_driver_pb2.VersionInfo)
    def get_version(self) -> pku_driver_pb2.VersionInfo:
        return self.stub.GetVersion(google.protobuf.empty_pb2.Empty())

    def __ping(self) -> None:
        try:
            grpc.channel_ready_future(self.channel).result(timeout=self.TIMEOUT)
        except grpc.FutureTimeoutError as exception:
            self.logger.error("Connection was not established.")
            raise connecting_driver_caller_ping_error.ConnectingDriverCallerPingError(
                "Connection was not established.", connecting_driver_caller_ping_error.ConnectingDriverCallerPingError
            ) from exception
