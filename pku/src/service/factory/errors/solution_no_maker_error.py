import typing
from factory.errors import solution_error

Identifier = typing.TypeVar("Identifier")


class SolutionNoMakerError(solution_error.SolutionError, typing.Generic[Identifier]):
    def __init__(self, message: str, first_error: typing.Type[typing.Any], identifier: Identifier) -> None:
        super().__init__(message, first_error, identifier)
