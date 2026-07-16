import typing


class Error(Exception):
    message: str
    first_error: typing.Type[typing.Any]
    data: typing.Any

    def __init__(self, message: str, first_error: typing.Type[typing.Any], data: typing.Any) -> None:
        self.message = message
        self.first_error = first_error
        self.data = data

    def __str__(self) -> str:
        return self.message

    def get_first_error(self) -> typing.Type[typing.Any]:
        return self.first_error

    def get_data(self) -> typing.Any:
        return self.data
