#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MKO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
REPO_ROOT="$(cd "${MKO_ROOT}/.." && pwd)"

OUT_DIR="${REPO_ROOT}/gui/src/generated/mko"
PROTO_FILE="${MKO_ROOT}/api/mko.proto"
GRPC_CPP_PLUGIN="$(command -v grpc_cpp_plugin)"

mkdir -p "${OUT_DIR}"

protoc \
  --proto_path="${MKO_ROOT}/api" \
  --cpp_out="${OUT_DIR}" \
  --grpc_out="${OUT_DIR}" \
  --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}" \
  "${PROTO_FILE}"
