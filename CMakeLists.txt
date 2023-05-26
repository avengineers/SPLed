# cmake project definition
cmake_minimum_required(VERSION 3.22.0)

set(VARIANT ${FLAVOR}/${SUBSYSTEM} CACHE STRING "Variant to build.")
set(BUILD_KIT prod CACHE STRING "Target Group to build.")

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/config.cmake)

project(${VARIANT} CXX C ASM)

# This step will also triger the Kconfig generation
include(build/spl-core/cmake/spl.cmake)

# TODO: check the configuration for the required tools and install them
# this can also be done with a Python module/extension
# this extension can generate a file to track the installed tools
# The KConfig file should retrigger a cmake configure.

# TODO: trigger the extensions cmake generation
include(modules/spl_extension_zip/index.cmake)

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/parts.cmake)

# TODO: refactor/remove; we want to explicitly add includes where required
foreach(component_name ${COMPONENT_NAMES})
    target_include_directories(${component_name} PUBLIC ${target_include_directories__INCLUDES})
endforeach()
