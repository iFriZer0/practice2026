import sys
import connecting_driver_caller

IP: str = "localhost"
PORT: str = "50051"


def main() -> int:
    with connecting_driver_caller.ConnectingDriverCaller(IP, PORT) as caller:
        print(f"Версия: {caller.get_version().version:s}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
