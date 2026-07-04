import sys
import grpc
import google.protobuf.empty_pb2
import pku_driver_pb2
import pku_driver_pb2_grpc

IP: str = "localhost"
PORT: str = ":50051"


def main() -> int:
    with grpc.insecure_channel(IP + PORT) as channel:
        stub: pku_driver_pb2_grpc.PkuDriverStub = pku_driver_pb2_grpc.PkuDriverStub(channel)
        print(f"Версия: {stub.GetVersion(google.protobuf.empty_pb2.Empty()).version:s}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
