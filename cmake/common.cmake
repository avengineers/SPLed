# variables
set(spl_install_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

set(BUILD_BINARY_DIRECTORY ${CMAKE_BINARY_DIR})
set(LINK_TARGET_NAME link)
string(REPLACE "/" "_" BINARY_BASENAME ${VARIANT})

# macros and functions
macro(_spl_slash_to_underscore out in)
    string(REGEX REPLACE "/" "_" ${out} ${in})
endmacro()

macro(_spl_get_absolute_path out in)
    if(IS_ABSOLUTE ${in})
        cmake_path(CONVERT ${in} TO_CMAKE_PATH_LIST ${out} NORMALIZE)
    else()
        cmake_path(CONVERT ${CMAKE_CURRENT_LIST_DIR}/${in} TO_CMAKE_PATH_LIST ${out} NORMALIZE)
    endif()
endmacro()

macro(spl_add_component component_path)
    _spl_slash_to_underscore(component_name ${component_path})
    add_subdirectory(${CMAKE_SOURCE_DIR}/${component_path})

    if(BUILD_KIT STREQUAL prod)
        target_link_libraries(${LINK_TARGET_NAME} ${component_name})
    endif()
endmacro()

macro(spl_add_source fileName)
    _spl_get_absolute_path(to_be_appended ${fileName})
    list(APPEND SOURCES ${to_be_appended})
endmacro()

macro(spl_add_include includeDirectory)
    _spl_get_absolute_path(to_be_appended ${includeDirectory})
    list(APPEND INCLUDES ${to_be_appended})
endmacro()

macro(spl_add_test_source fileName)
    _spl_get_absolute_path(to_be_appended ${fileName})
    list(APPEND TEST_SOURCES ${to_be_appended})
endmacro()

macro(_spl_get_google_test)
    # GoogleTest requires at least C++14
    set(CMAKE_CXX_STANDARD 14)

    set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/../../_deps CACHE INTERNAL "")

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )

    if(WIN32)
        # Prevent overriding the parent project's compiler/linker
        # settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    include(GoogleTest)

    find_package(Threads REQUIRED)

    enable_testing()
endmacro(_spl_get_google_test)

macro(spl_create_mocks fileName)
endmacro()

