# cmake project definition
cmake_minimum_required(VERSION 3.22.0)

set(VARIANT ${FLAVOR}/${SUBSYSTEM} CACHE STRING "Variant to build.")
set(BUILD_KIT prod CACHE STRING "Target Group to build.")

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/config.cmake)

project(${VARIANT} CXX C ASM)

if(DEFINED ENV{SPLCORE_PATH})
    message(WARNING "SPLCORE_PATH defined! Using SPL Core from here: $ENV{SPLCORE_PATH}")
    include($ENV{SPLCORE_PATH}/cmake/spl.cmake)
else()
    include(modules/spl/cmake/spl.cmake)
endif()

include(modules/spl-extension-example/src/cmake/index.cmake)
spl_extension_example(
    INPUTS ${USE_COMPONENT_A_INTERFACE} ${PART_NUMBER}
    OUTPUTS out3 out4
)

include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/parts.cmake)

# TODO: refactor/remove; we want to explicitly add includes where required
foreach(component_name ${COMPONENT_NAMES})
    target_include_directories(${component_name} PUBLIC ${target_include_directories__INCLUDES})
endforeach()
