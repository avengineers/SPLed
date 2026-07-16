# cmake project definition
cmake_minimum_required(VERSION 3.24.0)
cmake_policy(VERSION 3.24)

# default build kit is prod (when not specified as command line argument)
set(BUILD_KIT prod CACHE STRING "Target Group to build.")

# Include variant's configuration
include(${CMAKE_SOURCE_DIR}/variants/${VARIANT}/config.cmake)

if(BUILD_KIT STREQUAL prod)
    project(${VARIANT} C ASM)
else()
    # C++ project due to GTest usage
    project(${VARIANT} C ASM CXX)
endif()

# Fetch all external dependencies into modules directory
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/build/modules CACHE INTERNAL "")
set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

# Find spl-core location dynamically (works on both Windows and Linux)
if(WIN32)
    set(_VENV_PYTHON "${PROJECT_SOURCE_DIR}/.venv/Scripts/python.exe")
else()
    set(_VENV_PYTHON "${PROJECT_SOURCE_DIR}/.venv/bin/python")
endif()
execute_process(
    COMMAND "${_VENV_PYTHON}" -c "import spl_core, os; print(os.path.dirname(spl_core.__file__))"
    OUTPUT_VARIABLE SPL_CORE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE SPL_CORE_RESULT
)
if(NOT SPL_CORE_RESULT EQUAL 0)
    message(FATAL_ERROR "Could not find spl_core Python package. Make sure the virtual environment is set up.")
endif()
# Export SPL_CORE_DIR so KConfig source statements can use $(SPL_CORE_DIR)
set(ENV{SPL_CORE_DIR} "${SPL_CORE_DIR}")
# Ensure venv python is found first (spl_core's kconfig.cmake invokes bare 'python')
cmake_path(GET _VENV_PYTHON PARENT_PATH _VENV_BIN_DIR)
if(WIN32)
    set(ENV{PATH} "${_VENV_BIN_DIR};$ENV{PATH}")
else()
    set(ENV{PATH} "${_VENV_BIN_DIR}:$ENV{PATH}")
endif()
include("${SPL_CORE_DIR}/spl.cmake")

# The object_deps_report extension is currently Windows-only: its index.cmake
# hardcodes the runner as "object_deps_report.exe", which does not exist on
# Linux/macOS (the console script is "object_deps_report"). Guard the include to
# Windows until the upstream .exe hardcoding is fixed (tracked upstream in the
# object_deps_report package).
if(BUILD_KIT STREQUAL prod AND WIN32)
    execute_process(
        COMMAND "${_VENV_PYTHON}" -c "import object_deps_report, os; print(os.path.dirname(object_deps_report.__file__))"
        OUTPUT_VARIABLE OBJECT_DEPS_REPORT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE OBJECT_DEPS_REPORT_RESULT
    )
    if(NOT OBJECT_DEPS_REPORT_RESULT EQUAL 0)
        message(FATAL_ERROR "Could not find object_deps_report Python package. Make sure the virtual environment is set up.")
    endif()
    include("${OBJECT_DEPS_REPORT_DIR}/index.cmake")
endif()

# Include variant specific parts/components definitions
include(${PROJECT_SOURCE_DIR}/variants/${VARIANT}/parts.cmake)
