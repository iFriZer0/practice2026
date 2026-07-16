import typing
import abc

Source = typing.TypeVar("Source")
Destination = typing.TypeVar("Destination")


class Converter(abc.ABC, typing.Generic[Source, Destination]):
    @abc.abstractmethod
    def convert(self, source: Source) -> Destination:
        pass
