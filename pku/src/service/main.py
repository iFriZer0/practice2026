import sys
import application


def main() -> int:
    return application.Application.start()


if __name__ == "__main__":
    sys.exit(main())
