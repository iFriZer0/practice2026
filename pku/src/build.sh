#!/bin/bash

readonly BUILD_DIRECTORY="build"

mkdir -p "$BUILD_DIRECTORY"
cmake -B "$BUILD_DIRECTORY"
