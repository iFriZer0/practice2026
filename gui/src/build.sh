#!/bin/bash

cd "$(dirname "$0")" || exit 1
qmake6
make
