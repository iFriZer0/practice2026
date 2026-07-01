#!/bin/bash

DIRECTORY="../grpc_ui_service"

cd "$(dirname "$0")" || exit 1
protoc --cpp_out="$DIRECTORY" --grpc_out="$DIRECTORY" --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./pku_service.proto
