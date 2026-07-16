import abc
import typing

Data = typing.TypeVar("Data")
Result = typing.TypeVar("Result")


class Executor(abc.ABC, typing.Generic[Data, Result]):
    @abc.abstractmethod
    def execute(self, data: Data) -> Result:
        pass
