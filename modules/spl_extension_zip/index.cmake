# call the extension generate function
execute_process(
    COMMAND python ${CMAKE_CURRENT_LIST_DIR}/src/my_extension.py --generate --project_root_dir ${CMAKE_SOURCE_DIR} --variant ${VARIANT} --build_kit ${BUILD_KIT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    RESULT_VARIABLE result
)

# check if the command succeeded
if(result)
    message(FATAL_ERROR "Failed to generate extension: ${error}")
else()
    message(INFO "CMake file generation OK. Stdout: ${output}")
endif()

# include the generated cmake file
include(${CMAKE_BINARY_DIR}/my_extension.cmake)

# Trigger CMake reconfigure if the extension generator script changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/my_extension.py)
