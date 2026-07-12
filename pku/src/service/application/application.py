import enum
import logging
import os
from concurrent import futures
import grpc
import pku_service_pb2_grpc
from application.factory import main_service_servicer_director
from application.factory import main_service_servicer_builder
from application.factory import calling_pku_service_builder
from application.factory.loaders import address_loader
from application.factory.loaders import json_address_loader
from application.factory.loaders import txt_address_loader
from factory import solution
from factory import simple_creator
from errors import error


class Application:
    DRIVER_ADDRESS_PATH: str = "../configuration/driver_address.json"
    SERVICE_ADDRESS_PATH: str = "../../../configuration/pku_service_address.txt"

    LOG_DIRECTORY: str = "log"
    LOG_PATH: str = "log/log.txt"

    WORKERS_COUNT: int = 16

    class Servicers(enum.Enum):
        BUILT = 1

    class Builders(enum.Enum):
        CALLING_PKU = 1

    class Loaders(enum.Enum):
        JSON = 1
        TXT = 2

    OK: int = 0

    servicer_solution: solution.Solution[pku_service_pb2_grpc.MainServiceServicer, Servicers] = solution.Solution({
        Servicers.BUILT: lambda: main_service_servicer_director.MainServiceServicerDirector()
    })

    builder_solution: solution.Solution[main_service_servicer_builder.MainServiceServicerBuilder, Builders] = solution.Solution({
        Builders.CALLING_PKU: lambda: simple_creator.SimpleCreator(calling_pku_service_builder.CallingPkuServiceBuilder)
    })

    loader_solution: solution.Solution[address_loader.AddressLoader, Loaders] = solution.Solution({
        Loaders.JSON: lambda: simple_creator.SimpleCreator(json_address_loader.JSONAddressLoader),
        Loaders.TXT: lambda: simple_creator.SimpleCreator(txt_address_loader.TXTAddressLoader)
    })

    @classmethod
    def start(cls) -> int:
        cls.__configure_log()
        server: grpc.Server = cls.__create_server()
        server.start()
        server.wait_for_termination()
        return cls.OK

    @classmethod
    def __configure_log(cls) -> None:
        try:
            os.makedirs(cls.LOG_DIRECTORY, exist_ok=True)
            logging.basicConfig(
                level=logging.INFO,
                filename=cls.LOG_PATH,
                filemode="a",
                format="%(asctime)s %(levelname)s: %(name)s.%(funcName)s:  %(message)s"
            )
        except PermissionError:
            print("Недостаточно прав для создания лога.")

    @classmethod
    def __create_server(cls) -> grpc.Server:
        try:
            server: grpc.Server = grpc.server(futures.ThreadPoolExecutor(max_workers=cls.WORKERS_COUNT))
            pku_service_pb2_grpc.add_MainServiceServicer_to_server(
                cls.servicer_solution.make(
                    cls.Servicers.BUILT
                ).create(
                    cls.builder_solution.make(
                        cls.Builders.CALLING_PKU
                    ).create(cls.loader_solution.make(cls.Loaders.JSON).create(cls.DRIVER_ADDRESS_PATH))
                ),
                server
            )
            server.add_insecure_port("[::]:" + cls.loader_solution.make(cls.Loaders.TXT).create(cls.SERVICE_ADDRESS_PATH).load_port())
        except (error.Error, OSError) as exception:
            print(f"Не удалось создать сервер: \"{str(exception):s}\".")
        return server
