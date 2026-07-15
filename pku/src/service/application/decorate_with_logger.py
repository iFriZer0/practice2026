import typing
import logging

Type = typing.TypeVar("Type", bound=type)


def decorate_with_logger(cls: Type) -> Type:
    setattr(cls, "logger", logging.getLogger(f"{cls.__module__}.{cls.__qualname__}"))
    return cls
