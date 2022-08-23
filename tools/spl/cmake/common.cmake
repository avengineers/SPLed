# variables
set(spl_install_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

set(BUILD_BINARY_DIRECTORY ${CMAKE_BINARY_DIR})
set(LINK_TARGET_NAME link)
string(REPLACE "/" "_" BINARY_BASENAME ${VARIANT})

# macros and functions
macro(slash_to_underscore out in)
    string(REGEX REPLACE "/" "_" ${out} ${in})
endmacro()

macro(get_absolute_path out in)
    string(FIND ${in} "/" position)

    if(position STREQUAL 0)
        # Assumption: path starting with '/' means relative to project root
        cmake_path(CONVERT ${PROJECT_SOURCE_DIR}/${in} TO_CMAKE_PATH_LIST ${out} NORMALIZE)
    else()
        cmake_path(CONVERT ${CMAKE_CURRENT_LIST_DIR}/${in} TO_CMAKE_PATH_LIST ${out} NORMALIZE)
    endif()
endmacro()

macro(checkout_git_submodules)
    #first time checkout submodules, ignore if existing
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        ERROR_QUIET
    )

    #consecutive times just pull changes
    execute_process(
        COMMAND git submodule update --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endmacro()

macro(add_component component_path)
    slash_to_underscore(component_name ${component_path})
    add_subdirectory(${CMAKE_SOURCE_DIR}/${component_path})

    if(BUILD_KIT STREQUAL prod)
        target_link_libraries(${LINK_TARGET_NAME} ${component_name})
    endif()
endmacro()

macro(add_source fileName)
    get_absolute_path(to_be_appended ${fileName})
    list(APPEND SOURCES ${to_be_appended})
endmacro()

macro(add_include includeDirectory)
    get_absolute_path(to_be_appended ${includeDirectory})
    list(APPEND INCLUDES ${to_be_appended})
endmacro()

macro(add_test_source fileName)
    get_absolute_path(to_be_appended ${fileName})
    list(APPEND TEST_SOURCES ${to_be_appended})
endmacro()

macro(create_mocks fileName)
    if(BUILD_KIT STREQUAL test)
        get_absolute_path(FILE_TO_BE_MOCKED ${fileName})
        cmake_path(GET FILE_TO_BE_MOCKED FILENAME FILE_BASE_NAME)
        cmake_path(REMOVE_EXTENSION FILE_BASE_NAME LAST_ONLY OUTPUT_VARIABLE FILE_BASE_NAME_WITHOUT_EXTENSION)
        file(RELATIVE_PATH component_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
        if($Env{SPL_CMOCK_CONFIG_FILE})
            SET(CMOCK_CONFIG_OPT -o${SPL_CMOCK_CONFIG_FILE})
        else()
            SET(CMOCK_CONFIG_OPT -o${PROJECT_SOURCE_DIR}/cmock-config.yml)
        endif()
        add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/build/${VARIANT}/test/${component_path}/mocks/mock_${FILE_BASE_NAME_WITHOUT_EXTENSION}.c
            COMMAND cmd /C "ruby ${PROJECT_SOURCE_DIR}/tools/CMock/lib/cmock.rb ${CMOCK_CONFIG_OPT} ${FILE_TO_BE_MOCKED}"
            DEPENDS ${FILE_TO_BE_MOCKED}
        )
        add_include(/build/${VARIANT}/test/${component_path}/mocks)
        add_test_source(/build/${VARIANT}/test/${component_path}/mocks/mock_${FILE_BASE_NAME_WITHOUT_EXTENSION}.c)
    endif()
endmacro()

macro(create_component)
    file(RELATIVE_PATH component_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    slash_to_underscore(component_name ${component_path})
    add_library(${component_name} OBJECT ${SOURCES})
    target_compile_options(${component_name} PRIVATE ${VARIANT_ADDITIONAL_COMPILE_C_FLAGS})

    list(APPEND COMPONENT_NAMES ${component_name})
    set(COMPONENT_NAMES ${COMPONENT_NAMES} PARENT_SCOPE)

    list(APPEND target_include_directories__INCLUDES ${CMAKE_CURRENT_LIST_DIR}/src)

    foreach(source ${SOURCES})
        get_filename_component(path ${source} DIRECTORY)
        list(APPEND target_include_directories__INCLUDES ${path})
    endforeach()

    list(APPEND target_include_directories__INCLUDES ${INCLUDES})
    list(REMOVE_DUPLICATES target_include_directories__INCLUDES)
    set(target_include_directories__INCLUDES ${target_include_directories__INCLUDES} PARENT_SCOPE)

    if((BUILD_KIT STREQUAL test) AND TEST_SOURCES)
        set(exe_name ${component_name}_test)
        add_test_suite(${TEST_SOURCES})
    endif()
endmacro()

macro(add_test_suite)
    add_executable(${exe_name}
        ${ARGN}
    )

    set(TEST_OUT_JUNIT junit.xml)
    add_custom_command(
        OUTPUT ${TEST_OUT_JUNIT}
        COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CTEST_ARGUMENTS} --output-junit ${TEST_OUT_JUNIT}
        DEPENDS ${exe_name}
    )

    set(COV_OUT_JSON coverage.json)
    add_custom_command(
        OUTPUT ${COV_OUT_JSON}
        COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --filter ${CMAKE_CURRENT_LIST_DIR}/src --json --output ${COV_OUT_JSON} ${GCOVR_ADDITIONAL_OPTIONS}
        DEPENDS ${TEST_OUT_JUNIT}
    )

    set(COV_OUT_HTML coverage/index.html)
    add_custom_command(
        OUTPUT ${COV_OUT_HTML}
        COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --add-tracefile ${COV_OUT_JSON} --html --html-details --output ${COV_OUT_HTML} ${GCOVR_ADDITIONAL_OPTIONS}
        DEPENDS ${COV_OUT_JSON}
    )

    add_custom_target(${component_name}_coverage DEPENDS ${COV_OUT_HTML})
    add_custom_target(${component_name}_unittests DEPENDS ${component_name}_coverage)
    add_dependencies(coverage ${component_name}_coverage)

    target_link_libraries(${exe_name}
        ${component_name}
        CMock
    )

    add_test(${component_name}_test ${exe_name})
endmacro()


macro(spl_add_conan_requires requirement)
    list(APPEND CONAN__REQUIRES ${requirement})
endmacro(spl_add_conan_requires)

macro(spl_add_conan_build_requires requirement)
    list(APPEND CONAN__BUILD_REQUIRES ${requirement})
endmacro(spl_add_conan_build_requires)

macro(spl_add_conan_install_settings settings)
    list(APPEND CONAN_INSTALL_SETTINGS ${settings})
endmacro(spl_add_conan_install_settings)




macro(spl_run_conan)
    # This is the wrapper-code
    include(${spl_install_dir}/conan.cmake)
    # This replaces file conanfile.txt
    conan_cmake_configure(
        BUILD_REQUIRES
        ${CONAN__BUILD_REQUIRES}
        REQUIRES
        ${CONAN__REQUIRES}
        GENERATORS
        cmake_paths
        virtualrunenv
    )

    conan_config_install(
        ITEM $ENV{SPL_CONAN_CONFIG_URL}
    )

    # This replaces the call of command "conan install" on the command line
    conan_cmake_install(
        PATH_OR_REFERENCE .
        SETTINGS
        ${CONAN_INSTALL_SETTINGS}
    )
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

    # This is the ninja hack to get paths of conan packages
    set_ninja_wrapper_as_cmake_make()
endmacro(spl_run_conan)

macro(run_pip PIP_INSTALL_REQUIREMENTS)
    message("Execute PIP")
    execute_process(
        COMMAND python -m pip install --trusted-host $ENV{SPL_PIP_TRUSTED_HOST} --index-url $ENV{SPL_PIP_REPOSITORY} ${PIP_INSTALL_REQUIREMENTS}
        COMMAND_ERROR_IS_FATAL ANY
    )
endmacro()

macro(set_ninja_wrapper_as_cmake_make)
    set(NINJA_WRAPPER ${CMAKE_SOURCE_DIR}/build/${VARIANT}/${BUILD_KIT}/ninja_wrapper.bat)
    file(WRITE ${NINJA_WRAPPER}
        "@echo off
@call %~dp0%/activate_run.bat
@ninja %*
@call %~dp0%/deactivate_run.bat")
    set(CMAKE_MAKE_PROGRAM ${NINJA_WRAPPER} CACHE FILEPATH "Custom ninja wrapper to activate the Conan virtual environment" FORCE)
endmacro()

macro(spl_install_extension NAME VERSION)
    run_pip(${NAME}==${VERSION})
    execute_process(
        COMMAND python -c "import pathlib, ${NAME}; print(pathlib.Path(${NAME}.__file__).resolve().parent, end='')"
        OUTPUT_VARIABLE ${NAME}_EXTENSION_PATH
    )
    set(${NAME}_CMAKE_MODULE_PATH ${${NAME}_EXTENSION_PATH}/cmake)
    list(APPEND CMAKE_MODULE_PATH ${${NAME}_CMAKE_MODULE_PATH})
    include(${NAME})
endmacro()
