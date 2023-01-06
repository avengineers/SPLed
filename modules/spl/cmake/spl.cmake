include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)
#  Define the SPL Core root directory to be used to refer to files
# relative to the repository root.
set(SPL_CORE_ROOT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..)
set(SPL_CORE_PYTHON_MODULES_DIRECTORY ${SPL_CORE_ROOT_DIRECTORY}/src)

if(PIP_INSTALL_REQUIREMENTS)
    run_pip("${PIP_INSTALL_REQUIREMENTS}" $ENV{SPL_PIP_REPOSITORY} $ENV{SPL_PIP_TRUSTED_HOST})
endif() 

# Include and run KConfig
include(${CMAKE_CURRENT_LIST_DIR}/kconfig.cmake)

if(EXISTS ${AUTOCONF_CMAKE})
    include(${AUTOCONF_CMAKE})
endif()

if(BUILD_KIT STREQUAL prod)
    # set default link target output name
    if (LINK_FILE_BASENAME)
        message("LINK_FILE is set to '${LINK_FILE_BASENAME}' and will be used.")
    else()
        set(LINK_FILE_BASENAME main)
    endif()

    # set default link target output name
    if (LINK_FILE_EXTENSION)
        message("LINK_FILE_EXTENSION is set to '${LINK_FILE_EXTENSION}' and will be used.")
    else()
        set(LINK_FILE_EXTENSION .exe)
    endif()

    # add variant specific linker script if defined
    if(VARIANT_LINKER_FILE)
        list(APPEND LINK_TARGET_DEPENDS ${VARIANT_LINKER_FILE})
    endif()

    # create executable
    add_executable(${LINK_TARGET_NAME} ${LINK_TARGET_DEPENDS})
    target_compile_options(${LINK_TARGET_NAME} PRIVATE ${VARIANT_ADDITIONAL_LINK_FLAGS})
    set_target_properties(${LINK_TARGET_NAME} PROPERTIES
        OUTPUT_NAME ${LINK_FILE_BASENAME}
        SUFFIX ${LINK_FILE_EXTENSION}
        LINK_DEPENDS "${LINK_TARGET_DEPENDS}"
    )
elseif(BUILD_KIT STREQUAL test)
    _spl_get_google_test()
    include(CTest)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

    add_custom_target(coverage)
else()
    message(FATAL_ERROR "Invalid BUILD_KIT selected!")
endif()


## Things to be done at the very end of configure phase as if they would be at bottom of CMakelists.txt
cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL _spl_hook_end_of_configure())
function(_spl_hook_end_of_configure)
    _spl_coverage_create_overall_report()

   if(CONAN__REQUIRES OR CONAN__BUILD_REQUIRES)
   endif() # CONAN__REQUIRES
endfunction(_spl_hook_end_of_configure)


# TODO: find smarter way to open correct kconfig gui; maybe integrated
set(_CONFIGURATION_TARGET configuration.stamp)
add_custom_command(
    OUTPUT ${_CONFIGURATION_TARGET}
    COMMAND set "KCONFIG_CONFIG=${CMAKE_SOURCE_DIR}/variants/${VARIANT}/config.txt" && cd ${CMAKE_SOURCE_DIR} && guiconfig
)

add_custom_target(configuration DEPENDS ${_CONFIGURATION_TARGET})