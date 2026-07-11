import enum
import pku_service_pb2_grpc
import calling_pku_service
from application.factory import main_service_servicer_builder
from application.factory.loaders import address_loader
from calls import driver_caller
from calls import connecting_driver_caller
from factory import solution
from factory import simple_creator


class CallingPkuServiceBuilder(main_service_servicer_builder.MainServiceServicerBuilder):
    class Servicers(enum.Enum):
        CALLING_PKU = 1

    class Callers(enum.Enum):
        CONNECTING = 1

    servicer_solution: solution.Solution[pku_service_pb2_grpc.MainServiceServicer, Servicers] = solution.Solution({
        Servicers.CALLING_PKU: lambda: simple_creator.SimpleCreator(calling_pku_service.CallingPkuService)
    })

    caller_solution: solution.Solution[driver_caller.DriverCaller, Callers] = solution.Solution({
        Callers.CONNECTING: lambda: simple_creator.SimpleCreator(connecting_driver_caller.ConnectingDriverCaller)
    })

    loader: address_loader.AddressLoader

    def __init__(self, loader: address_loader.AddressLoader) -> None:
        self.loader = loader

    def build_servicer(self) -> pku_service_pb2_grpc.MainServiceServicer:
        return self.servicer_solution.make(self.Servicers.CALLING_PKU).create(self.__build_caller())

    def __build_caller(self) -> driver_caller.DriverCaller:
        return self.caller_solution.make(self.Callers.CONNECTING).create(self.loader.load_ip(), self.loader.load_port())
