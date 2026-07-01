#!/bin/bash

readonly DIRECTORY="../grpc_ui_service"

cd "$(dirname "$0")" || exit 1
mkdir -p "$DIRECTORY"
protoc --cpp_out="$DIRECTORY" --grpc_out="$DIRECTORY" --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./pku_service.proto
