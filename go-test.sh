#!/usr/bin/env bash

# test_pattern=cdd_options_test
# test_pattern=cdd2_test
test_pattern=match_test

set -x
# cmake --build build1 && ctest --test-dir build1
cmake --build build1 && ctest --test-dir build1 -R ${test_pattern}
