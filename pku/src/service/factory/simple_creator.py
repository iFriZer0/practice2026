import typing
from factory import creator

Base = typing.TypeVar("Base")


class SimpleCreator(creator.Creator[Base], typing.Generic[Base]):
    derived: typing.Type[Base]

    def __init__(self, derived: typing.Type[Base]) -> None:
        self.derived = derived

    def create(self, *args, **kwargs) -> Base:
        return self.derived(*args, **kwargs)
