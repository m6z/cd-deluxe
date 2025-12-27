#!/usr/bin/env bash

# helper aliases:
#   alias tes='pushd ~/base/development/cd-deluxe_cpp20 >/dev/null && ./go-test.sh'
#   alias tes='pushd ~/base/cdd_v2/cd-deluxe >/dev/null && ./go-test.sh'

# test_pattern=cdd_options_test
# test_pattern=cdd2_test
# test_pattern=match_test
test_pattern=cleanup_test

#----------------------------------------------------------------------
set -x

# All tests
cmake --build build1 && ctest --test-dir build1

# Filtered tests
# cmake --build build1 && ctest --test-dir build1 -R ${test_pattern}
