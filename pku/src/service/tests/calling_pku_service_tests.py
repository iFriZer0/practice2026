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
