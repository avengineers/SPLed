# Define generated header name
set(KCONFIG_OUT_DIR ${PROJECT_BINARY_DIR}/include/generated)
set(AUTOCONF_H ${KCONFIG_OUT_DIR}/autoconf.h)
set(AUTOCONF_JSON ${KCONFIG_OUT_DIR}/autoconf.json)
set(AUTOCONF_CMAKE ${KCONFIG_OUT_DIR}/autoconf.cmake)
set(KCONFIG_ROOT_FILE ${PROJECT_SOURCE_DIR}/KConfig)
set(KCONFIG_VARIANT_FILE ${PROJECT_SOURCE_DIR}/variants/${VARIANT}/config.txt)

if(EXISTS ${KCONFIG_ROOT_FILE})
    # Re-configure autoconf.h changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

    if(EXISTS ${KCONFIG_VARIANT_FILE})
        set(_kconfig_variant_file_option --kconfig_config_file ${KCONFIG_VARIANT_FILE})
    else()
        message(STATUS "No KConfig variant configuration file found: ${KCONFIG_VARIANT_FILE}")
    endif()

    execute_process(
        COMMAND python ${SPL_CORE_PYTHON_MODULES_DIRECTORY}/kconfig/kconfig.py
        --kconfig_model_file ${KCONFIG_ROOT_FILE} ${_kconfig_variant_file_option}
        --out_header_file ${AUTOCONF_H}
        --out_json_file ${AUTOCONF_JSON}
        --out_cmake_file ${AUTOCONF_CMAKE}
        COMMAND_ECHO STDOUT
        RESULT_VARIABLE ret
    )

    if(NOT "${ret}" STREQUAL "0")
        message(FATAL_ERROR "Executing KConfig generation failed with return code: ${ret}")
    endif()

    # Make the generated files location public
    include_directories(${KCONFIG_OUT_DIR})
else()
    message(STATUS "No KConfig feature model file found. Skip KConfig generation.")
endif()