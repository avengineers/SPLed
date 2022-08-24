# cmake project definition
cmake_minimum_required(VERSION 3.22.0)

set(VARIANT ${FLAVOR}/${SUBSYSTEM} CACHE STRING "Variant to build.")
set(BUILD_KIT prod CACHE STRING "Target Group to build.")

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/config.cmake)

project(${VARIANT} C ASM)


include(build/spl-core/cmake/spl.cmake)
# spl_install_extensions(...)
# run extensions

# get variant specific feature configuration for build or test
set(GEN_DIR ${CMAKE_SOURCE_DIR}/build/${VARIANT}/gen)
include_directories(${GEN_DIR})
configure_file(${CMAKE_SOURCE_DIR}/variants/config.h.in ${GEN_DIR}/config.h @ONLY)
include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/parts.cmake)

# get external git dependencies
spl_checkout_git_submodules()

# TODO: refactor/remove; we want to explicitly add includes where required
foreach(component_name ${COMPONENT_NAMES})
    target_include_directories(${component_name} PUBLIC ${target_include_directories__INCLUDES})
endforeach()
