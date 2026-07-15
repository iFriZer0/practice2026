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
CONFIG_PATH="${1:-${PROJECT_DIR}/config/config.yaml}"

if [[ ! -x "${EXECUTABLE}" ]]
then
    echo "RS-485 microservice is not built"
    echo "Run: ${PROJECT_DIR}/scripts/build.sh"
    exit 1
fi

if [[ ! -f "${CONFIG_PATH}" ]]
then
    echo "RS-485 config file was not found: ${CONFIG_PATH}"
    exit 1
fi

export RS485_CONFIG_PATH="${CONFIG_PATH}"

exec "${EXECUTABLE}"
