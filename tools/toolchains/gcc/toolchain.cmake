set(CMAKE_C_COMPILER gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER g++ CACHE STRING "CXX Compiler")
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "ASM Compiler")

# MinGW on Windows needs its own include path and -mbig-obj.
# The Linux assembler does not know -mbig-obj, so both stay Windows-only.
if(CMAKE_HOST_WIN32)
    unset(CMAKE_C_COMPILER_FOUND_PATH)
    find_program(CMAKE_C_COMPILER_FOUND_PATH ${CMAKE_C_COMPILER})
    get_filename_component(CMAKE_C_COMPILER_FOUND_PATH ${CMAKE_C_COMPILER_FOUND_PATH} DIRECTORY)

    # CMAKE_C_COMPILER_FOUND_PATH is the path to the 'bin' directory
    set(COMPILER_SPECIFIC_INCLUDES
        -I${CMAKE_C_COMPILER_FOUND_PATH}/../x86_64-w64-mingw32/include
    )

    set(COMPILE_CXX_FLAGS "-Wa,-mbig-obj")
    add_compile_options(
        "$<$<COMPILE_LANGUAGE:CXX>:${COMPILE_CXX_FLAGS}>"
    )
endif()
