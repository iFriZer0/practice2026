import sys
import grpc
import pku_driver_test_pb2
import pku_driver_test_pb2_grpc

IP: str = "localhost"
PORT: str = ":50051"


def main() -> int:
    with grpc.insecure_channel(IP + PORT) as channel:
        stub: pku_driver_test_pb2_grpc.PkuDriverStub = pku_driver_test_pb2_grpc.PkuDriverStub(channel)
        numbers_sum: pku_driver_test_pb2.AddResponse = stub.Add(pku_driver_test_pb2.AddRequest(number_1=1, number_2=2))
        print(f"Сумма: {numbers_sum.sum:d}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
