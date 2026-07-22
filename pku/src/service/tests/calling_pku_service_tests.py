import unittest
import unittest.mock
import enum
import calling_pku_service
import pku_service_pb2
import pku_driver_pb2
from calls import connecting_driver_caller


class CallingPkuServiceTests(unittest.TestCase):
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

    patcher: unittest.mock._patch
    service: calling_pku_service.CallingPkuService
    caller: connecting_driver_caller.ConnectingDriverCaller

    def setUp(self) -> None:
        self.patcher = unittest.mock.patch("calls.connecting_driver_caller.ConnectingDriverCaller")
        self.caller = self.patcher.start().return_value
        self.service = calling_pku_service.CallingPkuService(self.caller)

    def tearDown(self) -> None:
        self.patcher.stop()

    def test_check_connection(self) -> None:
        self.caller.check_connection.return_value = pku_driver_pb2.StandardResponse(success=True, error_message="")
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(command_id=self.Commands.CHECK_CONNECTION.value, command_param=""), None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_get_hardware_status(self) -> None:
        self.caller.get_hardware_status.return_value = pku_driver_pb2.StandardResponse(success=True, error_message="")
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(command_id=self.Commands.GET_HARDWARE_STATUS.value, command_param=""), None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_main_info(self) -> None:
        self.caller.write_main_information.return_value = pku_driver_pb2.StandardResponse(success=True, error_message="")
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(
                command_id=self.Commands.WRITE_MAIN_INFO.value,
                command_param=self.DELIMITER.join([
                    "a" * self.DESCRIPTION_SIZE,
                    "AA:BB:CC:DD:EE:FF",
                    "1.2.128.255",
                    "1.1.1.1",
                    "01.1.1.1",
                    "0.2.0.2",
                    "1",
                    "4"
                ])
            ),
            None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")
        self.caller.read_main_information.return_value = pku_driver_pb2.ReadMemoryResponse(
            success=True,
            error_message="",
            data= (
                b"\x50\x4B\x55\x69"
                + b"\xdc\x05\x00\x00"
                + b"\x61" * self.DESCRIPTION_SIZE
                + b"\xAA\xBB\xCC\xDD\xEE\xFF"
                + b"\x01\x02\x80\xFF"
                + b"\x01\x01\x01\x01"
                + b"\x01\x01\x01\x01"
                + b"\x00\x02\x00\x02"
                + b"\x01"
            )
        )
        response = self.service.SendCommand(
            pku_service_pb2.CommandRequest(command_id=self.Commands.READ_MAIN_INFO.value, command_param=""), None
        )
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
        self.caller.read_pku.return_value = pku_driver_pb2.ReadPkuResponse(
            success=True,
            error_message="",
            durations_ms=([1, 1, 0, 1] + [0] * 60)
        )
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(
                command_id=self.Commands.READ_PKU.value, command_param=self.DELIMITER.join(["1", "2", "4", "5"])
            ),
            None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, self.DELIMITER.join(["1", "1", "0", "1"] + ["0"] * 60))

    def test_set_pku_mode(self) -> None:
        self.caller.set_pku_mode.return_value = pku_driver_pb2.StandardResponse(success=True, error_message="")
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(
                command_id=self.Commands.SET_PKU_MODE.value, command_param=self.DELIMITER.join(["1", "0", "op"])
            ),
            None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_send_rk_by_index(self) -> None:
        self.caller.send_rk_by_index.return_value = pku_driver_pb2.StandardResponse(success=True, error_message="")
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(
                command_id=self.Commands.SEND_RK_BY_INDEX.value, command_param=self.DELIMITER.join(["1", "100", "op"])
            ),
            None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "")

    def test_get_version(self) -> None:
        self.caller.get_version.return_value = pku_driver_pb2.VersionInfo(version="0.04")
        response: pku_service_pb2.CommandResponse = self.service.SendCommand(
            pku_service_pb2.CommandRequest(command_id=self.Commands.GET_VERSION.value, command_param=""), None
        )
        self.assertTrue(response.success)
        self.assertEqual(response.result_text, "0.04")

