import enum
import logging
import os
from concurrent import futures
import grpc
import pku_service_pb2_grpc
import main_service_servicer_director
import main_service_servicer_builder
import calling_pku_service_builder
import address_loader
import json_address_loader
from factory import solution
from factory import simple_creator


class Application:
    DRIVER_ADDRESS_PATH: str = "../configuration/driver_address.json"

    LOG_DIRECTORY: str = "log"
    LOG_PATH: str = "log/log.txt"

    WORKERS_COUNT: int = 16

    class Servicers(enum.Enum):
        BUILT = 1

    class Builders(enum.Enum):
        CALLING_PKU = 1

    class Loaders(enum.Enum):
        JSON = 1

    OK: int = 0

    servicer_solution: solution.Solution[pku_service_pb2_grpc.MainServiceServicer, Servicers] = solution.Solution({
        Servicers.BUILT: lambda: main_service_servicer_director.MainServiceServicerDirector()
    })

    builder_solution: solution.Solution[main_service_servicer_builder.MainServiceServicerBuilder, Builders] = solution.Solution({
        Builders.CALLING_PKU: lambda: simple_creator.SimpleCreator(calling_pku_service_builder.CallingPkuServiceBuilder)
    })

    loader_solution: solution.Solution[address_loader.AddressLoader, Loaders] = solution.Solution({
        Loaders.JSON: lambda: simple_creator.SimpleCreator(json_address_loader.JSONAddressLoader)
    })

    @classmethod
    def start(cls) -> int:
        cls.__configure_log()
        loader: address_loader.AddressLoader = cls.loader_solution.make(cls.Loaders.JSON).create(cls.DRIVER_ADDRESS_PATH)
        server: grpc.Server = grpc.server(futures.ThreadPoolExecutor(max_workers=cls.WORKERS_COUNT))
        pku_service_pb2_grpc.add_MainServiceServicer_to_server(
            cls.servicer_solution.make(cls.Servicers.BUILT).create(cls.builder_solution.make(cls.Builders.CALLING_PKU).create(loader)),
            server
        )
        server.add_insecure_port("[::]:" + loader.load_port())
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
