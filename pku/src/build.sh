#!/bin/bash

readonly BUILD_DIRECTORY="build"

cd "$(dirname "$0")" || exit 1
mkdir -p "$BUILD_DIRECTORY"
cmake -B "$BUILD_DIRECTORY"
cd "$BUILD_DIRECTORY" || exit 1
make
cd ".." || exit 1
