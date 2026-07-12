#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(
    cd "$(dirname "${BASH_SOURCE[0]}")"
    pwd
)"

PROJECT_DIR="$(
    cd "${SCRIPT_DIR}/.."
    pwd
)"

API_DIR="${PROJECT_DIR}/api"
GENERATED_DIR="${PROJECT_DIR}/build/gui_generated"

PROTOC_EXECUTABLE="$(
    command -v protoc
)"

GRPC_CPP_PLUGIN="$(
    command -v grpc_cpp_plugin
)"

if [[ -z "${PROTOC_EXECUTABLE}" ]]
then
    echo "protoc was not found"
    exit 1
fi

if [[ -z "${GRPC_CPP_PLUGIN}" ]]
then
    echo "grpc_cpp_plugin was not found"
    exit 1
fi

mkdir -p "${GENERATED_DIR}"

"${PROTOC_EXECUTABLE}" \
    --proto_path="${API_DIR}" \
    --cpp_out="${GENERATED_DIR}" \
    --grpc_out="${GENERATED_DIR}" \
    --plugin="protoc-gen-grpc=${GRPC_CPP_PLUGIN}" \
    "${API_DIR}/rs485_service.proto"

echo "GUI gRPC files generated in:"
echo "${GENERATED_DIR}"
