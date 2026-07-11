import unittest
import enum
import grpc
import pku_service_pb2
import pku_service_pb2_grpc


class IntegrationTests(unittest.TestCase):
    class Commands(enum.Enum):
        CHECK_CONNECTION = 1
        GET_HARDWARE_STATUS = 2
        READ_MAIN_INFO = 3
        WRITE_MAIN_INFO = 4
        READ_PKU = 5
        SET_PKU_MODE = 6
        SEND_RK_BY_INDEX = 7
        GET_VERSION = 8

    ADDRESS: str = "localhost:50052"

    channel: grpc.Channel
    stub: pku_service_pb2_grpc.MainServiceStub

    def setUp(self) -> None:
        self.channel = grpc.insecure_channel(self.ADDRESS)
        self.stub = pku_service_pb2_grpc.MainServiceStub(self.channel)

    def tearDown(self) -> None:
        self.channel.close()

    def test_check_connection(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.CHECK_CONNECTION.value
        request.command_param = ""
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")


if __name__ == "__main__":
    unittest.main()
