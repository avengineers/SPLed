include(../spl_test.cmake)

# mocks
spl_test_create_mock(add_executable)
spl_test_create_mock(target_compile_options)
spl_test_create_mock(set_target_properties)


# given
cmake_minimum_required(VERSION 3.22.0)
project(test C ASM)

set(VARIANT mytestvariant)
set(BUILD_KIT prod)


# when
include(${CMAKE_CURRENT_LIST_DIR}/../../spl.cmake)


# expect
spl_test_expect_is_called(add_executable 1)
spl_test_expect_is_called(target_compile_options 1)
spl_test_expect_is_called(set_target_properties 1)