macro(spl_create_component)
    file(RELATIVE_PATH component_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    _spl_slash_to_underscore(component_name ${component_path})
    add_library(${component_name} OBJECT ${SOURCES})

    if(BUILD_KIT STREQUAL test)
        target_compile_options(${component_name} PRIVATE ${VARIANT_ADDITIONAL_COMPILE_C_FLAGS} -ggdb --coverage)
    else()
        target_compile_options(${component_name} PRIVATE ${VARIANT_ADDITIONAL_COMPILE_C_FLAGS})
    endif()

    list(APPEND COMPONENT_NAMES ${component_name})
    set(COMPONENT_NAMES ${COMPONENT_NAMES} PARENT_SCOPE)

    list(APPEND target_include_directories__INCLUDES ${CMAKE_CURRENT_LIST_DIR}/src)
    list(APPEND target_include_directories__INCLUDES ${CMAKE_CURRENT_BINARY_DIR})

    foreach(source ${SOURCES})
        get_filename_component(path ${source} DIRECTORY)
        list(APPEND target_include_directories__INCLUDES ${path})
    endforeach()

    list(APPEND target_include_directories__INCLUDES ${INCLUDES})
    list(REMOVE_DUPLICATES target_include_directories__INCLUDES)
    set(target_include_directories__INCLUDES ${target_include_directories__INCLUDES} PARENT_SCOPE)

    if((BUILD_KIT STREQUAL test) AND TEST_SOURCES)
        set(exe_name ${component_name}_test)
        _spl_add_test_suite("${SOURCES}" ${TEST_SOURCES})
    endif()
endmacro()

macro(_spl_set_coverage_create_overall_report_is_necessary)
    set(_SPL_COVERAGE_CREATE_OVERALL_REPORT_IS_NECESSARY TRUE PARENT_SCOPE)
endmacro(_spl_set_coverage_create_overall_report_is_necessary)

# TODO: replace global json filter pattern with config file listing components' json files explicitely
function(_spl_coverage_create_overall_report)
    if(_SPL_COVERAGE_CREATE_OVERALL_REPORT_IS_NECESSARY)
        set(COV_OUT_VARIANT_HTML coverage/index.html)
        add_custom_command(
            OUTPUT ${COV_OUT_VARIANT_HTML}
            COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --add-tracefile \"${CMAKE_CURRENT_BINARY_DIR}/**/coverage.json\" --html --html-details --output ${COV_OUT_VARIANT_HTML}
            DEPENDS coverage
        )
        add_custom_target(unittests DEPENDS coverage ${COV_OUT_VARIANT_HTML})
    else(_SPL_COVERAGE_CREATE_OVERALL_REPORT_IS_NECESSARY)
        add_custom_target(unittests)
    endif(_SPL_COVERAGE_CREATE_OVERALL_REPORT_IS_NECESSARY)
endfunction(_spl_coverage_create_overall_report)

macro(_spl_add_test_suite PROD_SRC TEST_SOURCES)
    _spl_set_coverage_create_overall_report_is_necessary()

    set(PROD_PARTIAL_LINK prod_partial_${component_name}.obj)
    set(MOCK_SRC mockup_${component_name}.cc)

    add_executable(${exe_name}
        ${TEST_SOURCES}
        ${MOCK_SRC}
    )

    # Produce debugging information to be able to set breakpoints while debugging.
    target_compile_options(${exe_name} PRIVATE -ggdb)

    target_link_options(${exe_name}
        PRIVATE -ggdb --coverage
    )

    add_custom_command(
        OUTPUT ${PROD_PARTIAL_LINK}
        COMMAND ${CMAKE_CXX_COMPILER} -r -nostdlib -o ${PROD_PARTIAL_LINK} $<TARGET_OBJECTS:${component_name}>
        COMMAND_EXPAND_LISTS
        VERBATIM
        DEPENDS $<TARGET_OBJECTS:${component_name}>
    )

    set(prop "$<TARGET_PROPERTY:${component_name},INCLUDE_DIRECTORIES>")
    add_custom_command(
        OUTPUT ${MOCK_SRC}
        BYPRODUCTS mockup_${component_name}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMAND python -m hammocking --suffix _${component_name} --sources ${PROD_SRC} --plink ${CMAKE_CURRENT_BINARY_DIR}/${PROD_PARTIAL_LINK} --outdir ${CMAKE_CURRENT_BINARY_DIR} "$<$<BOOL:${prop}>:-I$<JOIN:${prop},;-I>>" -x c
        COMMAND_EXPAND_LISTS
        VERBATIM
        DEPENDS
        ${PROD_PARTIAL_LINK}
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
        COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --filter ${CMAKE_CURRENT_LIST_DIR}/src --json --output ${COV_OUT_JSON} ${GCOVR_ADDITIONAL_OPTIONS} ${CMAKE_CURRENT_BINARY_DIR}
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
        GTest::gtest_main
        GTest::gmock_main
        Threads::Threads
    )

    gtest_discover_tests(${exe_name})
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
    if(CONAN__BUILD_REQUIRES OR CONAN__REQUIRES)
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

        if(DEFINED ENV{SPL_CONAN_CONFIG_URL})
            conan_config_install(
                ITEM $ENV{SPL_CONAN_CONFIG_URL}
            )
        endif()

        # This replaces the call of command "conan install" on the command line
        conan_cmake_install(
            PATH_OR_REFERENCE .
            SETTINGS
            ${CONAN_INSTALL_SETTINGS}
        )
        include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

        # This is the ninja hack to get paths of conan packages
        _spl_set_ninja_wrapper_as_cmake_make()
    endif()
endmacro(spl_run_conan)

macro(spl_run_pip PIP_INSTALL_REQUIREMENTS)
    message("Execute PIP")
    execute_process(
        COMMAND python -m pip install --trusted-host $ENV{SPL_PIP_TRUSTED_HOST} --index-url $ENV{SPL_PIP_REPOSITORY} ${PIP_INSTALL_REQUIREMENTS}
        COMMAND_ERROR_IS_FATAL ANY
    )
endmacro()

macro(_spl_set_ninja_wrapper_as_cmake_make)
    set(NINJA_WRAPPER ${CMAKE_SOURCE_DIR}/build/${VARIANT}/${BUILD_KIT}/ninja_wrapper.bat)
    file(WRITE ${NINJA_WRAPPER}
        "@echo off
@call %~dp0%/activate_run.bat
@ninja %*
@call %~dp0%/deactivate_run.bat")
    set(CMAKE_MAKE_PROGRAM ${NINJA_WRAPPER} CACHE FILEPATH "Custom ninja wrapper to activate the Conan virtual environment" FORCE)
endmacro()

macro(spl_install_extensions)
    # Strange hack found here: https://stackoverflow.com/questions/5248749/passing-a-list-to-a-cmake-macro
    set(_ARGN_LIST ${ARGN})

    foreach(arg IN LISTS _ARGN_LIST)
        string(REPLACE "@" ";" arglist ${arg})
        list(LENGTH arglist len)

        if(len EQUAL 2)
            list(GET arglist 0 NAME)
            list(GET arglist 1 VERSION)
            spl_run_pip(${NAME}==${VERSION})
            execute_process(
                COMMAND python -c "import pathlib, ${NAME}; print(pathlib.Path(${NAME}.__file__).resolve().parent, end='')"
                OUTPUT_VARIABLE ${NAME}_EXTENSION_PATH
            )
            set(${NAME}_CMAKE_MODULE_PATH ${${NAME}_EXTENSION_PATH}/cmake)
            list(APPEND CMAKE_MODULE_PATH ${${NAME}_CMAKE_MODULE_PATH})
            include(${NAME})
        else()
            message(FATAL_ERROR "Requested extension \"${arg}\" does not contain a version suffix with @.")
        endif()
    endforeach()

    spl_run_conan()
endmacro()

# deprecated
macro(add_include)
    spl_add_include(${ARGN})

    if(NOT add_include_warning)
        set(add_include_warning ON)
        message(WARNING "'add_include' is deprecated, use 'spl_add_include' instead. This warning is only printed once!")
    endif()
endmacro()

macro(add_source)
    spl_add_source(${ARGN})

    if(NOT add_source_warning)
        set(add_source_warning ON)
        message(WARNING "'add_source' is deprecated, use 'spl_add_source' instead. This warning is only printed once!")
    endif()
endmacro()

macro(create_component)
    spl_create_component(${ARGN})

    if(NOT create_component_warning)
        set(create_component_warning ON)
        message(WARNING "'create_component' is deprecated, use 'spl_create_component' instead. This warning is only printed once!")
    endif()
endmacro()
