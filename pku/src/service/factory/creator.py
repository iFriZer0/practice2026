import typing
import abc

Base = typing.TypeVar("Base")


class Creator(abc.ABC, typing.Generic[Base]):
    @abc.abstractmethod
    def create(self, *args, **kwargs) -> Base:
        pass
