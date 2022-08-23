# given
cmake_minimum_required(VERSION 3.22.0)
project(test-common-cmake C ASM)

set(VARIANT mytestvariant)

# when
include(${CMAKE_CURRENT_LIST_DIR}/../../common.cmake)

# expect
if (NOT LINK_TARGET_NAME STREQUAL "link")
    message(FATAL_ERROR "Failing Test case: LINK_TARGET_NAME = ${LINK_TARGET_NAME}.")
endif()

set(string_with_underscore "abc/def/ghi")
slash_to_underscore(string_with_underscore ${string_with_underscore})
if (NOT string_with_underscore STREQUAL "abc_def_ghi")
    message(FATAL_ERROR "Failing Test case: string_with_underscore = ${string_with_underscore}.")
endif()

get_absolute_path(absolute_path "/test")
string(FIND "${absolute_path}" "cmake/test/common.cmake/test" out)
if (NOT "${out}" GREATER 0)
    message(FATAL_ERROR "Failing Test case: absolute_path = ${absolute_path}.")
endif()

string(FIND "${absolute_path}" "C:" out)
if (NOT "${out}" EQUAL 0)
    message(FATAL_ERROR "Failing Test case: absolute_path = ${absolute_path}.")
endif()
