import abc


class AddressLoader(abc.ABC):
    @abc.abstractmethod
    def load_ip(self) -> str:
        pass

    @abc.abstractmethod
    def load_port(self) -> str:
        pass
