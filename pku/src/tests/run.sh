#!/bin/bash

cd "$(dirname "$0")" || exit 1
python integration_tests.py
