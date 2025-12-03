#!/usr/bin/env bash

# This script calls Catch2 executable directly

# section=action_and_direction
# section=list_forward
section=forward_plus_two

# test_case=match_test

#----------------------------------------------------------------------

build_dir=build1-debug

set -x

# All tests
# cmake --build ${build_dir} && lldb ./${build_dir}/main/cd-deluxe

# By TEST_CASE
# cmake --build ${build_dir} && lldb ./${build_dir}/test/testmain -- ${test_case}

# By SECTION
cmake --build ${build_dir} && lldb ./${build_dir}/test/testmain -- -c ${section}
