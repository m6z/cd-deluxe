#!/usr/bin/env bash

# helper alias:
#   alias deb='pushd ~/base/development/cd-deluxe_cpp20 >/dev/null && ./go-debug.sh'

# This script calls Catch2 executable directly

# section=action_and_direction
# section=list_forward
# section=forward_plus_two
# section=simple_match_forward
# section=delete_one
# section=garbage_collect
section=up_a

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
