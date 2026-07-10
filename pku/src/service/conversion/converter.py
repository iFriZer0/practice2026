import typing
import abc

Source = typing.TypeVar("Source")
Destination = typing.TypeVar("Destination")


class Converter(abc.ABC, typing.Generic[Source, Destination]):
    @abc.abstractmethod
    def convert_to_destination(self, source: Source) -> Destination:
        pass

    @abc.abstractmethod
    def convert_to_source(self, destination: Destination) -> Source:
        pass
