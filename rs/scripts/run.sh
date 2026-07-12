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

EXECUTABLE="${PROJECT_DIR}/build/rs485_microservice"

DRIVER_ENDPOINT="${1:-127.0.0.1:50051}"
SERVICE_ENDPOINT="${2:-0.0.0.0:50052}"

if [[ ! -x "${EXECUTABLE}" ]]
then
    echo "RS-485 microservice is not built"
    echo "Run: ${PROJECT_DIR}/scripts/build.sh"
    exit 1
fi

exec "${EXECUTABLE}" \
    "${DRIVER_ENDPOINT}" \
    "${SERVICE_ENDPOINT}"