# cmake project definition
cmake_minimum_required(VERSION 3.24.0)
cmake_policy(VERSION 3.24)

# configure the current variant to be build
set(VARIANT ${FLAVOR}/${SUBSYSTEM} CACHE STRING "Variant to build.")
set(BUILD_KIT prod CACHE STRING "Target Group to build.")

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/config.cmake)

if(BUILD_KIT STREQUAL prod)
    project(${VARIANT} C ASM)
else()
    project(${VARIANT} C ASM CXX)
    set(VARIANT_ADDITIONAL_COMPILE_C_FLAGS -Dstatic=)
endif()

# Fetch all external dependencies into modules directory
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/build/modules CACHE INTERNAL "")
set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

# Fetch spl-core
FetchContent_Declare(
    spl-core
    GIT_REPOSITORY https://git.marquardt.de/scm/sple/spl-core.git
    GIT_TAG v3.7.0
)
FetchContent_MakeAvailable(spl-core)

# Global config for spl-core
set(SPL_GTEST_URL https://git.marquardt.de/scm/sple/googletest.git)
set(SPL_GTEST_TAG v1.14.0)

# Include spl-core
include(${spl-core_SOURCE_DIR}/cmake/spl.cmake)

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/parts.cmake)

# TODO: refactor/remove; we want to explicitly add includes where required
foreach(component_name ${COMPONENT_NAMES})
    target_include_directories(${component_name} PUBLIC ${target_include_directories__INCLUDES})
endforeach()
