include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

if(PIP_INSTALL_REQUIREMENTS)
    run_pip("${PIP_INSTALL_REQUIREMENTS}" $ENV{SPL_PIP_REPOSITORY} $ENV{SPL_PIP_TRUSTED_HOST})
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
    include(CTest)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb --coverage")
    add_library(Unity STATIC
        tools/Unity/src/unity.c
    )

    target_include_directories(Unity PUBLIC
        tools/Unity/src
    )

    add_library(CMock
        tools/CMock/src/cmock.c
    )

    target_include_directories(CMock PUBLIC
        tools/CMock/src
    )

    target_link_libraries(CMock Unity)

    set(COV_OUT_VARIANT_HTML coverage/index.html)

    add_custom_command(
        OUTPUT ${COV_OUT_VARIANT_HTML}
        COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --add-tracefile \"${CMAKE_CURRENT_BINARY_DIR}/**/coverage.json\" --html --html-details --output ${COV_OUT_VARIANT_HTML}
        DEPENDS coverage
    )
    add_custom_target(coverage)
    add_custom_target(unittests DEPENDS coverage ${COV_OUT_VARIANT_HTML})
else()
    message(FATAL_ERROR "Invalid BUILD_KIT selected!")
endif()


## Things to be done at the very end of configure phase as if they would be at bottom of CMakelists.txt
cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL _spl_hook_end_of_configure())
function(_spl_hook_end_of_configure)
   if(CONAN__REQUIRES OR CONAN__BUILD_REQUIRES)
   endif() # CONAN__REQUIRES
endfunction(_spl_hook_end_of_configure)
