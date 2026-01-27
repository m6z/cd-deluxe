@echo off
setlocal

:: set test_pattern=cdd2_test
set test_pattern=match_test
:: set test_pattern=util_test
:: set test_pattern=get_path_components_tmp

:: Note: ctest does not support selection by SECTION, only by TEST_CASE

:: ----------------------------------------------------------------------

:: if test pattern specified, run only matching tests
if "%test_pattern%"=="" (
    cmake --build build1 && ctest --test-dir build1
) else (
    cmake --build build1 && ctest --test-dir build1 -R %test_pattern%
)
