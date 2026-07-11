import typing
import collections
from factory import creator
from factory.errors import solution_no_maker_error

Base = typing.TypeVar("Base")
Identifier = typing.TypeVar("Identifier")


class Solution(typing.Generic[Base, Identifier]):
    makers: typing.Dict[Identifier, collections.abc.Callable[[], creator.Creator[Base]]]

    def __init__(self, makers: typing.Dict[Identifier, collections.abc.Callable[[], creator.Creator[Base]]] = {}) -> None:
        self.makers = makers

    def add(self, identifier: Identifier, maker: collections.abc.Callable[[], creator.Creator[Base]]) -> bool:
        added: bool = False
        if identifier not in self.makers:
            self.makers[identifier] = maker
            added = True
        return added

    def make(self, identifier: Identifier) -> creator.Creator[Base]:
        try:
            return self.makers[identifier]()
        except KeyError as exception:
            raise solution_no_maker_error.SolutionNoMakerError(
                "Maker was not found", solution_no_maker_error.SolutionNoMakerError, identifier
            ) from exception
