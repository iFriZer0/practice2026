import sys
import logging
import os
import connecting_driver_caller

IP: str = "localhost"
PORT: str = "50051"

LOG_PATH: str = "log/log.txt"


def main() -> int:
    os.makedirs("log", exist_ok=True)
    logging.basicConfig(
        level=logging.INFO, filename=LOG_PATH, filemode="a", format="%(asctime)s %(levelname)s: %(name)s.%(funcName)s:  %(message)s"
    )
    with connecting_driver_caller.ConnectingDriverCaller(IP, PORT) as caller:
        print(f"Версия: {caller.get_version().version:s}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
