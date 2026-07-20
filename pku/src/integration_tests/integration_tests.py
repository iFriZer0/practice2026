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

    DELIMITER: str = ";"

    DESCRIPTION_SIZE: int = 120

    channel: grpc.Channel
    stub: pku_service_pb2_grpc.MainServiceStub

    def setUp(self) -> None:
        ip: str
        port: str
        with open("../configuration/pku_service_address.txt", "r", encoding="utf-8") as file:
            ip = file.readline().rstrip()
            port = file.readline().rstrip()
        self.channel = grpc.insecure_channel(ip + ":" + port)
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

    def test_get_hardware_status(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.GET_HARDWARE_STATUS.value
        request.command_param = ""
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_main_info(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.WRITE_MAIN_INFO.value
        request.command_param = self.DELIMITER.join([
            "a" * self.DESCRIPTION_SIZE,
            "AA:BB:CC:DD:EE:FF",
            "1.2.128.255",
            "1.1.1.1",
            "01.1.1.1",
            "0.2.0.2",
            "1",
            "op"
        ])
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")
        request.command_id = self.Commands.READ_MAIN_INFO.value
        request.command_param = ""
        response = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, self.DELIMITER.join([
            "\x50\x4B\x55\x69",
            "1500",
            "a" * self.DESCRIPTION_SIZE,
            "\xAA\xBB\xCC\xDD\xEE\xFF",
            "\x01\x02\x80\xFF",
            "\x01\x01\x01\x01",
            "\x01\x01\x01\x01",
            "\x00\x02\x00\x02",
            "1"
        ]))

    def test_read_pku(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.READ_PKU.value
        request.command_param = self.DELIMITER.join(["1", "2", "4", "op"])
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, self.DELIMITER.join(["1", "1", "0", "1"] + ["0"] * 60))

    def test_set_pku_mode(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.SET_PKU_MODE.value
        request.command_param = self.DELIMITER.join(["1", "0", "op"])
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_send_rk_by_index(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.SEND_RK_BY_INDEX.value
        request.command_param = self.DELIMITER.join(["1", "100", "op"])
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_get_version(self) -> None:
        request: pku_service_pb2.CommandRequest = pku_service_pb2.CommandRequest()
        request.command_id = self.Commands.GET_VERSION.value
        request.command_param = ""
        response: pku_service_pb2.CommandResponse = self.stub.SendCommand(request)
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "v1")


if __name__ == "__main__":
    unittest.main()
