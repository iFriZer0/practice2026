#!/bin/bash

readonly BUILD_DIRECTORY="build"

cd "$(dirname "$0")" || exit 1
mkdir -p "$BUILD_DIRECTORY"
cmake -B "$BUILD_DIRECTORY"
cmake --build "$BUILD_DIRECTORY"
