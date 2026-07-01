#!/bin/bash

readonly DIRECTORY_UI_SERVICE="../grpc_ui_service"
readonly DIRECTORY_SERVICE_DRIVER="../grpc_service_driver"

readonly API_UI_SERVICE="ui_service.proto"
readonly API_SERVICE_DRIVER="service_driver.proto"

generate_grpc() {
    mkdir -p "$1"
    protoc --cpp_out="$1" --grpc_out="$1" --plugin=protoc-gen-grpc="$(which grpc_cpp_plugin)" "$2"
}

cd "$(dirname "$0")" || exit 1
generate_grpc "$DIRECTORY_UI_SERVICE" "$API_UI_SERVICE"
generate_grpc "$DIRECTORY_SERVICE_DRIVER" "$API_SERVICE_DRIVER"
