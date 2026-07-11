#!/bin/bash

readonly DIRECTORY_PKU_DRIVER_API="."
readonly DIRECTORY_PKU_SERVICE_API="../../../api"

readonly DIRECTORY_PKU_DRIVER="../driver/grpc_pku_driver_cpp"
readonly DIRECTORY_PKU_SERVICE="../service"
readonly DIRECTORY_PKU_TESTS="../tests"

readonly API_PKU_SERVICE="pku_service.proto"
readonly API_PKU_DRIVER="pku_driver.proto"

generate_grpc_cpp() {
	mkdir -p "$1"
	protoc --cpp_out="$1" --grpc_out="$1" --plugin=protoc-gen-grpc="$(which grpc_cpp_plugin)" "$2"
}

generate_grpc_python() {
	mkdir -p "$1"
        python -m grpc_tools.protoc -I"$1" --python_out="$2" --pyi_out="$2" --grpc_python_out="$2" "$3"
}

cd "$(dirname "$0")" || exit 1
generate_grpc_cpp "$DIRECTORY_PKU_DRIVER" "$API_PKU_DRIVER"
generate_grpc_python "$DIRECTORY_PKU_DRIVER_API" "$DIRECTORY_PKU_SERVICE" "$API_PKU_DRIVER"
generate_grpc_python "$DIRECTORY_PKU_SERVICE_API" "$DIRECTORY_PKU_SERVICE" "$API_PKU_SERVICE"
generate_grpc_python "$DIRECTORY_PKU_SERVICE_API" "$DIRECTORY_PKU_TESTS" "$API_PKU_SERVICE"
