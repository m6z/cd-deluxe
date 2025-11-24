#!/usr/bin/env bash

# This script calls Catch2 executable directly

# section=action_and_direction
section=list_forward

# cmake --build build1-debug/ && lldb ./build1-debug/test/testmain -- -c ${section}

set -x

cmake --build build1-debug/ && lldb ./build1-debug/main/cd-deluxe
