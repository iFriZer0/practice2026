#!/bin/bash

readonly DIRECTORY_SERVICE_CPP="../grpc_service_cpp"
readonly DIRECTORY_PKU_DRIVER_CPP="../grpc_pku_driver_cpp"
readonly DIRECTORY_PKU_DRIVER_PYTHON="../service"

readonly API_SERVICE="service.proto"
readonly API_PKU_DRIVER="pku_driver_test.proto"

generate_grpc_cpp() {
	mkdir -p "$1"
	protoc --cpp_out="$1" --grpc_out="$1" --plugin=protoc-gen-grpc="$(which grpc_cpp_plugin)" "$2"
}

generate_grpc_python() {
	mkdir -p "$1"
        python -m grpc_tools.protoc -I. --python_out="$1" --pyi_out="$1" --grpc_python_out="$1" "$2"
	touch "$1/__init__.py"
}

cd "$(dirname "$0")" || exit 1
generate_grpc_cpp "$DIRECTORY_PKU_DRIVER_CPP" "$API_PKU_DRIVER"
generate_grpc_python "$DIRECTORY_PKU_DRIVER_PYTHON" "$API_PKU_DRIVER"
