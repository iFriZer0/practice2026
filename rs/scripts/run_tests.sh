#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(
    cd "$(dirname "${BASH_SOURCE[0]}")/.."
    pwd
)"

BUILD_DIR="${ROOT_DIR}/build-test"

if [[ ! -f "${BUILD_DIR}/CTestTestfile.cmake" ]]
then
    echo "Automatic tests are not built."
    echo "Run ./scripts/build_tests.sh first."
    exit 1
fi

export NO_PROXY="127.0.0.1,localhost"
export no_proxy="127.0.0.1,localhost"

unset grpc_proxy
unset GRPC_PROXY
unset http_proxy
unset HTTP_PROXY
unset https_proxy
unset HTTPS_PROXY
unset all_proxy
unset ALL_PROXY

ctest \
    --test-dir "${BUILD_DIR}" \
    --output-on-failure
